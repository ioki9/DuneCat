#include "dcprocesstracker.h"
#include "wmiclient.h"
#include <utility>
#include <vector>
#include <winver.h>
#include <strsafe.h>
#pragma comment(lib,"Version.lib")

DCProcessTracker::DCProcessTracker(QObject *parent)
    : QObject{parent}
{
    connect(WMIClient::get_instance(),&WMIClient::new_process_created,
            this,&DCProcessTracker::process_created_recieved);
    connect(WMIClient::get_instance(),&WMIClient::process_deleted,
            this,&DCProcessTracker::process_deleted_recieved);
}

DCProcessTracker::~DCProcessTracker()
{
}

// returns empty vector if failed
std::vector<DCProcessInfo> DCProcessTracker::get_process_list()
{
    //process count updates through signals if we already set it once before.
    if(m_process_count != -1)
        return WMIClient::get_instance()->get_process_list();

    std::vector<DCProcessInfo> vec =  WMIClient::get_instance()->get_process_list();
    if(vec.size()!=0)
        m_process_count = vec.size();
    return vec;
}
//returns -1 if failed
int DCProcessTracker::get_process_count()
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
    return count;
}

QString DCProcessTracker::get_process_description(QString filepath)
{
    if(filepath.isEmpty())
        return "";
    std::unique_ptr<WCHAR[]>filename = std::make_unique<WCHAR[]>(filepath.size()+1);
    filepath.toWCharArray(filename.get());
    int dwLen = GetFileVersionInfoSize(filename.get(), NULL);
    if(!dwLen)
        return "";

    std::unique_ptr<BYTE[]> sKey = std::make_unique<BYTE[]>(dwLen);
    if(!GetFileVersionInfo(filename.get(), NULL, dwLen, sKey.get()))
        return "";

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate;

    UINT cbTranslate = 0;
    if(!VerQueryValue(sKey.get(), L"\\VarFileInfo\\Translation",
                       (LPVOID*)&lpTranslate, &cbTranslate))
        return "";

    QString description{""};
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
            description = QString::fromWCharArray(wdescription);;
        break;
    }
    return description;
}

void DCProcessTracker::process_deleted_recieved(const DCProcessInfo &process)
{
    if(m_process_count != -1)
        m_process_count--;
    emit process_deleted(process);
}

void DCProcessTracker::process_created_recieved(const DCProcessInfo& process)
{
    if(m_process_count != -1)
        m_process_count++;
    emit process_created(process);
}
