pragma Singleton
import QtQuick
import QtQuick.Controls

QtObject {
    property var primaryColor: Material.primaryColor
    property real radius: 20
    property var button: QtObject{
        property real bgOpacity: 0.2
        property real darkerCoefficient: 1.05
        property color bgColor: "white"
        property color bgColorHovered: bgColor.darker(darkerCoefficient)
        property color bgColorDown: bgColorHovered.darker(darkerCoefficient)
        property color bgColorActivated : Qt.rgba(primaryColor.r,primaryColor.g,primaryColor.b,bgOpacity)
        property color bgColorActivatedHovered: bgColorActivated.darker(darkerCoefficient)
        property color bgColorActivatedDown: bgColorActivatedHovered.darker(darkerCoefficient)
        property color borderColor: "grey"
        property color borderColorHovered: borderColor.darker(1.5)
        property color borderColorDown: borderColorHovered
        property color borderColorActivated: primaryColor
        property color borderColorActivatedHovered: primaryColor.darker(1.5)
        property color borderColorActivatedDown: borderColorActivatedHovered
        property color textColor: "grey"
        property color textColorHovered: textColor.darker(1.5)
        property color textColorDown: textColorHovered
        property color textColorActivated:primaryColor

    }
}
