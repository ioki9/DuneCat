import QtQuick
import QtQuick.Controls 2.5
import DCBase

Window {
    color: "white"
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("DuneCat")

    DCLaunchButton
    {
        id: launchButton
        width:300
        height:300
        anchors.centerIn: parent
        property bool isOn:false
        onClicked:{
            isOn = isOn === false ? true: false
            DCSettings.autoStart = isOn}
        }
}
