import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle
import "qrc:/DuneCat/imports/qml/controls"
Item {
    property list<string> tableUrlList:["qrc:/DuneCat/imports/qml/controls/DCProcessTable.qml",
                                        "qrc:/DuneCat/imports/qml/controls/DCProcessHistoryTable.qml"]
    Row{
        id:header
        height: 50
        width:parent.width
        anchors.top:parent.top
        anchors.left:parent.left
        anchors.leftMargin: 5
        anchors.topMargin: 10
        spacing: 2
        property alias tableSelector: internTableSelector

        DCSwitchSelector{
            id:internTableSelector
            height:30
            width: 120
            textLeft.text: qsTr("Active")
            textLeft.font.pointSize: 10
            textRight.text: qsTr("History")
            textRight.font.pointSize: 10
            backgroundColor:  "white"
            selectionColor: DCStyle.primaryColor
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
        source:header.tableSelector.selectedRight ? tableUrlList[1] : tableUrlList[0]
    }

}
