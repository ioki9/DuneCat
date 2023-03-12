import QtQuick
import QtQuick.Controls
import DCBase

Window {
    color: "white"
    id: window

    width: 640
    height: 480
    visible: true
    title: qsTr("DuneCat")
    DCLaunchButton
    {
        id: launchButton
        width:300
        height:300
        anchors.centerIn: parent
        onClicked:{
            DCSettings.autoStart = !DCSettings.autoStart
        }

    }
    ToolButton
    {
        icon.name: "drawer"
        icon.height: 25
        icon.width: 25
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 10
        anchors.topMargin: 10
        onClicked: drawer.open()
    }

    Drawer {
        id: drawer
        width: Math.min(window.width, window.height) / 3 * 2.2
        height: window.height
    }
}
