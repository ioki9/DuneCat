import QtQuick
import QtQuick.Controls
import DCStyle
RoundButton {
    id:control
    //property double radius: DCStyle.radius
    property color backgroundActiveColor: DCStyle.primaryColor
    property color backgroundIdleColor: "white"
    property color buttonColor: DCStyle.primaryColor
    property bool activated: false
    Material.foreground: props.textColor
    QtObject{
        id:props
        property color textColor: DCStyle.button.textColor
        property color bgColorActivated: Qt.rgba(buttonColor.r,buttonColor.g,buttonColor.b,DCStyle.button.bgOpacity)
        property color bgColorActivatedHovered: Qt.rgba(buttonColor.r,buttonColor.g,buttonColor.b,DCStyle.button.bgOpacity + 0.07)
        property color bgColorActivatedDown: Qt.rgba(bgColorActivatedHovered.r,bgColorActivatedHovered.g,
                                                     bgColorActivatedHovered.b,bgColorActivatedHovered.a + 0.07)
    }
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    icon.color: Material.foreground
    function updateDelegateColor()
    {
        if(control.down)
        {
            delegate.color = control.activated ? props.bgColorActivatedDown : DCStyle.button.bgColorDown
            delegate.border.color = control.activated ? buttonColor : DCStyle.button.borderColorDown
            props.textColor = control.activated ? buttonColor : DCStyle.button.textColorDown
        }
        else
        {
            if(control.hovered)
            {
                delegate.color = control.activated ? props.bgColorActivatedHovered : DCStyle.button.bgColorHovered
                delegate.border.color = control.activated ? buttonColor : DCStyle.button.borderColorHovered
                props.textColor = control.activated ? buttonColor : DCStyle.button.textColorHovered
            }
            else
            {
                delegate.color = control.activated ? props.bgColorActivated : DCStyle.button.bgColor
                delegate.border.color = control.activated ? buttonColor : DCStyle.button.borderColor
                props.textColor = control.activated ? buttonColor : DCStyle.button.textColor
            }
        }
    }
    onClicked: {
        activated = !activated
        updateDelegateColor()
    }

    onHoveredChanged:updateDelegateColor()
    onDownChanged:updateDelegateColor()
    background:Rectangle{
        id:delegate
        anchors.fill:control
        opacity: enabled ? 1 : 0.3
        color: DCStyle.button.bgColor
        border.color: DCStyle.button.borderColor
        border.width: 1
        radius:control.radius
    }

}
