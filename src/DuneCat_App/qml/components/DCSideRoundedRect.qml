import QtQuick
import QtQuick.Controls
 import QtQuick.Shapes
Item {
    enum RoundedSide{
        Left,
        Right
    }
    id:root
    clip:true
    property real recOpacity:1.0
    required property int recRadius
    required property int recRoundSide
    property color borderColor
    property int borderWidth:1
    property color color: "white"
    readonly property bool isLeftRounded: recRoundSide === DCSideRoundedRect.RoundedSide.Left

    Rectangle{
        x:isLeftRounded ? 0 : -recRadius
        width:parent.width + recRadius
        height: parent.height
        radius: recRadius
        color:Qt.rgba(root.color.r,root.color.g,root.color.b,recOpacity)
        border.color: root.borderColor
        border.width: root.borderWidth
    }
    Rectangle{
        x:isLeftRounded ? root.width-1 : 0
        width:1
        height:root.height
        color:root.borderColor
    }

}
