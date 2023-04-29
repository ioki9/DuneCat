import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DCStyle
import ListModels


ScrollView {
    property double scrollerWidth:ScrollBar.vertical.width
    TableView {
        id:tableView
        anchors.fill:parent
        model: ProcessListModel{ }
        delegate:viewDelegate
        boundsBehavior: Flickable.StopAtBounds
        selectionBehavior: TableView.SelectRows
        selectionModel: ItemSelectionModel{}
        columnWidthProvider: function(column) {return Math.min(200,model.columnWidth(column))}
    }

    HorizontalHeaderView{
        id: horizontalHeader
        syncView: tableView
        z:2
        x:-tableView.contentX
        boundsBehavior: Flickable.StopAtBounds
        model: tableView.model
    }


    Component {
        id: viewDelegate
        Rectangle {
            id:wrapper
            implicitHeight: textDel.implicitHeight
            color: selected ? "blue" : "lightgray"

            required property bool selected
            Text{
                id: textDel
                width:parent.width
                text:model.display
                anchors.left: parent.left
                anchors.horizontalCenter: parent.horizontalCenter
                elide: Text.ElideRight | Text.ElideLeft
            }
        }
    }
    Component{
       id: listHighlight
       Rectangle{

           width: tableView.width - scrollerWidth; height: 40
           color: Material.primaryColor; radius: 15; opacity:0.7
           y: tableView.currentItem.y
           Behavior on y {
               SpringAnimation {
                   spring: 3
                   damping: 0.2
               }
           }
       }
    }
}
