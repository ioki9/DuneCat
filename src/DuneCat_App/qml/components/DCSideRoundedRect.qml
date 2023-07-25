import QtQuick
import QtQuick.Controls

Item {
    enum RectangleSide{
        Left,
        Right
    }
    layer.enabled: true
    opacity:1.0
    required property int recRadius
    required property int recRoundSide
    property color recColor: "white"


    Rectangle {
        id: leftRect
        radius: {
}
        color: recColor
        width:{
            }
        Component.onCompleted: {
            if(recRoundSide === DCSideRoundedRect.RectangleSide.Left)
                width = parent.width
            else
                width = recRadius
            if(recRoundSide === DCSideRoundedRect.RectangleSide.Left)
                radius = recRadius
            else
                radius = 0

        }
        height:parent.height
        anchors.left: parent.left
    }

    Rectangle {
        id: rightRect
        color: recColor
        height: parent.height
        Component.onCompleted: {
            if(recRoundSide === DCSideRoundedRect.RectangleSide.Right)
                width = parent.width
            else
                width = recRadius
            if(recRoundSide === DCSideRoundedRect.RectangleSide.Right)
                radius = recRadius
            else
                radius = 0
        }

        anchors.right : parent.right
    }
}
