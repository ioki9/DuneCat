import QtQuick
import DCStyle
import QtQuick.Controls
Switch {
    id:cSwitch
    property color textDefaultColor: "white"
    property alias textLeft : internIndecator.internTextLeft
    property alias textRight: internIndecator.internTextRight
    property alias backgroundColor: internIndecator.color
    property alias selectionColor: internIndecator.internSelectionRectColor
    readonly property  bool selectedRight: cSwitch.checked

    indicator: Rectangle{
        id:internIndecator
        radius: DCStyle.radius
        height: cSwitch.height
        width: cSwitch.width
        border.color: "grey"
        border.width: 1
        property alias internTextLeft : internLeftText
        property alias internTextRight: internRightText
        property alias internSelectionRectColor: selectionRect.color
        Text{
            id:internLeftText
            z:32
            x: ((parent.width/2) - contentWidth)/2
            y: (parent.height-contentHeight)/2
            color:cSwitch.checked ? "grey" : selectionRect.color
            font.bold: cSwitch.checked ? false : true

        }
        Text{
            id:internRightText
            z:2
            x: (3*parent.width-2*contentWidth)/4 //parent.width - contentWidth - ((parent.width/2) - contentWidth)/2
            y: (parent.height-contentHeight)/2
            color:cSwitch.checked ? selectionRect.color : "grey"
            font.bold: cSwitch.checked

        }
        Rectangle{
            id: selectionRect
            x: cSwitch.checked ? parent.width - width : 0
            width: parent.width/2
            height: parent.height
            radius: DCStyle.radius
            color: DCStyle.primaryColor
            opacity: 0.4
            Behavior on x {
                enabled: !cSwitch.down
                SmoothedAnimation { velocity: 200 }
            }
        }
        Rectangle{
            id: border
            x: cSwitch.checked ? parent.width - width : 0
            z:2
            width: parent.width/2
            height: parent.height
            radius: DCStyle.radius
            color: "transparent"
            border.color: DCStyle.primaryColor
            Behavior on x {
                enabled: !cSwitch.down
                SmoothedAnimation { velocity: 200 }
            }
        }
    }
}
