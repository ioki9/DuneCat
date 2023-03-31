#include "dcprocesstracker.h"
#include <iostream>


DCProcessTracker::DCProcessTracker(QObject *parent)
    : QObject{parent}
{
    m_active_processes.push_back(ProcessInfo{});
}

void DCProcessTracker::get_process_list()
{
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;
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
    if(!m_active_processes.isEmpty())
        m_active_processes.clear();

    do
    {
        ProcessInfo process{};

        // Retrieve the priority class.
        dwPriorityClass = 0;
        hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
        if( hProcess == NULL )
            qDebug()<<"hprocess == null";
        else
        {

            dwPriorityClass = GetPriorityClass( hProcess );
            if( !dwPriorityClass )
                qDebug()<<"GetPriorityClass";
            CloseHandle( hProcess );
        }
        process.name = QString::fromWCharArray(pe32.szExeFile);
        process.process_id = pe32.th32ProcessID;
        process.thread_count = pe32.cntThreads;
        process.parent_process_id = pe32.th32ParentProcessID;
        process.priority_base = pe32.pcPriClassBase;
        process.prority_class = dwPriorityClass;
        m_active_processes.push_back(std::move(process));

    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
}
