#include "processtracker.h"
#include "wmiclient.h"
#include <utility>
#include <vector>
#include <winver.h>
#include <strsafe.h>
#include <Psapi.h>
#include <windows.h>
#include <processthreadsapi.h>
#pragma comment(lib,"Version.lib")
#pragma comment(lib, "psapi.lib")

namespace DuneCat
{
bool set_debug_privilege(bool enable);
bool procinfo_less_pid_comp(const ProcessInfo& lhs,const ProcessInfo& rhs)
{
    return lhs.pid < rhs.pid;
}
ProcessTracker::ProcessTracker(QObject *parent)
    : QObject{parent}
{

    m_processes = gather_processes();
    if(m_processes.empty())
    {
        qFatal()<<"Couldn't gather process list. Exiting...";
        QCoreApplication::quit();
    }
    std::sort(m_processes.begin(),m_processes.end(),procinfo_less_pid_comp);
    connect(WMIClient::get_instance(),&WMIClient::new_process_created,this,
            &ProcessTracker::process_created_handler);
    connect(WMIClient::get_instance(),&WMIClient::process_deleted,this,
            &ProcessTracker::process_deleted_handler);

    connect(WMIClient::get_instance(),&WMIClient::new_process_created,
            this,&ProcessTracker::process_created);
    connect(WMIClient::get_instance(),&WMIClient::process_deleted,
            this,&ProcessTracker::process_deleted);

}

ProcessTracker::~ProcessTracker()
{
}

// returns empty vector if failed
std::vector<ProcessInfo> ProcessTracker::gather_processes()
{
    //process count updates through signals if we already set it once before.
    if(Q_LIKELY(m_process_count != 0))
        return m_processes;
    std::vector<ProcessInfo> vec = ProcessTracker::get_winapi_process_list();
    if(!vec.empty())
        m_process_count = vec.size();
    return vec;
}

//returns 0 if failed
int ProcessTracker::get_process_count()
{
    if(Q_LIKELY(m_process_count != 0))
        return m_process_count;
    int count{};
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        qDebug()<<"invalid hProcessSnap value";
        return 0;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        qDebug()<<"Process32First failure";
        CloseHandle( hProcessSnap );
        return 0;
    }

    do
    {
        count++;
    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
    m_process_count = count;
    return count;
}

void ProcessTracker::get_process_list(std::vector<ProcessInfo>& list_out) const
{
    std::shared_lock lck{proc_vec_mutex};
    list_out = m_processes;
}

std::vector<ProcessInfo> ProcessTracker::get_winapi_process_list()
{

    HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe32;
    std::vector<ProcessInfo> result;
    double elapsed_sum{0.0};
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        qDebug()<<"invalid hProcessSnap value while trying to get process list";
        return result;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        qDebug()<<"Process32First failure while trying to get process list";
        CloseHandle( hProcessSnap );
        return result;
    }
    if(!set_debug_privilege(true))
        qDebug()<<"failed";
    do
    {
        ProcessInfo info;
        HANDLE process_handle = NULL;
        WCHAR filename[MAX_PATH];
        process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (process_handle != NULL)
        {
            DWORD dwSize = sizeof(filename) / sizeof(*filename);
            if (QueryFullProcessImageName(process_handle, 0, filename, &dwSize) != 0)
                info.file_path = QString::fromWCharArray(filename);
            else
                qDebug()<<"couldn't get process path. Error:"<<GetLastError();
            FILETIME start,exit,kernel,user;
            if(GetProcessTimes(process_handle,&start,&exit,&kernel,&user) != 0)
            {
                SYSTEMTIME sys_time;
                FileTimeToSystemTime(&start,&sys_time);
                info.creation_time = QDateTime(QDate(sys_time.wYear,sys_time.wMonth,sys_time.wDay),
                                               QTime(sys_time.wHour,sys_time.wMinute,sys_time.wSecond,sys_time.wMilliseconds),
                                               Qt::UTC,0).toLocalTime();
            }

            HANDLE token_handle = NULL;
            if( OpenProcessToken( process_handle, TOKEN_QUERY | TOKEN_QUERY_SOURCE , &token_handle ) != FALSE )
            {
                BSTR bstr_user,bstr_domain;
                if(WMIClient::get_logon_from_token(token_handle, bstr_user, bstr_domain) != FALSE)
                {
                    info.owner_user = QString::fromWCharArray(bstr_user);
                    info.owner_domain = QString::fromWCharArray(bstr_domain);
                    SysFreeString(bstr_user);
                    SysFreeString(bstr_domain);
                }
            }

            CloseHandle( token_handle );
        }
        info.pid = pe32.th32ProcessID;
        info.name = QString::fromWCharArray(pe32.szExeFile);
        info.description = ProcessTracker::get_process_description(info.file_path);
        if(!info.creation_time.isValid())
            info.creation_time = WMIClient::get_instance()->get_sys_boot_time();

//        if(!info.file_path.isEmpty())
//        {
//            wchar_t *lol = new wchar_t[ info.file_path.length() + 1];
//            info.file_path.toWCharArray(lol);
//            lol[info.file_path.length()] = '\0';
//            info.icon = QPixmap::fromImage(QImage::fromHICON(ExtractIcon(GetModuleHandle(NULL),lol,0)));
//            delete[] lol;
//        }

        const auto start{std::chrono::steady_clock::now()};
        result.push_back(std::move(info));
        const auto end{std::chrono::steady_clock::now()};
        const std::chrono::duration<double> elapsed_seconds{end - start};
        elapsed_sum+=elapsed_seconds.count();
        CloseHandle(process_handle);
    } while( Process32Next( hProcessSnap, &pe32 ) );
    if(!set_debug_privilege(false))
        qDebug()<<"failed";
    CloseHandle( hProcessSnap );
    qDebug()<<elapsed_sum;
    return result;
}

QString ProcessTracker::get_process_description(QStringView filepath)
{
    if(filepath.isEmpty())
        return QLatin1String("");
    std::unique_ptr<WCHAR[]>filename = std::make_unique<WCHAR[]>(filepath.size()+1);
    filepath.toWCharArray(filename.get());
    int dwLen = GetFileVersionInfoSize(filename.get(), NULL);
    if(dwLen == 0)
        return "";

    std::unique_ptr<BYTE[]> sKey = std::make_unique<BYTE[]>(dwLen);
    if(!GetFileVersionInfo(filename.get(), 0, dwLen, sKey.get()))
        return "";

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate;

    UINT cbTranslate {0};
    if(!VerQueryValue(sKey.get(), L"\\VarFileInfo\\Translation",
                       (LPVOID*)&lpTranslate, &cbTranslate))
        return "";

    QString description{QLatin1String("")};
    for(unsigned int i = 0; i < (cbTranslate / sizeof(LANGANDCODEPAGE)); i++)
    {
        if(lpTranslate[i].wLanguage != 1033)
            continue;
        WCHAR subblock[50];
        StringCchPrintf(subblock,50,L"\\StringFileInfo\\%04x%04x\\FileDescription",
                        lpTranslate[i].wLanguage,lpTranslate[i].wCodePage);
        WCHAR *wdescription = NULL;
        UINT dwBytes;
        if(VerQueryValue(sKey.get(), subblock, (LPVOID*)&wdescription, &dwBytes))
            description = QString::fromWCharArray(wdescription);
        break;
    }
    return description;
}

void ProcessTracker::process_created_handler(const ProcessInfo &process)
{
    std::unique_lock lck{proc_vec_mutex};
    m_process_count +=1;
    auto it = std::upper_bound(m_processes.begin(), m_processes.end(), process,
                               [](auto& value, auto& elem ) { return value.pid < elem.pid;});
    m_processes.insert(it, process);
}

void ProcessTracker::process_deleted_handler(const ProcessInfo &process)
{
    std::unique_lock lck{proc_vec_mutex};
    auto it = std::lower_bound(m_processes.begin(),m_processes.end(),process,
                               [](auto& value, auto& elem ) { return value.pid < elem.pid;});
    if(it == m_processes.end())
    {
        qDebug()<<"process not found. PID: "<<process.pid;
        return;
    }
    m_processes.erase(it);
    m_process_count -=1;
}

bool set_debug_privilege(bool enable)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES token_privileges;
    LUID luid;
    DWORD priv_mode = enable ? SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED;
    // Open the current process token
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        qDebug()<<"Failed to open process token";
        return false;
    }

    // Lookup the LUID for the SeDebugPrivilege privilege
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
    {
        qDebug() << "Failed to lookup privilege value";
        CloseHandle(hToken);
        return false;
    }

    token_privileges.PrivilegeCount = 1;
    token_privileges.Privileges[0].Luid = luid;
    token_privileges.Privileges[0].Attributes = priv_mode;

    // Enable the SeDebugPrivilege privilege
    if (!AdjustTokenPrivileges(hToken, FALSE, &token_privileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
        qDebug() << "Failed to adjust token privileges";
        CloseHandle(hToken);
        return false;
    }

    CloseHandle(hToken);
    return true;
}

}
