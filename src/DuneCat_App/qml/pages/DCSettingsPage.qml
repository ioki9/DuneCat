import QtQuick
import QtQuick.Controls
import DCBase
Rectangle {

    Switch{
        text: "Enable autostart"
        checked:{checked = DCSettings.autoStart}
        onPositionChanged: DCSettings.autoStart = position
    }
}
