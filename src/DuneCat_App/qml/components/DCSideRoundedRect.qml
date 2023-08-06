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
        color: recColor
        Component.onCompleted: {
            if(recRoundSide === DCSideRoundedRect.RectangleSide.Left)
            {
                radius = recRadius
                width = parent.width
            }
            else
            {
                radius = 0
                width = recRadius
            }
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
            {
                radius = recRadius
                width = parent.width
            }
            else
            {
                radius = 0
                width = recRadius
            }

        }

        anchors.right : parent.right
    }
}
