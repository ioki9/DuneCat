#ifndef WMICLIENT_H
#define WMICLIENT_H
#include "essentialheaders.h"
#include "dcprocessinfo.h"
class WMIEventSink;
class WMIClient : public QObject
{
    Q_DISABLE_COPY_MOVE(WMIClient)
    Q_OBJECT
public:
    explicit WMIClient(QObject* parent = nullptr);
    ~WMIClient();
    friend class WMIEventSink;
private:
    HRESULT m_hres;
    IWbemLocator *m_pLoc = nullptr;
    IWbemServices *m_pSvc = nullptr;
    WMIEventSink* m_pSink = nullptr;
    IUnsecuredApartment* m_pUnsecApp = nullptr;
    IUnknown* m_pStubUnk = nullptr;
    IWbemObjectSink* m_pStubSink = nullptr;
    void process_created(DCProcessInfo& proc);
};

#endif // WMICLIENT_H
