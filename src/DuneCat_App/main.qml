import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle

DCMainWindow
{
    id: root
    visibility: visibility = Settings.window_maximized ? Window.Maximized : Window.Windowed
    width: width = Settings.window_width
    height: height = Settings.window_height
    Connections {
          target: SystemTray
          // Сигнал - показать окно
          function onShow() {
              root.show();
          }

          // The signal - close the application by ignoring the check-box
          function onQuit() {
              close();
          }

          // Minimize / maximize the window by clicking on the default system tray
          function onIcon_activated() {
               if(root.visibility === Window.Hidden) {
                   root.show()
               } else {
                   root.hide()
               }
          }
      }
    Binding{
        target: Settings
        property: "window_height"
        value: root.height
    }

    Binding{
        target: Settings
        property: "window_width"
        value: root.width
    }

    Binding{
        target: Settings
        property: "window_maximized"
        value: visibility === Window.Maximized
    }

    property list<string> pageUrlList:[]
    function startupFunction()
    {
        pageUrlList[DCAdminPanel.Home] = "qrc:/DuneCat/imports/qml/pages/DCMainPage.qml";
        pageUrlList[DCAdminPanel.Settings] = "qrc:/DuneCat/imports/qml/pages/DCSettingsPage.qml";
    }

    Component.onCompleted:{
        startupFunction()
    }
    DCAdminPanel{
        id:adminPanel
        width: 180
        listSpacing: 5
        z:5
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
    }
    Rectangle{
        id:page
        anchors.left: adminPanel.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: parent.right
        Loader{
            id:pageLoader
            source: root.pageUrlList[adminPanel.activePage]
            anchors.fill: parent
        }
    }
}

