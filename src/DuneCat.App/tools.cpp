#include "tools.h"
#include <qglobal.h>
#include <QtCore>
#include <DuneCatConfig.h>

bool tools::bootUpStart(bool isOn)
{
    #ifdef Q_OS_WIN
    QSettings bootUpSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString app_path = QCoreApplication::applicationFilePath();
    if (isOn) {
        bootUpSettings.setValue(PROJECT_NAME, app_path);
    } else {
        bootUpSettings.remove(PROJECT_NAME);
    }
    #endif
    return false;
}
