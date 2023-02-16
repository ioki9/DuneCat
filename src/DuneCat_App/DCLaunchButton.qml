import QtQuick 2.9
import QtQuick.Controls 2.5

Image {
    id: root
    source: Qt.resolvedUrl("img/LaunchButton.png")
    signal clicked
    MouseArea{
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
