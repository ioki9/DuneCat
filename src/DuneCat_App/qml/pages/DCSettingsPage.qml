import QtQuick
import QtQuick.Controls
import DCBase
Rectangle {

    Switch{
        id:autoStartSwitch
        text: "Enable autostart"
        checked:checked = Settings.autostart
        onPositionChanged: Settings.autostart = position
    }
}
