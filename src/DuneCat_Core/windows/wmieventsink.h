#pragma once
#include "essentialheaders.h"

namespace DuneCat
{
class WMIClient;
class WMIEventSink : public IWbemObjectSink
{

public:
    explicit WMIEventSink(WMIClient* client);
    ~WMIEventSink() { bDone = true; }

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT
        STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

    virtual HRESULT STDMETHODCALLTYPE Indicate(
        LONG lObjectCount,
        IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
        );

    virtual HRESULT STDMETHODCALLTYPE SetStatus(
        /* [in] */ LONG lFlags,
        /* [in] */ HRESULT hResult,
        /* [in] */ BSTR strParam,
        /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
        );
private:
    LONG m_lRef;
    bool bDone;
    WMIClient* m_wmiClient;


};
}
