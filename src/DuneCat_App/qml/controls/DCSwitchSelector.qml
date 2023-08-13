import QtQuick
import DCStyle
import QtQuick.Controls
import "qrc:/DuneCat/imports/qml/components"
Item {
    id:root
    property alias textLeft : internLeftButton.internTextLeft
    property color switchColor : DCStyle.primaryColor
    property alias textRight: internRightButton.internTextRight
//    property alias backgroundColor: internIndecator.color
//    property alias selectionColor: internIndecator.internSelectionRectColor
    property int selectedId: 0
    signal buttonClicked(emitter: int)

    DCSideRoundedRect{
        id:internLeftButton
        x:0
        height: root.height
        width: root.width/2
        color: checked ? switchColor : "white"
        borderColor:checked ? switchColor : "grey"
        borderWidth: 1
        recOpacity:0.2
        recRadius: DCStyle.radius
        recRoundSide:DCSideRoundedRect.RoundedSide.Left
        property bool checked: true
        property alias internTextLeft : internLeftText
        Connections {
             target: root
             function onButtonClicked(emitter: int) {
                 if(emitter !== 0)
                     internLeftButton.checked = false;
             }
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(parent.checked === true)
                    return;
                parent.checked = true;
                buttonClicked(0);
                root.selectedId = 0}
        }
    }
    Text{
        id:internLeftText
        z:3
        x: (internLeftButton.width - contentWidth)/2
        y: (internLeftButton.height-contentHeight)/2
        color:internLeftButton.checked ? switchColor : "grey"
        font.bold: internLeftButton.checked ? true : false
    }

    DCSideRoundedRect{
        id:internRightButton
        recRadius: DCStyle.radius
        x: root.width/2 -1
        height: root.height
        width: root.width/2
        recOpacity:0.2
        color: checked ? switchColor : "white"
        borderColor:checked ? switchColor : "grey"
        borderWidth: 1
        recRoundSide:DCSideRoundedRect.RoundedSide.Right
        property bool checked: false
        property alias internTextRight: internRightText
        Connections {
             target: root
             function onButtonClicked(emitter: int) {
                 if(emitter !== 1)
                     internRightButton.checked = false;
             }
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(parent.checked === true)
                    return;
                parent.checked = true;
                buttonClicked(1);
                root.selectedId = 1}
        }
    }
    Text{
        id:internRightText
        z:2
        y: (internRightButton.height - contentHeight)/2
        anchors.left: internRightButton.left
        anchors.leftMargin: (internRightButton.width - contentWidth)/2
        opacity:1.0
        color:internRightButton.checked ? switchColor : "grey"
        font.bold: internRightButton.checked ? true : false

    }

    Rectangle{
        id: border
        z:2
        width: 1
        height: parent.height
        color: switchColor
        anchors.centerIn: parent
    }

}
