import QtQuick
import QtQuick.Controls
import DCBase

DCMainWindow
{
    width:640
    height:480
    id: root
    onDrawerButtonClicked: drawer.open()
    Drawer {
        id: drawer
        width: Math.min(parent.width, parent.height) / 3
        height: parent.height
        ListView
        {
            id: listView
            clip:true
            focus: true
            currentIndex: -1
            anchors.fill: parent
            delegate: ItemDelegate {
                width: listView.width
                text: model.title
                icon.name: model.icon
                highlighted: hovered
                onClicked: {
                    listView.currentIndex = index
                    if(model.title === "Settings")
                        settingsDialog.open()
                    drawer.close()
                }
            }
            model:ListModel {
                ListElement { title: "Settings"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"menu" }
                ListElement { title: "Profile"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"drawer" }
                ListElement { title: "About"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"menu" }

            }
            Component : SettingsDialog{
                id: settingsDialog
                width:300
                height:300
            }
        }
    }
}

