import QtQuick
import QtQuick.Controls
import DCBase
ApplicationWindow {
    id: root
    color: "white"
    visible: true
    title: qsTr("DuneCat")
    Connections {
        target: SystemTray
        function onShow() {
            root.requestActivate()
            root.show()
            root.raise()
        }
        function onQuit() {
            Qt.quit()
        }
    }
}
