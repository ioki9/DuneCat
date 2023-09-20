import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id:column
    anchors.top: parent.top
    signal sorting()
    property alias label: columnText.text
    property alias fontPixelSize: columnText.font.pixelSize
    property int minColWidth: 30

    Text{
        id: columnText
        //font.weight: font.Medium
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - upDownIndicator.width
        elide: Text.ElideRight
    }

    Text{
        id:upDownIndicator
        anchors.right:parent.right
        anchors.margins: 5
        anchors.verticalCenter: parent.verticalCenter
        text: "\u2B9F"
        visible:false
    }

    //resize logic
    MouseArea{
        id:borderMouseArea
        anchors.right: parent.right
        height:parent.height
        width: 5
        acceptedButtons: Qt.LeftButton
        hoverEnabled: true
        onMouseXChanged: {
            if(borderMouseArea.pressedButtons & Qt.LeftButton){
                parent.width = (parent.width + mouseX) >= minColWidth ? (parent.width + mouseX) : parent.width
            }
        }

        Rectangle{
            id:columnBorder
            z:2
            anchors.right: parent.right
            height:parent.height
            width:1
            color:"darkgrey"
        }

        HoverHandler{
            acceptedPointerTypes: PointerDevice.AllDevices
            cursorShape: Qt.SizeHorCursor
        }
    }
    //sorting logic
    function stopSorting(){state=""}

    TapHandler{id:tap; onTapped: nextState()}

    function nextState(){
        if(state === "down")
            state = "up"
        else
            state = "down"
        column.sorting()
    }

    states:[
        State{
            name: "up"
            PropertyChanges{target:upDownIndicator; visible: true; rotation: 0}
            PropertyChanges{target:column; color:"lightblue"}
        },
        State{
            name: "down"
            PropertyChanges{target:upDownIndicator; visible: true; rotation: 180}
            PropertyChanges{target:column; color:"lightblue"}
        }
    ]
}
