pragma Singleton
import QtQuick
import QtQuick.Controls

QtObject {
    property var primaryColor: Material.primaryColor
    property double radius: 20
    property var button: QtObject{
        property double bgOpacity: 0.2
        property color bgColor: "white"
        property color bgColorHovered: bgColor.darker(1.05)
        property color bgColorDown: bgColorHovered.darker(1.06)
        property color bgColorActivated : Qt.rgba(primaryColor.r,primaryColor.g,primaryColor.b,bgOpacity)
        property color bgColorActivatedHovered: bgColorActivated.darker(1.05)
        property color bgColorActivatedDown: bgColorActivatedHovered.darker(1.05)
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
