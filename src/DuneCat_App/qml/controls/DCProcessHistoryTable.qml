import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DCStyle
import "qrc:/DuneCat/imports/qml/components"


ScrollView {
    id:scrollView
    property alias model : tableView.model
    property alias scrollBarPos: scrollBar.position
    ScrollBar.vertical: ScrollBar{
        id:scrollBar
        parent:scrollView
        anchors.topMargin: header.height
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right:parent.right
        }

    function setStateProps(props)
    {
        scrollBar.position = props[0]
        for(var i = 0; i < tableView.columns; i++)
            tableView.header.repeater.itemAt(i).width =props[1][i]
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
        property int lastSelected
        property alias header: header
        anchors.top:header.bottom
        delegate:viewDelegate
        boundsBehavior: Flickable.StopAtBounds
        selectionBehavior: TableView.SelectRows
        selectionModel: ism
        model: ProcessHistoryModel
        columnWidthProvider: function(column) {return header.repeater.itemAt(column).width}
        property bool headerBinded: false
        Connections{
            target: model
            function onModelAboutToBeReset(){
                var temp = ism.selectedRows(0)[0]
                tableView.lastSelected = temp.row
                console.log("on model about to be reset. Index = ",tableView.lastSelected)

            }
            function onModelReset(){
                console.log("on model reset. Index = ", tableView.lastSelected)

                if(tableView.lastSelected)
                    ism.select(tableView.model.index(tableView.lastSelected,0),
                                ItemSelectionModel.SelectCurrent )
            }
        }

    }

    Connections{
        target: header
        function onPositioningComplete() {
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
                width:{
                    var role
                    if(index === 2 || index === 3)
                        role = Qt.UserRole
                    else
                        role = Qt.DisplayRole

                    return Math.min(200,tableView.model.columnWidth(index,role,10))
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
            required property int row
            required property bool current
            Text{
                id: textDel
                z: 3
                width:parent.width
                text:{
                    if(column === 3 || column === 4)
                        return displayDate
                    else
                        return display
                }
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

//            Loader{
//                width: parent.width
//                height: parent.height
//                sourceComponent: {
//                    if(column === 0)
//                        return roundedLeftRec
//                    else if(column === tableView.model.columnCount() -1)
//                        return roundedRightRec
//                    return midRec
//                }
//                active: selected
//            }

        }
    }
    Component
    {
        id: roundedLeftRec
        DCSideRoundedRect{
            z:2
            color: "lightblue"
            opacity: 0.8
            recRadius: DCStyle.radius
            recRoundSide:DCSideRoundedRect.RoundedSide.Left
        }
    }
    Component
    {
        id: roundedRightRec
        DCSideRoundedRect{
            z:2
            color: "lightblue"
            opacity: 0.8
            recRadius: DCStyle.radius
            recRoundSide:DCSideRoundedRect.RoundedSide.Right
        }
    }
    Component
    {
        id: midRec
        Rectangle{
            z:2
            color: "lightblue"
            opacity: 0.8
        }
    }

    ItemSelectionModel{
        id:ism
        model:tableView.model
    }

}

