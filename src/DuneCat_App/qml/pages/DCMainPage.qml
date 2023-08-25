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
        activeStateProps = props[1]
        historyStateProps = props[2]
        header.tableSelector.selectedId = props[0]
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
            console.log(header.tableSelector.selectedId)
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
        DCSwitchSelector{
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
        anchors.verticalCenter: header.verticalCenter
        anchors.right:header.right
        anchors.rightMargin: 10
        onPressed:tableLoader.item.model.refresh()
    }

    DCSearchTextField
    {
        width:200
        height:30
        anchors.verticalCenter: header.verticalCenter
        anchors.right: refreshButton.left
        anchors.rightMargin: 10
        placeholderText: qsTr("Search...")
        property var filterCols:header.tableSelector.selectedId === 0 ? -1 : [0,1,2]
        onTextChanged: tableLoader.item.model.setFilter(text,filterCols)
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
