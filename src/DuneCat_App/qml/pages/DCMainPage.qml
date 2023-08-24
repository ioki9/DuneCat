import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle
import "qrc:/DuneCat/imports/qml/controls"
Rectangle {
    id:root
    property list<string> tableUrlList:["qrc:/DuneCat/imports/qml/controls/DCProcessTable.qml",
                                        "qrc:/DuneCat/imports/qml/controls/DCProcessHistoryTable.qml"]
    property list<real> tableScrollPosList:[0.0,0.0]
    Material.accent: DCStyle.primaryColor
    color: "white"
    function saveScrollPos(){
        if(header.tableSelector.selectedId === 0)
            tableScrollPosList[0] = tableLoader.item.scrollBarPos
        else
            tableScrollPosList[1] = tableLoader.item.scrollBarPos

    }

    Connections{
        target:header.tableSelector
        function onSelectedIdChanged(){
            if(header.tableSelector.selectedId === 1)
            {
                tableScrollPosList[0] = tableLoader.item.scrollBarPos
                tableLoader.setSource(tableUrlList[1])
            }
            else
            {
                tableScrollPosList[1] = tableLoader.item.scrollBarPos
                tableLoader.setSource(tableUrlList[0])
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
        onLoaded: item.scrollBarPos = tableScrollPosList[header.tableSelector.selectedId]
        source: tableUrlList[0]
    }

}
