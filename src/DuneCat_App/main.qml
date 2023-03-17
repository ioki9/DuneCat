import QtQuick
import QtQuick.Controls
import DCBase

DCMainWindow
{
    palette.highlightedText: "white"
    width:1024

    height:480
    id: root
    onDrawerButtonClicked: drawer.open()
    DCAdminPanel{
        id:adminPanel
        width: 180
        listSpacing: 5
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 30
        anchors.leftMargin: 15
    }
    DCMainPage
    {
        width:200
        height: 300
        anchors.right:parent.right
        anchors.left:adminPanel.right
        anchors.leftMargin: 30
    }
}

