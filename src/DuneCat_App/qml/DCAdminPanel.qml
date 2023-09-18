import QtQuick
import QtQuick.Controls.Material
import DCStyle
// TO DO: save header width state
Rectangle {
    enum DCMenuButton{
        Home,
        Settings,
        Devices,
        About
    }
    id: root
    color:styleColor
    property color styleColor: DCStyle.primaryColor
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
        highlight: highlight

        delegate: ItemDelegate {
            id: itemDelegate
            width: listView.width
            anchors.left: parent.left
            anchors.leftMargin: 3
            anchors.right: parent.right
            anchors.rightMargin: 3

            icon.name: model.icon
            icon.color: itemDelegate.highlighted ? root.styleColor : "white";

            Text{
                id:buttonText
                text:model.title
                font.pointSize: 12
                color: itemDelegate.highlighted ? root.styleColor : "white";
                anchors.left:parent.left
                anchors.leftMargin: parent.icon.width + 30
                anchors.verticalCenter: parent.verticalCenter
            }
            background:Rectangle{
                id:bgDelegate
                anchors.fill: itemDelegate
                color: itemDelegate.hovered ? Qt.rgba(255,255,255,0.2) : "transparent"
                radius: DCStyle.radius
            }
            highlighted: listView.currentIndex === index
            onClicked: {
                listView.currentIndex = index
                listView.activePage = model.enumeration
            }
        }
        Component {
            id: highlight
            Rectangle {
                anchors.left: parent.left
                anchors.leftMargin: 3
                anchors.right: parent.right
                anchors.rightMargin: 3
                color: "white"; radius: DCStyle.radius
            }
        }
        model:ListModel {
            ListElement { title: "Tables"; enumeration: DCAdminPanel.DCMenuButton.Home; icon:"table_view" }
            ListElement { title: "Settings"; enumeration: DCAdminPanel.DCMenuButton.Settings; icon:"settings" }
            ListElement { title: "Devices"; enumeration: DCAdminPanel.DCMenuButton.Devices; icon:"menu" }
            ListElement { title: "About"; enumeration: DCAdminPanel.DCMenuButton.About; icon:"menu" }

        }
    }
}
