#include "wmiclient.h"
#include "wmieventsink.h"

QObject* WMIClient::client = nullptr;
WMIClient::WMIClient(QObject* parent) : QObject(parent)
{
    initialize();
}


WMIClient::~WMIClient()
{
    m_pSvc->CancelAsyncCall(m_pStubSink);
    m_pSvc->Release();
    m_pLoc->Release();
    m_pUnsecApp->Release();
    m_pStubUnk->Release();
    m_pSink->Release();
    m_pStubSink->Release();
    CoUninitialize();
}
void WMIClient::process_created(const DCProcessInfo& proc)
{
    emit new_process_created(proc);
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
    if (FAILED(hres))
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
    hres = m_pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &m_pSvc
        );

    if (FAILED(hres))
    {
        qDebug() << "Could not connect. Error code = 0x"
                 << std::hex << hres << '\n';
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

    // Receive event notifications -----------------------------
    // Use an unsecured apartment for security
    hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL,
                            CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment,
                            (void**)&m_pUnsecApp);

    m_pSink = new WMIEventSink(this);
    m_pSink->AddRef();

    m_pUnsecApp->CreateObjectStub(m_pSink, &m_pStubUnk);
    m_pUnsecApp->CreateObjectStub(m_pSink, &m_pStubUnk2);

    m_pStubUnk->QueryInterface(IID_IWbemObjectSink,
                               (void **) &m_pStubSink);
    m_pStubUnk2->QueryInterface(IID_IWbemObjectSink,
                               (void **) &m_pStubSink2);
    BSTR bstr_wql = SysAllocString(L"WQL" );
    BSTR bstr_sql = SysAllocString(L"SELECT * "
                                   "FROM __InstanceCreationEvent WITHIN 1 "
                                   "WHERE TargetInstance ISA 'Win32_Process'" );
    BSTR bstr_sql2 = SysAllocString(L"SELECT * "
                                   "FROM __InstanceDeletionEvent WITHIN 1 "
                                   "WHERE TargetInstance ISA 'Win32_Process'" );
    // The ExecNotificationQueryAsync method will call
    // The EventQuery::Indicate method when an event occurs
    hres = m_pSvc->ExecNotificationQueryAsync(
        bstr_wql,
        bstr_sql,
        WBEM_FLAG_SEND_STATUS,
        NULL,
        m_pStubSink);
    hres = m_pSvc->ExecNotificationQueryAsync(
        bstr_wql,
        bstr_sql2,
        WBEM_FLAG_SEND_STATUS,
        NULL,
        m_pStubSink2);
    SysReleaseString(bstr_wql);
    SysReleaseString(bstr_sql);
    // Check for errors.
    if (FAILED(hres))
    {
        printf("ExecNotificationQueryAsync failed "
               "with = 0x%X\n", hres);
        m_pSvc->Release();
        m_pLoc->Release();
        m_pUnsecApp->Release();
        m_pStubUnk->Release();
        m_pSink->Release();
        m_pStubSink->Release();
        CoUninitialize();
        return false;
    }
    return true;
}
