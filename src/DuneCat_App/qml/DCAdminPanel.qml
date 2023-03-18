import QtQuick
import QtQuick.Controls.Material
Rectangle {

    color:Material.primaryColor
    property alias listSpacing : listView.spacing
    property alias activePageUrl: listView.activePage
    ListView
    {
        id: listView
        clip:true
        focus: true
        currentIndex: 0
        interactive: false
        anchors.topMargin: 70
        anchors.fill: parent

        property string activePage:"qrc:/DuneCat/imports/qml/pages/DCMainPage.qml"

        delegate: ItemDelegate {
            id: itemDelegate
            width: listView.width
            icon.name: model.icon
            icon.color: itemDelegate.highlighted ? Material.primary : "white";

            Text{
                id:buttonText
                text:model.title
                font.pointSize: 12
                color: itemDelegate.highlighted ? Material.primary : "white";
                anchors.left:parent.left
                anchors.leftMargin: parent.icon.width + 30
                anchors.verticalCenter: parent.verticalCenter
            }
            background:Rectangle{
                anchors.fill: itemDelegate
                anchors.leftMargin: 10
                color: itemDelegate.highlighted ? "white" : "transparent"
                radius: DCStyle.radius
            }

            highlighted: hovered || listView.currentIndex === index
            onClicked: {
                listView.currentIndex = index
                listView.activePage = model.source
            }
        }
        model:ListModel {
            ListElement { title: "Home"; source: "qrc:/DuneCat/imports/qml/pages/DCMainPage.qml"; icon:"drawer" }
            ListElement { title: "Settings"; source: "qrc:/DuneCat/imports/qml/pages/DCSettingsPage.qml"; icon:"menu" }
            ListElement { title: "Devices"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"menu" }
            ListElement { title: "About"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"menu" }

        }
    }
}
