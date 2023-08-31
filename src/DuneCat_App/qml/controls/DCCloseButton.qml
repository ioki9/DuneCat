import QtQuick
import QtQuick.Controls
import DCStyle
Rectangle {
    id:root
    signal closed()
    property alias icon: icon
    property color backgroundColor: DCStyle.primaryColor
    property color backgroundDefaultColor: backgroundDefaultColor = root.color
    property color iconHoveredColor: "white"
    property color iconDefaultColor: iconDefaultColor = icon.color
    color: "transparent"
    radius: DCStyle.radius
    MouseArea{
        anchors.fill:parent
        hoverEnabled: true
        onEntered: {
            icon.color = iconHoveredColor
            parent.color = backgroundColor
        }
        onExited: {
            icon.color = iconDefaultColor
            parent.color = backgroundDefaultColor
        }
        onPressed:parent.color = parent.color.darker(DCStyle.button.darkerCoefficient)
        onClicked: {closed();parent.color = backgroundColor}
    }
    IconImage{
        id:icon
        color: iconDefaultColor
        width:parent.width
        height: parent.height
    }
}
