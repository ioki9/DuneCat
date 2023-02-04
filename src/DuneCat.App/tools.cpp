#include "tools.h"
#include <qglobal.h>
#include <QtCore>
#include <DuneCatConfig.h>

bool tools::bootUpStart(bool isOn)
{
    #ifdef Q_OS_WIN
    QSettings bootUpSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QString base_dir = qApp->applicationDirPath() + '\\' + PROJECT_NAME + ".exe";
    
    if (isOn) {
        bootUpSettings.setValue(PROJECT_NAME,"\""+base_dir+"\"");
    } else {
        bootUpSettings.remove(PROJECT_NAME);
    }
    #endif
    return false;
}
