import QtQuick
import DCStyle
import QtQuick.Controls
Text{
    id:root
    property var itemList: []
    property color styleColor: DCStyle.primaryColor
    property alias popupObj: popup
    signal selected(item: var,index:int)
    property int positionViewIndex: 0
    //workaround property binding loop when using return statement inside the function. Qt creator gives the warning even if binding isn't used.
    Component.onCompleted: setPopupWidth()
    onItemListChanged: setPopupWidth()
    MouseArea{
        anchors.fill:parent
        onClicked:{popup.open();listView.positionViewAtIndex(positionViewIndex,ListView.Center)}
        hoverEnabled: true
        onHoveredChanged: cursorShape = containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
    TextMetrics{
        id:metrics
        font: root.font
    }

    function setPopupWidth()
    {
        var maxWidth = 0

        for( var i = 0; i < itemList.length; i++ ) {
            metrics.text = itemList[i]
            maxWidth = Math.max(maxWidth,metrics.advanceWidth)
        }
        popup.width = Math.max(24,(maxWidth + 15))
    }

    Popup{
        id: popup
        padding: 1
        modal: true
        focus: true
        x:0
        y:parent.y + parent.height + 2
        height: 200
        width: 100

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        property alias listView : listView
        contentItem: ListView {
            id:listView
            clip: true
            highlightFollowsCurrentItem: true
            model: popup.visible ? root.itemList : null
            delegate:listDelegate
            spacing: 5
            width:parent.width
            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            radius: 5
        }
        Component {
            id: listDelegate
            ItemDelegate {
                id: itemDelegate
                width: popup.listView.width
                height:itemText.contentHeight + 3
                //anchors.horizontalCenter: parent.horizontalCenter
                spacing: 5
                Text{
                    id:itemText
                    text:modelData
                    font:root.font
                    horizontalAlignment: Text.AlignHCenter
                    color: itemDelegate.highlighted ? "white" : "black";
                    anchors.left:parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                }
                background:Rectangle{
                    id:bgDelegate
                    anchors.fill: itemDelegate
                    color: itemDelegate.highlighted ? root.styleColor : "transparent"
                    radius: DCStyle.radius
                }
                highlighted: listView.currentIndex === index
                onHoveredChanged: hovered ? listView.currentIndex = index : undefined
                onClicked:{
                    root.text = modelData
                    root.selected(modelData,index)
                    popup.close()
                }
            }
        }
    }
}
