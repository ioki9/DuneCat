import QtQuick
import QtQuick.Controls
import DCBase
Rectangle {

    Switch{
        text: "Enable autostart"
        checked:{checked = Settings.autoStart}
        onPositionChanged: Settings.autoStart = position
    }
}
