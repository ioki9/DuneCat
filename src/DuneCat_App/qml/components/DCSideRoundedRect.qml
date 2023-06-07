import QtQuick
import QtQuick.Controls

Item {
    enum RectangleSide{
        None,
        Left,
        Right
    }
    layer.enabled: true
    opacity:1.0
    required property int recRadius
    required property int recRadiusSide
    property color recColor: "white"


    Rectangle {
      id: leftRect
      radius: recRadiusSide === DCSideRoundedRect.RectangleSide.Left ? recRadius : 0
      color: recColor
      width: recRadiusSide === DCSideRoundedRect.RectangleSide.Left ? parent.width : recRadius
      height:parent.height
      anchors.left: parent.left
    }

    Rectangle {
      id: rightRect
      radius: recRadiusSide === DCSideRoundedRect.RectangleSide.Right ? recRadius : 0
      color: recColor
      height: parent.height
      width: (recRadiusSide === DCSideRoundedRect.RectangleSide.Right ||
              recRadiusSide === DCSideRoundedRect.RectangleSide.None) ? parent.width : recRadius
      anchors.right : parent.right
    }
}
