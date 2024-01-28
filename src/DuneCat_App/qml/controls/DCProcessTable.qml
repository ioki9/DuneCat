import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DCStyle
import "qrc:/DuneCat/qml/components"
import DCTableModels

ScrollView {
    id:scrollView
    anchors.fill: parent
    property alias model : tableView.model
    property alias scrollBarPos: scrollBar.position
    property alias scrollBarHoriz: scrollBarHoriz

    ScrollBar.vertical: ScrollBar{
        id:scrollBar
        parent:scrollView
        anchors.topMargin: header.height
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right:parent.right
        }
    ScrollBar.horizontal: ScrollBar{
        id:scrollBarHoriz
        parent:scrollView
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right:parent.right
    }

    function setStateProps(props)
    {
        scrollBar.position = props[0]
        for(var i = 0; i < tableView.columns; i++)
            tableView.header.repeater.itemAt(i).width = props[1][i]
    }

    function getStateProps()
    {
        var headerWidths = []
        for(var i = 0; i < tableView.columns; i++)
            headerWidths.push(tableView.columnWidth(i))
        return [scrollBar.position,headerWidths]
    }

    function getColWidths()
    {
        var headerWidths = []
        for(var i = 0; i < tableView.columns; i++)
            headerWidths.push(tableView.columnWidth(i))
        return headerWidths
    }

    function setColWidths(widths)
    {
        for(var i = 0; i < tableView.columns; i++)
            tableView.header.repeater.itemAt(i).width = widths[i]
    }

    TableView
    {
        id:tableView
        anchors.top:header.bottom
        delegate:viewDelegate
        boundsBehavior: Flickable.StopAtBounds
        selectionBehavior: TableView.SelectRows
        selectionModel: ism
        model: SortFilterProcessModel{}
        columnWidthProvider:function(column) {
            return header.repeater.itemAt(column).width
        }
        property bool headerBinded: false
        property alias header: header
        Component.onCompleted: {
        }

        Connections{
            target: header
            function onPositioningComplete() {
                tableView.forceLayout()
            }
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
    }

    Row{
        id:header
        parent:scrollView
        height: 30
        width:parent.width
        x:-tableView.contentX
        anchors.top:parent.top
        property alias repeater: colRepeater
        Repeater
        {
            id: colRepeater
            model:tableView.model.columnCount()
            DCTableHeaderColumn{
                id:col
                height:header.height
                anchors.top: header.top
                fontPixelSize: DCStyle.font.pixelSize.body
                width:
                {
                    if(Qt.platform.os === "linux")
                    {
                        if(index === 1 || index === 4)
                            return 0
                    }
                    return Math.min(200,tableView.model.columnWidth(index,10))
                }
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
            property alias textDelegate: textDel
            implicitHeight: textDel.implicitHeight+15
            color: selected ? "lightblue" : "white"
            required property bool selected
            required property bool current
            Text{
                id: textDel
                //z: 1
                width:parent.width
                text:{
                    if(column === 5)
                        return display.toLocaleString(Locale.ShortFormat)
                    return display
                }
                font.pixelSize: DCStyle.font.pixelSize.body
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

