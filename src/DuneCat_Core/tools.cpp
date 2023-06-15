#include <DuneCatConfig.h>
#include "tools.h"
#include <string>
#include <cstdlib>
#include <charconv>
#include <chrono>
namespace DuneCat
{
//TODO: for linux make a more generic way to autostart an app (with .sh script after deployment)
bool bootUpStart(bool isOn)
{
#ifdef Q_OS_LINUX
    // Path to the autorun folder
    QString autostartPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0) + QLatin1String("/autostart");
    /* Check whether there is a directory in which to store the autorun file.*/
    QDir autorunDir(autostartPath);
    if(!autorunDir.exists()){
        autorunDir.mkpath(autostartPath);
    }
    QFile autorunFile(autostartPath + QLatin1String("/DuneCat.desktop"));
    /* Check the state of the checkbox, if checked, adds the application to the startup.
     * Otherwise remove */
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
                                                                                   "Hidden=flase\n"
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
                QProcess process;
                process.start("chmod +x",QStringList() << autostartPath + QLatin1String("/DuneCat.desktop"));
                return true;
            }
        }
    } else {
        // Delete the startup file
        if(autorunFile.exists()) autorunFile.remove();
    }

#elif defined(Q_OS_MAC)

    // Remove any existing login entry for this app first, in case there was one
    // from a previous installation, that may be under a different launch path.
    {
        QStringList args;
        args << "-e tell application \"System Events\" to delete login item\""
                    + macOSXAppBundleName() + "\"";

        QProcess::execute("osascript", args);
    }

    // Now install the login item, if needed.
    if ( isOn )
    {
        QStringList args;
        args << ("-e tell application \"System Events\" to make login item at end with properties {path:\"" + macOSXAppBundlePath() + "\", hidden:false}");

        QProcess::execute("osascript", args);
        return true;
    }
#elif defined(Q_OS_WIN)
    QSettings bootUpSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (isOn) {
        bootUpSettings.setValue(PROJECT_NAME, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        bootUpSettings.sync();
        return true;
    }
    bootUpSettings.remove(PROJECT_NAME);
    return false;
#endif
}

QString macOSXAppBundleName()
{
#ifdef Q_OS_MAC
    QString bundlePath = macOSXAppBundlePath();
    QFileInfo fileInfo(bundlePath);
    return fileInfo.baseName();
#else
    return "";
#endif
}

QString macOSXAppBundlePath()
{
#ifdef Q_OS_MAC
    QDir dir = QDir ( QCoreApplication::applicationDirPath() );
    dir.cdUp();
    dir.cdUp();
    QString absolutePath = dir.absolutePath();
    // absolutePath will contain a "/" at the end,
    // but we want the clean path to the .app bundle
    if ( absolutePath.length() > 0 && absolutePath.right(1) == "/" ) {
        absolutePath.chop(1);
    }
    return absolutePath;
#else
    return "";
#endif
}


#ifdef Q_OS_WIN
QDateTime fromBSTRToDateTime(BSTR bstr)
{
    int wslen = SysStringLen(bstr);
    int min_str_len{14};
    if(wslen < min_str_len)
        return QDateTime{};
    const wchar_t* pstr = (wchar_t*)bstr;
    int len = WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);
    std::string str(len, '\0');

    WideCharToMultiByte(CP_ACP, 0 /* no flags */,
                        pstr, wslen /* not necessary NULL-terminated */,
                        str.data(), len,
                        NULL, NULL /* no default char */);
    std::string_view str_view{str};
    int year{},month{},day{};
    std::from_chars(str_view.data(),str_view.data()+4,year);
    std::from_chars(str_view.data()+4,str_view.data()+6,month);
    std::from_chars(str_view.data()+6,str_view.data()+8,day);

    int hour{},min{},sec{},msec{};
    std::from_chars(str_view.data()+8,str_view.data()+10,hour);
    std::from_chars(str_view.data()+10,str_view.data()+12,min);
    std::from_chars(str_view.data()+12,str_view.data()+14,sec);
    std::from_chars(str_view.data()+15,str_view.data()+18,msec);

    QDateTime result(QDate(year,month,day),QTime(hour,min,sec,msec));
    if(result > QDateTime::currentDateTime())
        return QDateTime{};
    return result;
}
#endif
}
