import QtQuick
import QtQuick.Layouts
Item {
    id:root
    height: 30
    width:parent.width
    z:2
    required property var titles
    property var columnWidths
    Row{
        Repeater{
        id:repeater
        model: root.titles
            Rectangle{
                property string name:modelData
                height: root.height
                width: columnWidths ? columnWidths[index] : (label.paintedWidth+5)
                color:"Grey"
                Text{
                    id:label
                    horizontalAlignment: Text.AlignHCenter
                    anchors.centerIn: parent
                    text: name
                }
            }
        }
    }


}
