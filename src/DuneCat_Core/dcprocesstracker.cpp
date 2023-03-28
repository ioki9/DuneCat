#include "dcprocesstracker.h"
#include "essentialheaders.h"
#include <iostream>
#include <string>
#ifdef Q_OS_WINDOWS
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#endif

DCProcessTracker::DCProcessTracker(QObject *parent)
    : QObject{parent}
{

}

void DCProcessTracker::get_process_list()
{
//    QProcess *process = new QProcess();
//    QString result;
//    process->start(QString("cmd"),QStringList()<<"start cmd");
//    if(!process->waitForFinished())
//        qDebug()<<"error";
//    else
//      result = process->readAllStandardOutput();
//    qDebug()<<result;
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        qDebug()<<"invalid handle value";
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
      qDebug()<<"Process32First"; // show cause of failure
      CloseHandle( hProcessSnap );
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do
    {
      _tprintf( TEXT("\n\n=====================================================" ));
      _tprintf( TEXT("\nPROCESS NAME:  %s" ), pe32.szExeFile );
      _tprintf( TEXT("\n-------------------------------------------------------" ));
      char ch[260];
      char DefChar = ' ';
      WideCharToMultiByte(CP_ACP,0,pe32.szExeFile,-1, ch,260,&DefChar, NULL);
      std::cout<<ch;
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

      _tprintf( TEXT("\n  Process ID        = 0x%08X"), pe32.th32ProcessID );
      _tprintf( TEXT("\n  Thread count      = %d"),   pe32.cntThreads );
      _tprintf( TEXT("\n  Parent process ID = 0x%08X"), pe32.th32ParentProcessID );
      _tprintf( TEXT("\n  Priority base     = %d"), pe32.pcPriClassBase );
      if( dwPriorityClass )
        _tprintf( TEXT("\n  Priority class    = %d"), dwPriorityClass );

      // List the modules and threads associated with this process

    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
}
