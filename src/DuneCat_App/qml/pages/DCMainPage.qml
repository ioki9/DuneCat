import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle
import "qrc:/DuneCat/imports/qml/controls"
Rectangle {
    id:root
    property list<string> tableUrlList:["qrc:/DuneCat/imports/qml/controls/DCProcessTable.qml",
                                        "qrc:/DuneCat/imports/qml/controls/DCProcessHistoryTable.qml"]
    property list<var> activeStateProps
    property list<var> historyStateProps

    Material.accent: DCStyle.primaryColor
    color: "white"

    function setStateProps(props){
        header.tableSelector.selectedId = props[0]
        activeStateProps = props[1]
        historyStateProps = props[2]
        if(header.tableSelector.selectedId === 1)
            tableLoader.item.setStateProps(historyStateProps)
        else
            tableLoader.item.setStateProps(activeStateProps)
    }

    function getStateProps(){
        if(header.tableSelector.selectedId === 1)
            historyStateProps = tableLoader.item.getStateProps()
        else
            activeStateProps = tableLoader.item.getStateProps()

        return [header.tableSelector.selectedId,activeStateProps,historyStateProps]
    }

    Connections{
        target:header.tableSelector
        function onSelectedIdChanged(){
            if(header.tableSelector.selectedId === 1)
            {
                activeStateProps = tableLoader.item.getStateProps()
                tableLoader.setSource(tableUrlList[1])
                if(historyStateProps.length !== 0)
                    tableLoader.item.setStateProps(historyStateProps)
            }
            else
            {
                historyStateProps = tableLoader.item.getStateProps()
                tableLoader.setSource(tableUrlList[0])
                if(activeStateProps.length !== 0)
                    tableLoader.item.setStateProps(activeStateProps)
            }
        }
    }

    Row{
        id:header
        height: 50
        width:parent.width
        anchors.left:parent.left
        anchors.leftMargin: 7
        spacing: 2
        property alias tableSelector: internTableSelector
        property bool hidden:false
        DCSegmentedButton{
            id:internTableSelector
            height:30            
            width: 120
            anchors.verticalCenter: header.verticalCenter
            textLeft.text: qsTr("Active")
            textLeft.font.pointSize: 10
            textRight.text: qsTr("History")
            textRight.font.pointSize: 10
        }
    }
    RoundButton{
        id:refreshButton
        icon.name: "refresh"
        icon.color: "white"
        anchors.verticalCenter: header.verticalCenter
        anchors.right:header.right
        anchors.rightMargin: 10
        radius:DCStyle.radius
        Material.background: DCStyle.primaryColor
        onPressed:tableLoader.item.model.refresh()

    }
    DCToggleButton{
        id:filterButton
        width:100
        height:30
        anchors.verticalCenter: header.verticalCenter
        anchors.right:refreshButton.right
        anchors.rightMargin: 40
        radius:DCStyle.radius
        icon.name: "filter_list"
        icon.height: 20
        icon.width: 20
        text: activated ? "Filters \u2B9D" : "Filters \u2B9F"
        font.pointSize: 10
        buttonColor: DCStyle.primaryColor
    }
    Rectangle{
        id:filterPane
        clip:true
        width: 200
        radius:10
        z:2
        border.width: 1
        border.color: "grey"
        property real childrenHeight
        Component.onCompleted: childrenHeight = childrenRect.height
        height: filterButton.activated ? childrenHeight + 30 : 0
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: filterButton.bottom
        anchors.topMargin:4
        Behavior on height{
            NumberAnimation{
                duration:200
            }
        }
        DCCloseButton{
            width:20
            height:20
            z:3
            icon.name: "close"
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: 5
            anchors.topMargin: 5
            onClosed:filterButton.activated = false
        }

        DCSearchTextField
        {
            id:textSearchFilter
            width:150
            height:35
            z:3
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.top: filterPane.top
            anchors.topMargin: 7
            placeholderText: qsTr("Search...")
            property var filterCols:header.tableSelector.selectedId === 0 ? -1 : [0,1,2]
            onTextChanged: tableLoader.item.model.setFilter(text,filterCols)
        }
        DCDatePicker{
            id:dateTime
            z:3
            width:150
            height:35
            radius: DCStyle.radius
            anchors.top: textSearchFilter.bottom
            anchors.topMargin: 10
            border.width: 1
            border.color: "black"
            fontPointSize: 14
        }

        MouseArea{
            anchors.fill:parent
            onClicked: focus = true
        }
    }

    Rectangle{
        width:parent.width
        height:2
        color:"lightgrey"
        anchors.bottom:tableLoader.top
    }

    Loader{
        id:tableLoader
        anchors.top:header.bottom
        anchors.bottom:parent.bottom
        height: parent.height
        width: parent.width
        source: source = tableUrlList[header.tableSelector.selectedId]
    }

}
