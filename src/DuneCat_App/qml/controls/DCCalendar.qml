import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DCStyle
//TODO: animation(opacity, small movement), calendar icon
Rectangle{
    id:root
    property color styleColor:DCStyle.primaryColor
    border.width: 1
    border.color: "black"
    radius:7

    width:layoutGrid.width + 15
    height:childrenRect.height + 15
    property var locale: Qt.locale()
    property date currentDate: new Date()
    property date maxDate: new Date()
    property date minDate: new Date(2022,6,23)
    signal closed()
    Rectangle{
        id: header
        height: 55
        color:"transparent"
        anchors.top: parent.top
        anchors.topMargin: 1
        anchors.left: parent.left
        anchors.leftMargin: 1
        anchors.right: parent.right
        anchors.rightMargin: 1
        Rectangle{
            anchors.centerIn: parent
            width:childrenRect.width
            height: childrenRect.height
            Text {
                id: monthText
                text: root.locale.monthName(layoutGrid.monthGrid.month)
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenterOffset: 7
                anchors.verticalCenter: parent.verticalCenter
                anchors.left:parent.left
                font.pointSize: 12
            }
            Button{
                id: unfoldMonth
                height:22
                width:22
                anchors.left: monthText.right
                anchors.leftMargin: 2
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 7
                leftPadding : 0
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0
                padding:0
                spacing: 0
                icon.name: "unfold_more"
                icon.width: 40
                icon.height: 40
                icon.color: unfoldMonth.hovered ? "white" : "black"
                background:Rectangle{
                    id:unfoldMonthDelegate
                    radius:DCStyle.radius
                    anchors.fill: parent
                    color: unfoldMonth.hovered ? root.styleColor : "transparent"
                }
            }

            Text {
                id: yearText
                text:layoutGrid.monthGrid.year
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenterOffset: 7
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: unfoldMonth.right
                anchors.leftMargin: 2
                font.pointSize: 12
            }
            Button{
                id: unfoldYear
                height:22
                width:22
                anchors.left: yearText.right
                anchors.leftMargin: 2
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 7
                leftPadding : 0
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0
                padding:0
                spacing: 0
                icon.name: "unfold_more"
                icon.width: 40
                icon.height: 40
                icon.color: unfoldYear.hovered ? "white" : "black"
                background:Rectangle{
                    id:unfoldYearDelegate
                    radius:DCStyle.radius
                    anchors.fill: parent
                    color: unfoldYear.hovered ? root.styleColor : "transparent"
                }
            }
        }

        DCCloseButton{
            id:closeButton
            width:20
            height:20
            z:3
            icon.name: "close"
            anchors.top:parent.top
            anchors.right: parent.right
            anchors.topMargin: 3
            anchors.rightMargin: 3
            onClosed:root.closed()
        }

        Button{
            id: leftArrow
            height:22
            width:22
            anchors.left: header.left
            anchors.leftMargin: 6
            anchors.verticalCenter: header.verticalCenter
            anchors.verticalCenterOffset: 7
            leftPadding : 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
            padding:0
            spacing: 0
            icon.name: "navigate_before"
            icon.width: 40
            icon.height: 40
            icon.color: leftArrow.hovered ? "white" : "black"
            background:Rectangle{
                id:leftArrowDelegate
                radius:DCStyle.radius
                anchors.fill: parent
                color: leftArrow.hovered ? root.styleColor : "transparent"
            }

            onClicked: {
                root.currentDate.setMonth(root.currentDate.getMonth() - 1)
                root.currentDateChanged()
            }
        }
        Button{
            id: rightArrow
            height:22
            width:22
            leftPadding : 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
            anchors.right: header.right
            anchors.rightMargin: 6
            anchors.verticalCenter: header.verticalCenter
            anchors.verticalCenterOffset: 7
            spacing: 0
            icon.name: "navigate_next"
            icon.width: 40
            icon.height: 40
            icon.color: rightArrow.hovered ? "white" : "black"
            background:Rectangle{
                id:rightArrowDelegate
                radius:DCStyle.radius
                width: rightArrow.width
                height: rightArrow.height
                anchors.centerIn: rightArrow
                color: rightArrow.hovered ? root.styleColor : "transparent"
            }
            onClicked: {
                root.currentDate.setMonth(root.currentDate.getMonth() + 1)
                root.currentDateChanged()
            }
        }
    }

    GridLayout {
        id:layoutGrid
        columns: 2
        anchors.top: header.bottom
        anchors.horizontalCenter: root.horizontalCenter
        property alias monthGrid : grid

        DayOfWeekRow {
            locale: root.locale
            font.pointSize: 8
            Layout.column: 1
            Layout.fillWidth: true
        }

        WeekNumberColumn {
            month: grid.month
            year: grid.year
            locale: grid.locale
            font.pointSize: 8
            Layout.fillHeight: true
        }

        MonthGrid {
            id: grid
            locale: root.locale
            font.pointSize: 8
            year:root.currentDate.getFullYear()
            month: root.currentDate.getMonth()
            Connections{
                target:root
                function onCurrentDateChanged(){
                    grid.year = root.currentDate.getFullYear()
                    grid.month = root.currentDate.getMonth()
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 1
            delegate: Rectangle{
                id: gridDelegate
                required property var model
                radius: DCStyle.radius
                color: "transparent"
                Text{
                    id:dayText
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    opacity: {

                        if(model.month === grid.month)
                        {
                            if(model.date >= root.minDate && model.date <= root.maxDate)
                                return 1
                            else
                                return 0.3
                        }
                        else
                            return 0.3
                    }
                    text: model.day
                    font: grid.font
                    color: "black"

                }
                MouseArea{
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged:{
                        if(dayText.opacity !== 1)
                            return
                        if(containsMouse)
                        {
                            gridDelegate.color ="orange"
                            dayText.color = "white"
                        }
                        else
                        {
                            gridDelegate.color ="transparent"
                            dayText.color = "black"
                        }
                    }
                }
            }
        }
    }
}


