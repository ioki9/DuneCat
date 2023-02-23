#ifndef DCCONNECTIONMANAGER_H
#define DCCONNECTIONMANAGER_H

#include <QObject>
#include "DCStunClient.h"
#include "dctrackermanager.h"

class DCTrackerManager : public QObject
    {
        Q_OBJECT
    public:
        explicit DCTrackerManager(const DCEndPoint& tracker_addr,QObject* parent = nullptr);
        std::unique_ptr<DCStunClient> socket;
        DCEndPoint m_tracker;
        DCTrackerAnnounce m_announce{};
        DCTrackerConnect::Request m_connect{};

    private slots:
        bool connectToTracker();
};

#endif // DCCONNECTIONMANAGER_H
