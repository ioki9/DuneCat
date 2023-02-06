import QtQuick
import QtQuick.Controls 2.5
Window {
    color: "white"
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
        Button {
            text: "Ok"
            onClicked: model.submit()
        }
        Button {
            text: "Cancel"
            onClicked: model.revert()
        }
}
