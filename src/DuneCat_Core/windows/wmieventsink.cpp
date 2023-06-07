#include "wmieventsink.h"
#include <strsafe.h>
#include "dcprocessinfo.h"
#include "dctools.h"
#include "wmiclient.h"

WMIEventSink::WMIEventSink(WMIClient *client) : m_wmiClient {client}
{
    m_lRef = 0;
}

ULONG WMIEventSink::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

ULONG WMIEventSink::Release()
{
    LONG lRef = InterlockedDecrement(&m_lRef);
    if(lRef == 0)
        delete this;
    return lRef;
}

HRESULT WMIEventSink::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
    {
        *ppv = (IWbemObjectSink *) this;
        AddRef();
        return WBEM_S_NO_ERROR;
    }
    else return E_NOINTERFACE;
}


HRESULT WMIEventSink::Indicate(long lObjectCount,
    IWbemClassObject **apObjArray)
{
    for (int i = 0; i < lObjectCount; i++)
    {
        m_wmiClient->handle_event(apObjArray[i]);
    }
    return WBEM_S_NO_ERROR;
}

HRESULT WMIEventSink::SetStatus(
            /* [in] */ LONG lFlags,
            /* [in] */ HRESULT hResult,
            /* [in] */ BSTR strParam,
            /* [in] */ IWbemClassObject __RPC_FAR *pObjParam)
{
    if(lFlags == WBEM_STATUS_COMPLETE)
    {
        printf("Call complete. hResult = 0x%X\n", hResult);
    }
    else if(lFlags == WBEM_STATUS_PROGRESS)
    {
        printf("Call in progress.\n");
    }

    return WBEM_S_NO_ERROR;
}
