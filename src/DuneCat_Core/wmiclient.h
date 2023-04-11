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
    static WMIClient *get_instance(QObject *parent =nullptr){
        if(client)
            return qobject_cast<WMIClient*>(WMIClient::client);

        auto instance = new WMIClient(parent);
        client = instance;
        return  instance;
    }
    ~WMIClient();
    friend class WMIEventSink;
private:
    static QObject* client;
    explicit WMIClient(QObject* parent = nullptr);
    HRESULT m_hres;
    IWbemLocator *m_pLoc = nullptr;
    IWbemServices *m_pSvc = nullptr;
    WMIEventSink* m_pSink = nullptr;
    IUnsecuredApartment* m_pUnsecApp = nullptr;
    IUnknown* m_pStubUnk = nullptr;
    IUnknown* m_pStubUnk2 = nullptr;
    IWbemObjectSink* m_pStubSink = nullptr;
    IWbemObjectSink* m_pStubSink2 = nullptr;

    void process_created(const DCProcessInfo& proc);
    bool initialize();
signals:
    void new_process_created(const DCProcessInfo& new_process);
};

#endif // WMICLIENT_H
