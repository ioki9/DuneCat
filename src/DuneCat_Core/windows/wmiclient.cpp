#include "wmiclient.h"
#include "wmieventsink.h"
#include "tools.h"
#include "processtracker.h"
#include <vector>
#include <chrono>
#include <iostream>
#include <psapi.h>
#include <oleauto.h>
#include <strsafe.h>
#define MAX_NAME 256

namespace DuneCat
{
QObject* WMIClient::client = nullptr;
WMIClient::WMIClient(QObject* parent) : QObject(parent)
{
    if(!initialize())
        qDebug()<<"WMIClient initialization failed.";
}

WMIClient::~WMIClient()
{
    if(is_initialized)
    {
        m_pLoc->Release();
        m_pUnsecApp->Release();
        m_pSink->Release();
        for(auto* stubUnk : m_pStubUnkList)
            stubUnk->Release();
        for(auto* stubSink : m_pStubSinkList)
        {
            m_pSvc->CancelAsyncCall(stubSink);
            stubSink->Release();
        }
        m_pSvc->Release();
        CoUninitialize();
    }
}


void WMIClient::handle_event(IWbemClassObject *obj)
{
    //    else if(wcscmp(vtProp.bstrVal, L"__InstanceDeletionEvent"))
    //        handle_process_deletion(obj);
    HRESULT hr;
    IUnknown* inst = nullptr;
    ProcessInfo proc_info{};
    VARIANT vtProp;
    VARIANT vtClassProp;
    BSTR strClassProp = SysAllocString(L"__CLASS");//__CLASS
    obj->Get(strClassProp,0,&vtClassProp,NULL,NULL);
    SysFreeString(strClassProp);
    BSTR strInstanceProp = SysAllocString(L"TargetInstance");//__CLASS
    hr = obj->Get(strInstanceProp,0,&vtProp,NULL,NULL);
    SysFreeString(strInstanceProp);

    if(SUCCEEDED(hr))
    {
        inst = vtProp.punkVal;
        hr = inst->QueryInterface(IID_IWbemClassObject,reinterpret_cast<void**>(&obj));
        if(SUCCEEDED(hr))
        {
            VARIANT vtVal;
            proc_info = get_process_stats(obj);
            if(QString::fromWCharArray(vtClassProp.bstrVal) == QString("__InstanceCreationEvent"))
            {
                BSTR str_user;
                BSTR str_domain;
                if(SUCCEEDED(get_user_from_process(proc_info.pid,str_user,str_domain)))
                {
                    proc_info.owner_user = QString::fromWCharArray(str_user);
                    proc_info.owner_domain = QString::fromWCharArray(str_domain);
                    if(str_user)
                        SysFreeString(str_user);
                    if(str_domain)
                        SysFreeString(str_domain);
                }
                emit new_process_created(proc_info);
            }
            else if(QString::fromWCharArray(vtClassProp.bstrVal) == "__InstanceDeletionEvent")
            {
                hr = obj->Get(L"TerminationDate",0,&vtVal,NULL,NULL);
                if(SUCCEEDED(hr))
                    proc_info.termination_time = QDateTime::currentDateTime();
                VariantClear(&vtVal);
                emit process_deleted(proc_info);
            }
        }
        inst->Release();
    }
    VariantClear(&vtProp);
    VariantClear(&vtClassProp);
}


void WMIClient::handle_process_deletion(IWbemClassObject *obj)
{

}

bool WMIClient::initialize()
{
    // Initialize COM.
    HRESULT hres;
    hres =  CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    if (FAILED(hres))
    {
        qDebug()<< "Failed to initialize COM library. Error code = 0x"
                 << std::hex << hres << "\n";
        return false;// Program has failed.
    }
    // Set general COM security levels
    hres =  CoInitializeSecurity(
        NULL,
        -1,                          // COM negotiates service
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities
        NULL                         // Reserved
        );
    if (FAILED(hres) && (hres != RPC_E_TOO_LATE))
    {
        qDebug() << "Failed to initialize security. Error code = 0x"
                 << std::hex << hres << '\n';
        CoUninitialize();
        return false;
    }
    // Obtain the initial locator to WMI
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *) &m_pLoc);

    if (FAILED(hres))
    {
        qDebug() << "Failed to create IWbemLocator object. "
                 << "Err code = 0x"
                 << std::hex << hres << '\n';
        CoUninitialize();
        return false;
    }

    // Connect to WMI through the IWbemLocator::ConnectServer method
    // Connect to the local root\cimv2 namespace
    // and obtain pointer pSvc to make IWbemServices calls.
    BSTR root_str = SysAllocString(L"ROOT\\CIMV2");
    hres = m_pLoc->ConnectServer(
        root_str,
        NULL,
        NULL,
        0,
        0,
        0,
        0,
        &m_pSvc
        );
    SysFreeString(root_str);
    if (FAILED(hres))
    {
        qDebug() << "Could not connect to root\\cimv2 namespace. Error code = "<< std::hex << hres << '\n';
        m_pLoc->Release();
        CoUninitialize();
        return false;
    }

    qDebug() << "Connected to ROOT\\CIMV2 WMI namespace" << '\n';

    // Set security levels on the proxy
    hres = CoSetProxyBlanket(
        m_pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        NULL,                        // Server principal name
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities
        );

    if (FAILED(hres))
    {
        qDebug() << "Could not set proxy blanket. Error code = 0x"
                 << std::hex << hres << '\n';
        m_pSvc->Release();
        m_pLoc->Release();
        CoUninitialize();
        return false;
    }
    //Get boot time
    BSTR bstrWql = SysAllocString(L"WQL" );
    BSTR bootQuery = SysAllocString(L"SELECT * FROM Win32_OperatingSystem");
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = m_pSvc->ExecQuery(bstrWql,
                             bootQuery,
                             WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                             NULL,
                             &pEnumerator);
    SysFreeString(bstrWql);
    SysFreeString(bootQuery);
    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
                                       &pclsObj, &uReturn);
        if(0 == uReturn)
        {
            break;
        }
        VARIANT vtProp;
        VariantInit(&vtProp);
        //Get the value
        hr = pclsObj->Get(L"LastBootUpTime", 0, &vtProp, 0, 0);
        m_boot_time = fromBSTRToDateTime(vtProp.bstrVal);
        VariantClear(&vtProp);
        pclsObj->Release();
    }

    // Receive event notifications -----------------------------
    // Use an unsecured apartment for security
    CoCreateInstance(CLSID_UnsecuredApartment, NULL,
                            CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment,
                            (void**)&m_pUnsecApp);

    m_pSink = new WMIEventSink(this);
    m_pSink->AddRef();
    BSTR creationQuery = SysAllocString(L"SELECT * "
                                        "FROM __InstanceCreationEvent WITHIN 1 "
                                        "WHERE TargetInstance ISA 'Win32_Process'" );
    if(!subscribe_to_event(creationQuery))
    {
        SysFreeString(creationQuery);
        return false;
    }
    SysFreeString(creationQuery);

    BSTR deletionQuery = SysAllocString(L"SELECT * "
                                        "FROM __InstanceDeletionEvent WITHIN 1 "
                                        "WHERE TargetInstance ISA 'Win32_Process'"  );
    if(!subscribe_to_event(deletionQuery))
    {
        SysFreeString(deletionQuery);
        return false;
    }
    SysFreeString(deletionQuery);
    is_initialized = true;
    return true;
}

ProcessInfo WMIClient::get_process_stats(IWbemClassObject *obj)
{
    HRESULT hr;
    ProcessInfo proc_info;
    VARIANT vtVal;
    hr = obj->Get(L"Name",0,&vtVal,NULL,NULL);
    if(SUCCEEDED(hr))
        proc_info.name = QString::fromWCharArray(vtVal.bstrVal);
    VariantClear(&vtVal);

    hr = obj->Get(L"ProcessId",0,&vtVal,NULL,NULL);
    if(SUCCEEDED(hr))
        proc_info.pid = vtVal.uintVal;
    VariantClear(&vtVal);

    hr = obj->Get(L"CreationDate",0,&vtVal,NULL,NULL);
    if(SUCCEEDED(hr))
        proc_info.creation_time = fromBSTRToDateTime(vtVal.bstrVal);
    VariantClear(&vtVal);

    hr = obj->Get(L"ExecutablePath",0,&vtVal,NULL,NULL);
    if(SUCCEEDED(hr))
        proc_info.file_path = QString::fromWCharArray(vtVal.bstrVal);
    VariantClear(&vtVal);

    hr = obj->Get(L"CommandLine",0,&vtVal,NULL,NULL);
    if(SUCCEEDED(hr))
        proc_info.command_line = QString::fromWCharArray(vtVal.bstrVal);
    VariantClear(&vtVal);
    proc_info.description = ProcessTracker::get_process_description(proc_info.file_path);
    return proc_info;
}

bool WMIClient::subscribe_to_event(BSTR event_query)
{
    HRESULT hres;
    IUnknown* pStubUnk;
    m_pUnsecApp->CreateObjectStub(m_pSink, &pStubUnk);

    m_pStubUnkList.push_back(pStubUnk);
    IWbemObjectSink* pStubSink;
    pStubUnk->QueryInterface(IID_IWbemObjectSink, (void **) &pStubSink);
    m_pStubSinkList.push_back(pStubSink);
    BSTR bstr_wql = SysAllocString(L"WQL" );

    // The ExecNotificationQueryAsync method will call
    // The EventQuery::Indicate method when an event occurs
    hres = m_pSvc->ExecNotificationQueryAsync(
        bstr_wql,
        event_query,
        WBEM_FLAG_SEND_STATUS,
        NULL,
        pStubSink);

    SysFreeString(bstr_wql);
    // Check for errors.
    if (FAILED(hres))
    {
        qDebug()<<"Couldn't subscribe to event. ExecNotificationQueryAsync failed "
                    "with error = 0x"<< Qt::hex<<hres<<'\n'<<"Event query:"<<QString::fromWCharArray(event_query);
        m_pSvc->Release();
        m_pLoc->Release();
        m_pUnsecApp->Release();
        m_pStubUnkList.pop_back();
        pStubUnk->Release();
        m_pSink->Release();
        m_pStubSinkList.pop_back();
        pStubSink->Release();
        CoUninitialize();
        return false;
    }
    return true;
}

std::vector<ProcessInfo> WMIClient::get_process_list()
{
    HRESULT hres;
    IEnumWbemClassObject* pEnumerator = NULL;
    BSTR wql = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM Win32_Process");
    hres = m_pSvc->ExecQuery( wql, query,
                             WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    SysFreeString(wql);
    SysFreeString(query);
    if (FAILED(hres))
    {
        qDebug() << "Couldn't get process list. ExecQuery failed" << " Error code = 0x"
                 << std::hex << hres << '\n';
        qDebug()<< _com_error(hres).ErrorMessage() << '\n';
        return {};            // Program has failed.
    }

    // Get the data from the WQL sentence
    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;
    std::vector<ProcessInfo> process_list{};
    while (pEnumerator)
    {
        ProcessInfo proc_info;
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if(0 == uReturn || FAILED(hr))
            break;

        VARIANT vtProp;
        proc_info = get_process_stats(pclsObj);
        BSTR str_user,str_domain;
        if(SUCCEEDED(get_user_from_process(proc_info.pid,str_user,str_domain)))
        {
            proc_info.owner_user = QString::fromWCharArray(str_user);
            proc_info.owner_domain = QString::fromWCharArray(str_domain);
            SysFreeString(str_user);
            SysFreeString(str_domain);
        }
        proc_info.description = ProcessTracker::get_process_description(proc_info.file_path);
        process_list.push_back(proc_info);
    }

    return process_list;
}

BOOL WMIClient::get_logon_from_token(HANDLE hToken, BSTR& strUser, BSTR& strdomain)
{
    DWORD dwSize = MAX_NAME;
    BOOL bSuccess = FALSE;
    DWORD dwLength = 0;
    PTOKEN_USER ptu = NULL;
    //Verify the parameter passed in is not NULL.
    if (NULL == hToken)
        return 0;

    if (!GetTokenInformation(
            hToken,         // handle to the access token
            TokenUser,    // get information about the token's groups
            NULL,   // pointer to PTOKEN_USER buffer
            0,              // size of buffer
            &dwLength       // receives required buffer size
            ))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            return 0;

        ptu = (PTOKEN_USER)HeapAlloc(GetProcessHeap(),
                                      HEAP_ZERO_MEMORY, dwLength);

        if (ptu == NULL)
            return 0;
    }

    if (!GetTokenInformation(
            hToken,         // handle to the access token
            TokenUser,    // get information about the token's groups
            (LPVOID) ptu,   // pointer to PTOKEN_USER buffer
            dwLength,       // size of buffer
            &dwLength       // receives required buffer size
            ))
    {
        return 0;
    }
    SID_NAME_USE SidType;
    WCHAR lpName [MAX_NAME];
    WCHAR lpDomain [MAX_NAME];

    if( !LookupAccountSidW(NULL, ptu->User.Sid, lpName, &dwSize, lpDomain, &dwSize, &SidType))
    {
        DWORD dwResult = GetLastError();
        if( dwResult == ERROR_NONE_MAPPED )
            qDebug()<<"NONE_MAPPED";
        else
        {
            qDebug()<<"LookupAccountSid Error \n"<<GetLastError();
        }
    }
    else
    {
        strUser = SysAllocString(lpName);
        strdomain = SysAllocString(lpDomain);
        bSuccess = TRUE;
    }
    //cleanup
    if (ptu != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)ptu);
    return bSuccess;
}

QDateTime WMIClient::get_sys_boot_time()
{
    return m_boot_time;
}

HRESULT WMIClient::get_user_from_process(const DWORD procId, BSTR& strUser, BSTR& strdomain)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ ,FALSE,procId);
    if(hProcess == NULL)
    {
        return E_FAIL;
    }
    HANDLE hToken = NULL;

    if( !OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_QUERY_SOURCE, &hToken ) )
    {
        CloseHandle( hProcess );
        return E_FAIL;
    }
    BOOL bres = get_logon_from_token(hToken, strUser, strdomain);

    CloseHandle( hToken );
    CloseHandle( hProcess );
    return bres?S_OK:E_FAIL;
}
}
