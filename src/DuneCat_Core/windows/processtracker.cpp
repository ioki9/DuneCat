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

namespace DuneCat
{

ProcessTracker::ProcessTracker(QObject *parent)
    : QObject{parent}
{
    connect(WMIClient::get_instance(),&WMIClient::new_process_created,
            this,&ProcessTracker::process_created_recieved);
    connect(WMIClient::get_instance(),&WMIClient::process_deleted,
            this,&ProcessTracker::process_deleted_recieved);
}

ProcessTracker::~ProcessTracker()
{
}

// returns empty vector if failed
std::vector<ProcessInfo> ProcessTracker::get_process_list()
{
    //process count updates through signals if we already set it once before.
    if(m_process_count != -1)
        return ProcessTracker::get_winapi_process_list();

    std::vector<ProcessInfo> vec = ProcessTracker::get_winapi_process_list();
    if(!vec.empty())
        m_process_count = vec.size();
    return vec;
}
//returns -1 if failed
int ProcessTracker::get_process_count()
{
    if(m_process_count != -1)
        return m_process_count;
    int count{};
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        qDebug()<<"invalid hProcessSnap value";
        return -1;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        qDebug()<<"Process32First failure";
        CloseHandle( hProcessSnap );
        return -1;
    }

    do
    {
        count++;
    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
    m_process_count = count;
    return count;
}
std::vector<ProcessInfo> ProcessTracker::get_winapi_process_list()
{
    HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe32;
    std::vector<ProcessInfo> result;
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

    do
    {
        ProcessInfo info;
        HANDLE process_handle = NULL;
        WCHAR filename[MAX_PATH];
        process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (process_handle != NULL)
        {
            if (GetModuleFileNameExW(process_handle, NULL, filename, MAX_PATH) != 0)
                info.file_path = QString::fromWCharArray(filename);

            FILETIME start,exit,kernel,user;
            if(GetProcessTimes(process_handle,&start,&exit,&kernel,&user) != 0)
            {
                SYSTEMTIME sys_time;
                FileTimeToSystemTime(&start,&sys_time);
                info.creation_date = QDateTime(QDate(sys_time.wYear,sys_time.wMonth,sys_time.wDay),
                                               QTime(sys_time.wHour,sys_time.wMinute,sys_time.wSecond,sys_time.wMilliseconds),
                                               Qt::UTC,0).toLocalTime();
            }
            HANDLE token_handle = NULL;
            if( OpenProcessToken( process_handle, TOKEN_QUERY, &token_handle ) != FALSE )
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

//        if(!info.file_path.isEmpty())
//        {
//            wchar_t *lol = new wchar_t[ info.file_path.length() + 1];
//            info.file_path.toWCharArray(lol);
//            lol[info.file_path.length()] = '\0';
//            info.icon = QPixmap::fromImage(QImage::fromHICON(ExtractIcon(GetModuleHandle(NULL),lol,0)));
//            delete[] lol;
//        }
        result.push_back(std::move(info));
        CloseHandle(process_handle);
    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
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
    if(!GetFileVersionInfo(filename.get(), NULL, dwLen, sKey.get()))
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

void ProcessTracker::process_deleted_recieved(const ProcessInfo &process)
{
    if(m_process_count != -1)
        m_process_count--;
    emit process_deleted(process);
}

void ProcessTracker::process_created_recieved(const ProcessInfo& process)
{
    if(m_process_count != -1)
        m_process_count++;
    emit process_created(process);
}
}
