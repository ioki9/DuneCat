import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id:column
    anchors.top: parent.top
    signal sorting()
    property alias label: columnText.text
    property int minColWidth: 30

    Text{
        id: columnText
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - upDownIndicator.width
        elide: Text.ElideRight
    }

    Text{
        id:upDownIndicator
        anchors.right:parent.right
        anchors.margins: 4
        anchors.verticalCenter: parent.verticalCenter
        text: "^"
        visible:false
    }

    //resize logic
    MouseArea{
        id:borderMouseArea
        anchors.right: parent.right
        height:parent.height
        width: 5
        hoverEnabled: true
        onMouseXChanged: {
            if(pressed){
                parent.width = (parent.width + mouseX) >= minColWidth ? (parent.width + mouseX) : parent.width
            }
        }

        Rectangle{
            id:columnBorder
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
