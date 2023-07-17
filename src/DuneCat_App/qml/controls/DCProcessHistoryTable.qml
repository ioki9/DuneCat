import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DCStyle
import "qrc:/DuneCat/imports/qml/components"


ScrollView {
    id:scrollView
    property alias model : tableView.model
    ScrollBar.vertical: ScrollBar{
        id:scrollBar
        parent:scrollView
        anchors.topMargin: header.height
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right:parent.right
        }

    TableView
    {
        id:tableView
        anchors.fill:parent
        topMargin: header.height
        delegate:viewDelegate
        boundsBehavior: Flickable.StopAtBounds
        selectionBehavior: TableView.SelectRows
        selectionModel: ism
        model: ProcessHistoryModel
        columnWidthProvider:function(column) {
            return Math.min(200,model.columnWidth(column,10))
        }
        property bool headerBinded: false
    }

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

    Rectangle
    {
        parent:scrollView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: header.height
        color: "darkgrey"
        height: 1
        width:parent.width
        z:10
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
                    console.log("called sort in qml");
                    tableView.model.sort(index, state === "up" ? Qt.AscendingOrder : Qt.DescendingOrder)
                    console.log("sorted in qml")
                }
            }
        }
    }


    Component {
        id: viewDelegate
        DCSideRoundedRect {
            id:wrapper
            property alias textDelegate: textDel
            implicitHeight: textDel.implicitHeight+15
            recColor: selected ? "lightblue" : "white"
            recRadius: DCStyle.radius
            recRadiusSide:{
                if(column === 0)
                   return DCSideRoundedRect.RectangleSide.Left
                else if(column === (tableView.model.columnCount() - 1))
                    return DCSideRoundedRect.RectangleSide.Right
                else
                    return DCSideRoundedRect.RectangleSide.None
            }
            required property bool selected
            required property bool current
            Text{
                id: textDel
                z: 3
                width:parent.width
                text:display/*{
                    if(column === 23 || column === 24)
                        return displayDate.toLocaleString(Locale.ShortFormat)
                    else
                        return display
                }*/
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
}

