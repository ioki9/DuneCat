import QtQuick
import QtQuick.Controls
Rectangle {
    height: 300
    width: 200
    anchors.centerIn: parent
    SettingsList{
        anchors.fill: parent
        highlight: true
        model:ListModel {
            ListElement { title: "General"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"drawer" }
            ListElement { title: "Connection"; source: "qrc:/DuneCat/imports/qml/pages/SettingsDialog.qml"; icon:"menu" }
            ListElement { title: "Advanced"; source: "qrc:/DuneCat/imports/SettingsDialog.qml"; icon:"menu" }
        }
    }
}
