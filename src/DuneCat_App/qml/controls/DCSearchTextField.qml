import QtQuick
import QtQuick.Controls
import DCStyle
TextField{
    id:searchTextField
    leftPadding:searchImage.width + 5
    topPadding: 10
    Image{
        id:searchImage
        source:"qrc:/DuneCat/imports/icons/20x20@2/search.png"
        width:parent.height - 3
        height:parent.height - 3
        anchors.left: parent.left
        anchors.leftMargin: 3
        anchors.verticalCenter: parent.verticalCenter
    }
    placeholderTextColor: searchTextField.activeFocus ?  DCStyle.primaryColor : "lightgrey"
    background:Rectangle{
        radius: DCStyle.radius
        width:searchTextField.width
        height:searchTextField.height
        color: "transparent"
        border.color: {
             if(searchTextField.activeFocus)
                return DCStyle.primaryColor
            else if(searchTextField.hovered)
                return "grey"
            else
                return "lightgrey"
        }
        Item{
            x:searchTextField.leftPadding - 4
            y:0
            z:2
            height:parent.border.width
            width:placeholderTextMetrics.boundingRect.width/2
            clip:true

            Rectangle{
                x:searchTextField.activeFocus ? 0 : width
                z:2
                height:1
                width:placeholderTextMetrics.boundingRect.width/2
                color:searchTextField.parent.color
                Behavior on x{
                    NumberAnimation{
                        duration:300
                    }
                }
            }
        }
        Item{
            x:searchTextField.leftPadding + width - 4
            y:0
            z:2
            height:parent.border.width
            width:placeholderTextMetrics.boundingRect.width/2
            clip:true

            Rectangle{
                x:searchTextField.activeFocus ? 0 : (-width)
                z:2
                height:1
                width:parent.width
                color:searchTextField.parent.color
                Behavior on x{
                    NumberAnimation{
                        duration:300
                    }
                }

            }
        }

        TextMetrics{
            id:placeholderTextMetrics
            font:searchTextField.font
            text: searchTextField.placeholderText
        }
    }
}
