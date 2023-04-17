import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DCStyle
import ListModels
ScrollView {
    property double scrollerWidth:ScrollBar.vertical.width
    ListView {
        id:listView
        anchors.fill:parent
        clip: true
        highlight: listHighlight
        currentIndex: -1
        highlightFollowsCurrentItem: false
        model: processModel
        delegate:viewDelegate
        ProcessListModel{id:processModel}
        header: myheader

    }

    Component{
        id: listHeader
        Text{
            text:model.header
        }
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

                Text {
                    text: model.name
                    font.pointSize: 12
                    color: wrapper.ListView.isCurrentItem ? "white" : "black"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    Layout.leftMargin: 10
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }

                Text {
                    text: model.PID
                    font.pointSize: 12
                    color: wrapper.ListView.isCurrentItem ? "white" : "black"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    Layout.leftMargin: 10
                    Layout.alignment: Qt.AlignRigh | Qt.AlignVCenter
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

           width: listView.width - scrollerWidth; height: 40
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
