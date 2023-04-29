import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle

DCMainWindow
{
    width:1024
    height:480
    id: root

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
            source: adminPanel.activePageUrl
            anchors.fill: parent
        }
    }

}

