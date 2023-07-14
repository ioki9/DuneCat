import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle

DCMainWindow
{
    id: root
    width: width = Settings.window_width
    height: height = Settings.window_height

    Connections {
        target: root
        function onClosing(close) {
            if(root.visibility !== Window.Hidden)
            {
                close.accepted = false
                root.visibility = Window.Hidden
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
    function startupFunction(){
        pageUrlList[DCAdminPanel.Home] = "qrc:/DuneCat/imports/qml/pages/DCMainPage.qml";
        pageUrlList[DCAdminPanel.Settings] = "qrc:/DuneCat/imports/qml/pages/DCSettingsPage.qml";

        if(Qt.application.arguments[1] === "--hidden")
           root.visibility = Window.Hidden
        else if(Settings.window_maximized)
            root.visibility = Window.Maximized
        else
            root.visibility = Window.Windowed

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
            active: root.visibility === Window.Hidden ? false : true
            source: root.pageUrlList[adminPanel.activePage]
            anchors.fill: parent
        }
    }


}

