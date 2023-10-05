#include <QApplication>
#include <QQmlApplicationEngine>
#include <QObject>
#include <QtQml/qqmlregistration.h>
#include <QString>
#include <QtGlobal>
#include <QProcess>
#include <QtCore>
#include <QLocale>
#include <QIcon>
#include <vector>
#include <utility>
#ifdef Q_OS_WINDOWS
#define _WIN32_DCOM
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "wbemuuid.lib")
#endif //Q_OS_WINDOWS
#include <thread>
#include <set>
#include <functional>
#include <chrono>
