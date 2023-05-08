import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml.Models
import DCStyle
import TableModels


ScrollView {
    id:scrollView
    property double scrollerWidth:ScrollBar.vertical.width

    TableView {
        id:tableView
        anchors.fill:parent
        topMargin: header.height
        model: SortFilterProcessModel{ }
        delegate:viewDelegate
        boundsBehavior: Flickable.StopAtBounds
        selectionBehavior: TableView.SelectRows
        selectionModel: ism
        columnWidthProvider:function(column) {
            return Math.min(200,model.columnWidth(column,10))
        }

        property bool headerBinded: false
        Connections{
            target: header
            function onPositioningComplete() {
                if(!tableView.headerBinded)
                {
                    tableView.columnWidthProvider = function(column){return header.repeater.itemAt(column).width}
                    tableView.forceLayout()
                    tableView.headerBinded = true
                }
                else
                    tableView.forceLayout()
            }
        }
    }

    Row{
        id:header
        parent:scrollView
        height: 30
        property alias repeater: colRepeater
        Repeater
        {
            id: colRepeater
            model:tableView.model.columnCount()
            DCTableHeaderColumn{
                id:col
                height:parent.height
                fontPointSize: 10
                width:Math.min(200,tableView.model.columnWidth(index,10))
                label:tableView.model.headerData(index,Qt.Horizontal,Qt.DisplayRole)
                onSorting: {
                    for(var i = 0; i < colRepeater.model; i++)
                        if(i !== index)
                            colRepeater.itemAt(i).stopSorting()

                    tableView.model.sort(index, state === "up" ? Qt.AscendingOrder : Qt.DescendingOrder)
                }
            }
        }
    }

    Component {
        id: viewDelegate
        Rectangle {
            id:wrapper
            implicitHeight: textDel.implicitHeight+5
            color: selected ? "lightblue" : "white"
            //onSelectedChanged: {tableView.currentRow;wrapper.color =selected ? "blue" : "white"}
            required property bool selected
            required property bool current
            Text{
                id: textDel
                width:parent.width
                text:model.display
                font.pointSize: 10
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                elide: Text.ElideRight
            }
            MouseArea
            {
                anchors.fill: parent
                onClicked: {
                    if(!ism.isRowSelected(index))
                    {
                        ism.select(tableView.model.index(row,column),
                                   ItemSelectionModel.SelectCurrent | ItemSelectionModel.Rows | ItemSelectionModel.Clear)
                    }
                }
            }
        }
    }
    ItemSelectionModel{
        id:ism
        model:tableView.model
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
