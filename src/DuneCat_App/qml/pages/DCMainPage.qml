import QtQuick
import QtQuick.Controls
import DCBase
import DCStyle
import "qrc:/DuneCat/imports/qml/controls"
Rectangle {

    radius:DCStyle.radius
    //property alias procTableScrollPos: procTable.scrollBarPos
    DCProcessHistoryTable{
        id:procTable
        anchors.fill:parent
    }

}
