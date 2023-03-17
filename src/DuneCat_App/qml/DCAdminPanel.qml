import QtQuick
import QtQuick.Controls.Material
Rectangle {
    property alias listSpacing : listView.spacing
    ListView
    {
        id: listView
        clip:true
        focus: true
        currentIndex: 0
        interactive: false
        anchors.fill: parent

        delegate: ItemDelegate {
            id: itemDelegate
            width: listView.width

            text: talk
            icon.name: model.icon
            icon.color: Material.accentColor

            Text{id:talk;
                text:model.title;
                color: itemDelegate.highlighted ? "white": "black";
                anchors.left:parent.left
                anchors.leftMargin: parent.icon.width + 30
                anchors.verticalCenter: parent.verticalCenter
            }
            background:Rectangle{
                anchors.fill: itemDelegate
                color: itemDelegate.highlighted ? Material.primary : "transparent"
                radius: 15
                opacity: 0.8
            }

            highlighted: hovered || listView.currentIndex === index
            onClicked: {
                DCStyle.primary = "blue"
                listView.currentIndex = index
                myLoader.source = model.source
                myLoader.item.open()
            }
        }
        model:ListModel {
            ListElement { title: "Profile"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"drawer" }
            ListElement { title: "Settings"; source: "qrc:/DuneCat/imports/qml/pages/SettingsDialog.qml"; icon:"menu" }
            ListElement { title: "Devices"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"menu" }
            ListElement { title: "About"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"menu" }

        }
        Loader{
            id: myLoader
        }
    }
}
