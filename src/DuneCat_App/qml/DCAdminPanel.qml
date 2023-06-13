import QtQuick
import QtQuick.Controls.Material

Rectangle {
    enum DCMenuButton{
        Home,
        Settings,
        Devices,
        About
    }
    id: root
    color:Material.primaryColor
    property alias listSpacing : listView.spacing
    property alias activePage: listView.activePage
    ListView
    {

        id: listView
        clip:true
        focus: true
        currentIndex: 0
        interactive: false
        anchors.topMargin: 70
        anchors.fill: parent
        property int activePage:DCAdminPanel.DCMenuButton.Home

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
                listView.activePage = model.enumeration
            }
        }
        model:ListModel {
            ListElement { title: "Home"; enumeration: DCAdminPanel.DCMenuButton.Home; icon:"drawer" }
            ListElement { title: "Settings"; enumeration: DCAdminPanel.DCMenuButton.Settings; icon:"menu" }
            ListElement { title: "Devices"; enumeration: DCAdminPanel.DCMenuButton.Devices; icon:"menu" }
            ListElement { title: "About"; enumeration: DCAdminPanel.DCMenuButton.About; icon:"menu" }

        }
    }
}
