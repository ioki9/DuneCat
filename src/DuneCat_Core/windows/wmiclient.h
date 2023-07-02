#pragma once
#include "essentialheaders.h"
#include "processinfo.h"
#include <QCoreApplication>

namespace DuneCat
{
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

        auto *instance = new WMIClient(QCoreApplication::instance());
        client = instance;
        return  instance;
    }
    std::vector<ProcessInfo> get_process_list();
    static BOOL get_logon_from_token(HANDLE hToken, BSTR &strUser, BSTR &strdomain);

private:
    explicit WMIClient(QObject* parent = nullptr);
    ~WMIClient() override;
    static QObject* client;
    IWbemLocator *m_pLoc = nullptr;
    IWbemServices *m_pSvc = nullptr;
    WMIEventSink* m_pSink = nullptr;
    IUnsecuredApartment* m_pUnsecApp = nullptr;
    QVector<IUnknown*> m_pStubUnkList;
    QVector<IWbemObjectSink*> m_pStubSinkList;
    bool is_initialized{false};
    bool subscribe_to_event(BSTR eventQuery);
    void handle_event(IWbemClassObject* obj);
    std::pair<QString,QString> get_process_user_domain(IWbemClassObject* obj);
    void handle_process_deletion(IWbemClassObject* obj);
    bool initialize();
    ProcessInfo get_process_stats(IWbemClassObject* obj);
    HRESULT get_user_from_process(const DWORD procId, BSTR &strUser, BSTR &strdomain);
signals:
    void process_deleted(const ProcessInfo& process);
    void new_process_created(const ProcessInfo& new_process);
};
}
