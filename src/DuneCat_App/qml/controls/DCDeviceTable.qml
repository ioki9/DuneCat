import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DCBase
import DCStyle 1.0

ScrollView {
    id:root
    signal tableCompleted
    property double scrollerWidth:ScrollBar.vertical.width

    ListView {

        id:listView
        anchors.fill:parent
        clip: true
        focus: true
        highlight: listHighlight
        currentIndex: -1
        highlightFollowsCurrentItem: true

        model: ListModel {
            ListElement{indicatorIcon: "circle_indicator"; deviceIcon:"desktop"; name:"test1"; menuIcon:"menu"}
            ListElement{indicatorIcon: "circle_indicator"; deviceIcon:"desktop"; name:"test2"; menuIcon:"menu"}
            ListElement{indicatorIcon: "circle_indicator"; deviceIcon:"desktop"; name:"test3"; menuIcon:"menu"}
            ListElement{indicatorIcon: "circle_indicator"; deviceIcon:"desktop"; name:"test4явлгбтмлявбтм"; menuIcon:"menu"}
        }
        delegate:viewDelegate

    }

    Component {

        id: viewDelegate
        Rectangle {
            id:wrapper
            width: listView.width - scrollerWidth
            height: 40
            color:"transparent"
            RowLayout {
                id:rowLayout
                anchors.fill: parent
                spacing:5
                IconImage {
                    id:devIcon
                    name: model.deviceIcon
                    color:wrapper.ListView.isCurrentItem ? "white" : "black"
                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.leftMargin: 7
                }

                Text {
                    text: model.name
                    font.pixelSize: DCStyle.font.pixelSize.body
                    color: wrapper.ListView.isCurrentItem ? "white" : "black"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    Layout.leftMargin: 10
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }

                ToolButton {
                    id:menuButton
                    icon.name:model.menuIcon
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.rightMargin: 8
                }
            }
            MouseArea {
                anchors.fill:parent
                onClicked:{listView.currentIndex = model.index}
            }
        }
    }
    Component{
       id: listHighlight
       Rectangle{
           color: Material.primaryColor; radius: 15; opacity:0.7
           y: listView.currentItem.y
           Behavior on y {
               SpringAnimation {
                   spring: 3
                   damping: 0.2
               }
           }
       }
    }
}
