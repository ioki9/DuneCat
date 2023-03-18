import QtQuick
import QtQuick.Controls

ToolButton {
    id: root
    icon.name: "LaunchButton"
    icon.source: "qrc:/DuneCat/imports/img/LaunchButton.png"
    icon.height: 300
    icon.width: 300
    signal clicked
    MouseArea{
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
