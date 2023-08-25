import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle

DCMainWindow
{
    id: root
    width: width = Settings.window_width
    height: height = Settings.window_height
    property list<string> pageUrlList:[]
    property list<var> mainPageStateProps:[]
    Component.onCompleted:{
        startupFunction()
    }
    Connections {
        target: SystemTray
        function onShow() {
            root.requestActivate()
            root.show()
            root.raise()
            if(!page.loader.active)
            {
                adminPanelLoader.active = true
                pageLoader.active = true
            }
        }
        function onQuit() {
            Settings.window_height = root.height
            Settings.window_width = root.width
            Settings.window_maximized = root.visibility === Window.Maximized
            Qt.quit()
        }
    }
    Connections {
        target: root
        function onClosing(close) {
            if(root.visibility !== Window.Hidden)
            {
                close.accepted = false
                root.visibility = Window.Hidden
                pageLoader.active = false
                adminPanelLoader.active = false
            }
        }
    }
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

    Connections{
        target: adminPanel.adminLoaderItem
        function onActivePageChanged(){
            if(adminPanel.adminLoaderItem.activePage !== DCAdminPanel.Home)
            {
                mainPageStateProps = page.loaderItem.getStateProps()
                page.loader.setSource(root.pageUrlList[adminPanel.adminLoaderItem.activePage])
            }
            else
            {
                page.loader.setSource(root.pageUrlList[adminPanel.adminLoaderItem.activePage])
                page.loaderItem.setStateProps(mainPageStateProps)
            }
        }
    }

    Item{
        id:adminPanel
        property alias adminLoaderItem: adminPanelLoader.item
        width: 180
        z:5
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top

        Loader{
            id: adminPanelLoader
            active:true
            anchors.fill: parent
            sourceComponent: adminPanelComp
        }
        Component{
            id:adminPanelComp
            DCAdminPanel{

                listSpacing: 5
                anchors.fill: parent
            }
        }
    }

    Rectangle{
        id:page
        property alias loader: pageLoader
        property alias loaderItem: pageLoader.item
        anchors.left: adminPanel.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: parent.right
        Loader{
            id:pageLoader
            source: root.pageUrlList[DCAdminPanel.Home]
            anchors.fill: parent
        }
    }


}

