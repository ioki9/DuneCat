#pragma once
#include "essentialheaders.h"

namespace DuneCat
{
class WMIClient;
class WMIEventSink : public IWbemObjectSink
{

public:
    explicit WMIEventSink(WMIClient* client);
    WMIEventSink (const WMIEventSink&) = delete;
    WMIEventSink& operator= (const WMIEventSink&) = delete;
    ~WMIEventSink() { bDone = true; }

    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT
        STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override;

    HRESULT STDMETHODCALLTYPE Indicate(
        LONG lObjectCount,
        IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
        ) override;

    HRESULT STDMETHODCALLTYPE SetStatus(
        /* [in] */ LONG lFlags,
        /* [in] */ HRESULT hResult,
        /* [in] */ BSTR strParam,
        /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
        ) override;
private:
    LONG m_lRef;
    bool bDone;
    WMIClient* m_wmiClient;


};
}
