import QtQuick
import QtQuick.Controls
import DCComponents
import DCStyle
import "qrc:/DuneCat/imports/qml/components"
Rectangle {
    id:root
    property var dateOrderList: []
    property color styleColor: DCStyle.primaryColor
    property int fontPointSize: 10
    property var currentLocale: Qt.locale()
    property bool active
    property string dateDelim:getDateDelimeter(dateTimeFormat)
    property string dateTimeFormat: currentLocale.dateFormat(Locale.ShortFormat)
    property date minDate: new Date(2023,0,1)
    property date maxDate: new Date(2099,0,1)

    activeFocusOnTab: true
    enum Date{
        Day,
        Month,
        Year
    }

    QtObject{
        id:internal
        property real leftMargin
        property real topMargin
    }
    onActiveChanged: root.border.color = root.active ? styleColor : "black"
    Component.onCompleted: {
        setDateOrderList(dateTimeFormat)
        internal.leftMargin = (root.width - textMetrics.advanceWidth) / 2
        internal.topMargin = (root.height - textMetrics.height) / 2
    }

    function getDateDelimeter(dtFormat)
    {
        for(var i = 0;i<dtFormat.length;i++)
        {
            if(dtFormat[i] !== 'd' && dtFormat[i] !== 'D'
                && dtFormat[i] !== 'M' && dtFormat[i] !== 'm'
                && dtFormat[i] !== 'y' && dtFormat[i] !== 'Y')
            {
                return dtFormat[i]
            }
        }
        return undefined
    }
    function setDateOrderList(dtFormat)
    {
        var i = 0
        for(i; i < dtFormat.length; i++)
        {
            if(dtFormat[i] === dateDelim)
            {
                i++
                break;
            }
            if(dtFormat[i] === 'd' || dtFormat[i] === 'D')
                dateOrderList[DCDatePicker.Date.Day] = 0
            else if (dtFormat[i] === 'm' || dtFormat[i] === 'M')
                dateOrderList[DCDatePicker.Date.Month] = 0
            else if(dtFormat[i] === 'y' || dtFormat[i] === 'Y')
                dateOrderList[DCDatePicker.Date.Year] = 0

        }
        for(i; i < dtFormat.length; i++)
        {
            if(dtFormat[i] === dateDelim)
            {
                i++
                break;
            }
            if(dtFormat[i] === 'd' || dtFormat[i] === 'D')
                dateOrderList[DCDatePicker.Date.Day] = 1
            else if (dtFormat[i] === 'm' || dtFormat[i] === 'M')
                dateOrderList[DCDatePicker.Date.Month] = 1
            else if(dtFormat[i] === 'y' || dtFormat[i] === 'Y')
                dateOrderList[DCDatePicker.Date.Year] = 1

        }
        for(i;i < dtFormat.length; i++)
        {
            if(dtFormat[i] === 'd' || dtFormat[i] === 'D')
            {
                dateOrderList[DCDatePicker.Date.Day] = 2
                break
            }
            else if (dtFormat[i] === 'm' || dtFormat[i] === 'M')
            {
                dateOrderList[DCDatePicker.Date.Month] = 2
                break
            }
            else if(dtFormat[i] === 'y' || dtFormat[i] === 'Y')
            {
                dateOrderList[DCDatePicker.Date.Year] = 2
                break
            }
        }
    }
    function deselectAllInputs()
    {
        inputDay.focus = false
        inputDay.deselect()
        inputMonth.deselect()
        inputMonth.focus = false
        inputYear.deselect()
        inputYear.focus = false

    }
    TextMetrics{
        id:textMetrics
        text:"12/12/2023"
        font.pointSize: fontPointSize
    }

    Text{
        id:dateFirstDelim
        text:dateDelim
        anchors.top: root.top
        anchors.topMargin: internal.topMargin
        font.pointSize: fontPointSize
        Component.onCompleted: {
            if(dateOrderList[DCDatePicker.Date.Day] === 0)
                anchors.left = Qt.binding(function(){return inputDay.right})
            else if(dateOrderList[DCDatePicker.Date.Month] === 0)
                anchors.left = Qt.binding(function(){return inputMonth.right})
            else
                anchors.left = Qt.binding(function(){return inputYear.right})
        }

    }
    Text{
        id:dateSecondDelim
        text:dateDelim
        anchors.top: root.top
        anchors.topMargin: internal.topMargin
        font.pointSize: fontPointSize
        Component.onCompleted: {
            if(dateOrderList[DCDatePicker.Date.Day] === 1)
                anchors.left = Qt.binding(function(){return inputDay.right})
            else if(dateOrderList[DCDatePicker.Date.Month] === 1)
                anchors.left = Qt.binding(function(){return inputMonth.right})
            else
                anchors.left = Qt.binding(function(){return inputYear.right})
        }
    }
    DCTextInputInt{
        id:inputDay
        z:2
        anchors.top: root.top
        anchors.topMargin: internal.topMargin
        minTextLength: 2
        selectionColor: styleColor
        Component.onCompleted: {
            if(dateOrderList[DCDatePicker.Date.Day] === 0)
            {
                anchors.left = Qt.binding(function(){return root.left})
                anchors.leftMargin = internal.leftMargin
            }
            else if(dateOrderList[DCDatePicker.Date.Day] === 1)
                anchors.left = Qt.binding(function(){return dateFirstDelim.right})
            else
                anchors.left = Qt.binding(function(){return dateSecondDelim.right})

            dcValidator.bottom = Qt.binding(function(){return minDate.getDate()})
            dcValidator.top = Qt.binding(function(){return maxDate.getDate()})
        }
        onTextEdited: {
            if(text.length < minTextLength)
                undo()
        }

        font.pointSize: fontPointSize
        inputMask:"99"
        height: root.height
        text:"01"
        onFocusChanged: {
            if(focus)
            {
                inputDay.deselect()
                inputYear.deselect()
                root.active = true
            }
            else
            {
                root.active = false
            }
        }

        MouseArea
        {
            propagateComposedEvents: true
            anchors.fill: parent
            hoverEnabled: true
            onHoveredChanged: {
                if(containsMouse)
                    cursorShape = Qt.IBeamCursor
                else
                    cursorShape = Qt.ArrowCursor
            }
            onClicked: (mouse)=>{mouse.accepted = false;}
            onPressed: (mouse)=>{mouse.accepted = false;}
            onReleased: (mouse)=>{mouse.accepted = false;}
            onDoubleClicked: (mouse)=>{mouse.accepted = false;}
            onPositionChanged: (mouse)=>{mouse.accepted = false;}
            onPressAndHold:(mouse)=>{mouse.accepted = false;}
        }
    }
    DCTextInputInt{
        id:inputMonth
        z:2
        anchors.top: root.top
        anchors.topMargin: internal.topMargin
        minTextLength: 2
        selectionColor: styleColor
        Component.onCompleted: {
            if(dateOrderList[DCDatePicker.Date.Month] === 0)
            {
                anchors.left = Qt.binding(function(){return root.left})
                anchors.leftMargin = internal.leftMargin
            }
            else if(dateOrderList[DCDatePicker.Date.Month] === 1)
                anchors.left = Qt.binding(function(){return dateFirstDelim.right})
            else
                anchors.left = Qt.binding(function(){return dateSecondDelim.right})

            dcValidator.bottom = Qt.binding(function(){return minDate.getMonth()})
            dcValidator.top = Qt.binding(function(){return maxDate.getMonth()})
        }
        font.pointSize: fontPointSize
        height: root.height
        inputMask:"99"
        text:"01"
        onFocusChanged: {
            if(focus)
            {
                inputDay.deselect()
                inputYear.deselect()
                root.active = true
            }
            else
            {
                root.active = false
            }
        }

        MouseArea
        {
            propagateComposedEvents: true
            anchors.fill: parent
            hoverEnabled: true
            onHoveredChanged: {
                if(containsMouse)
                    cursorShape = Qt.IBeamCursor
                else
                    cursorShape = Qt.ArrowCursor
            }
            onClicked: (mouse)=>{mouse.accepted = false;}
            onPressed: (mouse)=>{mouse.accepted = false;}
            onReleased: (mouse)=>{mouse.accepted = false;}
            onDoubleClicked: (mouse)=>{mouse.accepted = false;}
            onPositionChanged: (mouse)=>{mouse.accepted = false;}
            onPressAndHold:(mouse)=>{mouse.accepted = false;}
        }
    }
    DCTextInputInt{
        id:inputYear
        z:2
        minTextLength: 4
        anchors.top: root.top
        anchors.topMargin: internal.topMargin
        selectionColor: styleColor
        Component.onCompleted: {
            if(dateOrderList[DCDatePicker.Date.Year] === 0)
            {
                anchors.left = Qt.binding(function(){return root.left})
                anchors.leftMargin = internal.leftMargin

            }
            else if(dateOrderList[DCDatePicker.Date.Year] === 1)
                anchors.left = Qt.binding(function(){return dateFirstDelim.right})
            else
                anchors.left = Qt.binding(function(){return dateSecondDelim.right})
            dcValidator.bottom = Qt.binding(function(){return minDate.getFullYear()})
            dcValidator.top = Qt.binding(function(){return maxDate.getFullYear()})
        }
        font.pointSize: fontPointSize
        height: root.height
        //using 0099 mask for correct cursor position on first digit
        inputMask:"0099"
        text:"2023"
        onFocusChanged: {
            if(focus)
            {
                inputDay.deselect()
                inputYear.deselect()
                root.active = true
            }
            else
            {
                root.active = false
            }
        }

        MouseArea
        {
            propagateComposedEvents: true
            anchors.fill: parent
            hoverEnabled: true
            onHoveredChanged: {
                if(containsMouse)
                    cursorShape = Qt.IBeamCursor
                else
                    cursorShape = Qt.ArrowCursor
            }
            onClicked: (mouse)=>{mouse.accepted = false;}
            onPressed: (mouse)=>{mouse.accepted = false;}
            onReleased: (mouse)=>{mouse.accepted = false;}
            onDoubleClicked: (mouse)=>{mouse.accepted = false;}
            onPositionChanged: (mouse)=>{mouse.accepted = false;}
            onPressAndHold:(mouse)=>{mouse.accepted = false;}
        }
    }
}
