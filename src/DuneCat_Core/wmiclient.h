#ifndef WMICLIENT_H
#define WMICLIENT_H
#include "essentialheaders.h"
#include "dcprocessinfo.h"
#include <QCoreApplication>
class WMIEventSink;
class WMIClient : public QObject
{
    Q_DISABLE_COPY_MOVE(WMIClient)
    Q_OBJECT
    friend class WMIEventSink;
public:
    static WMIClient *get_instance(){
        if(client)
            return qobject_cast<WMIClient*>(WMIClient::client);

        auto instance = new WMIClient(QCoreApplication::instance());
        client = instance;
        return  instance;
    }

    std::vector<DCProcessInfo> get_process_list();

private:
    ~WMIClient();
    static QObject* client;
    explicit WMIClient(QObject* parent = nullptr);
    IWbemLocator *m_pLoc = nullptr;
    IWbemServices *m_pSvc = nullptr;
    WMIEventSink* m_pSink = nullptr;
    IUnsecuredApartment* m_pUnsecApp = nullptr;
    QVector<IUnknown*> m_pStubUnkList;
    QVector<IWbemObjectSink*> m_pStubSinkList;
    bool subscribe_to_event(BSTR eventQuery);
    void handle_event(IWbemClassObject* obj);
    std::pair<QString,QString> get_process_user_domain(IWbemClassObject* obj);
    void handle_process_deletion(IWbemClassObject* obj);
    bool initialize();
signals:
    void process_deleted(const DCProcessInfo& process);
    void new_process_created(const DCProcessInfo& new_process);
};

#endif // WMICLIENT_H
