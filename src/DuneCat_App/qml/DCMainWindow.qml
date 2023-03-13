import QtQuick
import QtQuick.Controls
import DCBase
import "controls/"

ApplicationWindow {
    color: "white"
    visible: true
    signal drawerButtonClicked()
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
        onClicked: drawerButtonClicked()
    }


}
