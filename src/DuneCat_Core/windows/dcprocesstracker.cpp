#include "dcprocesstracker.h"
#include "wmiclient.h"
#include <utility>
#include <vector>

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
std::vector<DCProcessInfo> DCProcessTracker::get_active_processes()
{
    return WMIClient::get_instance()->get_process_list();
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
