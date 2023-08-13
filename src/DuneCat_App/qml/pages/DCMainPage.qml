import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle
import "qrc:/DuneCat/imports/qml/controls"
Item {
    property list<string> tableUrlList:["qrc:/DuneCat/imports/qml/controls/DCProcessTable.qml",
                                        "qrc:/DuneCat/imports/qml/controls/DCProcessHistoryTable.qml"]
    Material.accent: DCStyle.primaryColor

    Row{
        id:header
        height: 50
        width:parent.width
        anchors.left:parent.left
        anchors.leftMargin: 5
        spacing: 2
        property alias tableSelector: internTableSelector

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
    Image{
        source:"qrc:/DuneCat/imports/icons/20x20@2/search.png"
        width:25
        height:25
        anchors.right:searchTextField.left
        anchors.rightMargin: 5
        anchors.verticalCenter: searchTextField.verticalCenter
    }

    TextField{
        id:searchTextField
        anchors.verticalCenter: header.verticalCenter
        anchors.right: header.right
        anchors.rightMargin: 10
        //anchors.bottom: header.bottom
        //anchors.bottomMargin: 20
        width:200
        height:30
        placeholderText: qsTr("Type text to search")
//        placeholderTextColor: searchTextField.focus ? "black" : "lightgrey"
//        background:Rectangle{
//            radius: 5
//            color: "transparent"
//            border.color: searchTextField.focus ? DCStyle.primaryColor : "grey"
//        }
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
        source: tableUrlList[header.tableSelector.selectedId]
    }

}
