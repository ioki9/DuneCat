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

    HRESULT hr;
    for (int i = 0; i < lObjectCount; i++)
    {
        VARIANT vtProp;
        BSTR strInstanceProp = SysAllocString(L"TargetInstance");
        hr = apObjArray[i]->Get(strInstanceProp,0,&vtProp,NULL,NULL);
        SysFreeString(strInstanceProp);
        if(SUCCEEDED(hr))
        {
            IUnknown* inst = vtProp.punkVal;
            DCProcessInfo proc_info;
            hr = inst->QueryInterface(IID_IWbemClassObject,reinterpret_cast<void**>(&apObjArray[i]));
            if(SUCCEEDED(hr))
            {
                VARIANT vtVal;
                hr = apObjArray[i]->Get(L"Name",0,&vtVal,NULL,NULL);
                if(SUCCEEDED(hr))
                    proc_info.name = QString::fromWCharArray(vtVal.bstrVal);
                VariantClear(&vtVal);
                hr = apObjArray[i]->Get(L"ProcessId",0,&vtVal,NULL,NULL);
                if(SUCCEEDED(hr))
                    proc_info.pid = vtVal.uintVal;
                hr = apObjArray[i]->Get(L"CreationDate",0,&vtVal,NULL,NULL);
                if(SUCCEEDED(hr))
                    proc_info.creation_date = tools::fromBSTRToDateTime(vtVal.bstrVal);
                VariantClear(&vtVal);
                hr = apObjArray[i]->Get(L"ExecutablePath",0,&vtVal,NULL,NULL);
                if(SUCCEEDED(hr))
                    proc_info.exe_path = QString::fromWCharArray(vtVal.bstrVal);
                VariantClear(&vtVal);
            }
            m_wmiClient->process_created(proc_info);
            inst->Release();
        }
        VariantClear(&vtProp);

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
