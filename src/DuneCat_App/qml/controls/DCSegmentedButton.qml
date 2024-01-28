import QtQuick
import DCStyle
import QtQuick.Controls
import "qrc:/DuneCat/qml/components"

Item {
    id:root
    property alias textLeft : internLeftButton.internTextLeft
    property color buttonColor : DCStyle.primaryColor
    property alias textRight: internRightButton.internTextRight
    QtObject{
        id:props
        property color buttonColorActive: Qt.rgba(buttonColor.r,buttonColor.g,buttonColor.b,DCStyle.button.bgOpacity)
    }

//    property alias backgroundColor: internIndecator.color
//    property alias selectionColor: internIndecator.internSelectionRectColor
    property int selectedId: 0
    signal buttonClicked(emitter: int)

    DCSideRoundedRect{
        id:internLeftButton
        x:0
        height: root.height
        width: root.width/2
        color: checked ? props.buttonColorActive : DCStyle.button.bgColor
        borderColor:checked ? buttonColor : DCStyle.button.borderColor
        borderWidth: 1
        //recOpacity: DCStyle.button.bgOpacity
        recRadius: DCStyle.radius
        recRoundSide:DCSideRoundedRect.RoundedSide.Left
        property bool checked: selectedId === 0 ? true : false
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
            hoverEnabled: true
            onClicked: {
                if(parent.checked === true)
                    return;
                parent.checked = true;
                buttonClicked(0);
                root.selectedId = 0
                parent.borderColor = parent.checked ? root.buttonColor : parent.borderColor
                parent.color = parent.checked ? props.buttonColorActive : parent.color
                internLeftText.color = internLeftButton.checked ? buttonColor : internLeftText.color
            }
            onHoveredChanged: {
                if(containsMouse && !parent.checked)
                {
                    parent.borderColor = DCStyle.button.borderColorHovered
                    internLeftText.color = DCStyle.button.textColorHovered
                    parent.color = DCStyle.button.bgColorHovered
                }
                else
                {
                    parent.borderColor = Qt.binding(function() { return parent.checked ? root.buttonColor : DCStyle.button.borderColor})
                    parent.color = Qt.binding(function() { return  parent.checked ? props.buttonColorActive : DCStyle.button.bgColor})
                    internLeftText.color = Qt.binding(function() { return internLeftButton.checked ? buttonColor : DCStyle.button.textColor})
                }
            }

            onPressed: {
                parent.borderColor = parent.checked ? root.buttonColor : parent.borderColor
                parent.color = parent.checked ? props.buttonColorActive : DCStyle.button.bgColorDown
                internLeftText.color = internLeftButton.checked ? buttonColor : internLeftText.color
            }
        }
    }
    Text{
        id:internLeftText
        z:3
        x: (internLeftButton.width - contentWidth)/2
        y: (internLeftButton.height-contentHeight)/2
        color:internLeftButton.checked ? buttonColor : "grey"
        font.bold: internLeftButton.checked ? true : false
    }

    DCSideRoundedRect{
        id:internRightButton
        recRadius: DCStyle.radius
        x: root.width/2 -1
        height: root.height
        width: root.width/2
        //recOpacity:DCStyle.button.bgOpacity
        color: checked ? props.buttonColorActive : DCStyle.button.bgColor
        borderColor:checked ? buttonColor : DCStyle.button.borderColor
        borderWidth: 1
        recRoundSide:DCSideRoundedRect.RoundedSide.Right
        property bool checked: selectedId === 1 ? true : false
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
            hoverEnabled: true
            onClicked: {
                if(parent.checked === true)
                    return;
                parent.checked = true;
                buttonClicked(1);
                root.selectedId = 1
                parent.borderColor = parent.checked ? root.buttonColor : parent.borderColor
                parent.color = parent.checked ? props.buttonColorActive : parent.color
                internRightText.color = internRightButton.checked ? buttonColor : internRightText.color
            }
            onHoveredChanged: {
                if(containsMouse && !parent.checked)
                {
                    parent.borderColor = DCStyle.button.borderColorHovered
                    internRightText.color = DCStyle.button.textColorHovered
                    parent.color = DCStyle.button.bgColorHovered
                }
                else
                {
                    parent.borderColor = Qt.binding(function() { return parent.checked ? buttonColor : DCStyle.button.borderColor})
                    internRightText.color = Qt.binding(function() { return internRightButton.checked ? buttonColor : DCStyle.button.textColor})
                    parent.color = Qt.binding(function() { return  parent.checked ? props.buttonColorActive : DCStyle.button.bgColor})
                }
            }
            onPressed: {
                parent.borderColor = parent.checked ? root.buttonColor : parent.borderColor
                parent.color = parent.checked ? props.buttonColorActive : DCStyle.button.bgColorDown
                internRightText.color = internRightButton.checked ? buttonColor : internRightText.color
            }
        }
    }
    Text{
        id:internRightText
        z:2
        y: (internRightButton.height - contentHeight)/2
        anchors.left: internRightButton.left
        anchors.leftMargin: (internRightButton.width - contentWidth)/2
        opacity:1.0
        color:internRightButton.checked ? buttonColor : DCStyle.button.textColor
        font.bold: internRightButton.checked ? true : false

    }

    Rectangle{
        id: border
        z:2
        width: 1
        height: parent.height
        color: buttonColor
        anchors.centerIn: parent
    }

}
