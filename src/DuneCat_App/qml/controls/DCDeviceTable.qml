import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
ScrollView {

    width:200
    height:200
    ListView{
        id:listView
        anchors.fill:parent
        clip: true
        highlight: listHighlight
        currentIndex: -1
        highlightFollowsCurrentItem: true


        model: ListModel {
            ListElement{deviceIcon:"drawer"; name:"test123"; menuIcon:"menu"}
            ListElement{deviceIcon:"drawer"; name:"test123"; menuIcon:"menu"}
            ListElement{deviceIcon:"drawer"; name:"test123"; menuIcon:"menu"}
            ListElement{deviceIcon:"drawer"; name:"test123"; menuIcon:"menu"}
        }
        delegate:viewDelegate

    }
    Component {
        id: viewDelegate
        Rectangle {
            id:wrapper
            width: 180
            height: 40
            color:"transparent"
            Row{
                spacing:5
                IconImage
                {
                }
                IconImage{ name: model.deviceIcon; color:wrapper.ListView.isCurrentItem ? "white" : "black"}

                Text {
                    text: model.name
                    color: wrapper.ListView.isCurrentItem ? "white" : "black"
                }
                ToolButton{ icon.name:model.menuIcon }
            }
            MouseArea{
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
