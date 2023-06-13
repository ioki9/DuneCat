import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle

DCMainWindow
{
    width:1024
    height:480
    id: root
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
        DCMainPage{
        visible: adminPanel.activePage === DCAdminPanel.Home ? true : false
        anchors.fill:parent
        }
        Loader{
            id:pageLoader
            source: adminPanel.activePage === DCAdminPanel.Home ? "" : root.pageUrlList[adminPanel.activePage]
            anchors.fill: parent
        }
    }

}

