#include "dcprocesstracker.h"
#include <utility>
#include "wmiclient.h"
DCProcessTracker::DCProcessTracker(QObject *parent)
    : QObject{parent}
{
    connect(WMIClient::get_instance(),&WMIClient::new_process_created,
            this,&DCProcessTracker::process_created_recieved);
}

QVector<DCProcessInfo> DCProcessTracker::get_active_processes()
{
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;
    QVector<DCProcessInfo> active_processes{};
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
        qDebug()<<"invalid hProcessSnap value";

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        qDebug()<<"Process32First failure";
        CloseHandle( hProcessSnap );
    }

    do
    {
        DCProcessInfo process{};

        // Retrieve the priority class.
        dwPriorityClass = 0;
        hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
        if( hProcess != NULL )
        {
            dwPriorityClass = GetPriorityClass( hProcess );
            if( !dwPriorityClass )
                qDebug()<<"GetPriorityClass";
            CloseHandle( hProcess );
        }
        process.name = QString::fromWCharArray(pe32.szExeFile);
        process.pid = pe32.th32ProcessID;
//        process.thread_count = pe32.cntThreads;
//        process.priority_base = pe32.pcPriClassBase;
//        process.prority_class = dwPriorityClass;
        active_processes.push_back(std::move(process));

    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
    return active_processes;
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

void DCProcessTracker::process_created_recieved(const DCProcessInfo& process)
{
    if(m_process_count != -1)
        m_process_count++;

    emit new_process_created(process);
}
