import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle
import "qrc:/DuneCat/imports/qml/controls"
Rectangle {
    radius:DCStyle.radius
    DCProcessTable{
        anchors.fill:parent
    }

}
