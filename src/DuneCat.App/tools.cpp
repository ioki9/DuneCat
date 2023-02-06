#include "tools.h"
#include <QtCore>
#include <DuneCatConfig.h>

#ifdef Q_OS_WIN
bool tools::bootUpStart(bool isOn)
{   
    QSettings bootUpSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (isOn) {
        bootUpSettings.setValue(PROJECT_NAME, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        bootUpSettings.sync();
    } else {
        bootUpSettings.remove(PROJECT_NAME);
    }
    return false;
}
#elif defined(Q_OS_UNIX)
bool tools::bootUpStart(bool isOn)
{
        // Path to the autorun folder
    QString autostartPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + QLatin1String("/autostart");
    /* Check whether there is a directory in which to store the autorun file.
     * And then you never know ... user deleted ...
     * */
    QDir autorunDir(autostartPath);
    if(!autorunDir.exists()){
        autorunDir.mkpath(autostartPath);
    }
    QFile autorunFile(autostartPath + QLatin1String("/DuneCat.desktop"));
    /* Check the state of the checkbox, if checked, adds the application to the startup.
     * Otherwise remove
     * */
    if(isOn) {
        // Next, check the availability of the startup file
        if(!autorunFile.exists()){
 
            /* Next, open the file and writes the necessary data
             * with the path to the executable file, using QCoreApplication::applicationFilePath()
             * */
            if(autorunFile.open(QFile::WriteOnly)){
 
                QString autorunContent("[Desktop Entry]\n"
                                       "Type=Application\n"
                                       "Exec=" + QCoreApplication::applicationFilePath() + "\n"
                                       "Hidden=true\n"
                                       "NoDisplay=false\n"
                                       "X-GNOME-Autostart-enabled=true\n"
                                       "Name[en_GB]=DuneCat\n"
                                       "Name=DuneCat\n"
                                       "Comment[en_GB]=DuneCat\n"
                                       "Comment=DuneCat\n");
                QTextStream outStream(&autorunFile);
                outStream << autorunContent;
                // Set access rights, including on the performance of the file, otherwise the autorun does not work
                autorunFile.setPermissions(QFileDevice::ExeUser|QFileDevice::ExeOwner|QFileDevice::ExeOther|QFileDevice::ExeGroup|
                                       QFileDevice::WriteUser|QFileDevice::ReadUser);
                autorunFile.close();
            }
        }
    } else {
        // Delete the startup file
        if(autorunFile.exists()) autorunFile.remove();
    }
}
#endif