import QtQuick 2.9
import QtQuick.Controls 2.5

ToolButton {
    id: root
    icon.name: "LaunchButton"
    icon.source: "qrc:/DuneCat/qml/img/LaunchButton.png"
    icon.height: 300
    icon.width: 300
    signal clicked
    MouseArea{
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
