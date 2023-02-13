import QtQuick
import QtQuick.Controls 2.5
import UI
Window {
    color: "white"
    id: root
    width: 640
    height: 480
    visible: true
        DCSettings
        {
            id: ebanyJS
            autoStart: true
        }
    title: qsTr("DuneCat")
        CheckBox{
            id: autoStartCheckBox
            text: "autostart"
            anchors.centerIn: parent   
        }
        Button {
            text: "Ok"
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            onClicked: ebanyJS.autoStart = autoStartCheckBox.checked
        }
        Button {
            text: "apply"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            onClicked: ebanyJS.autoStart = autoStartCheckBox.checked
        }
}
