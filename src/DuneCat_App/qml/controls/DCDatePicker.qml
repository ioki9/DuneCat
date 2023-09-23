import QtQuick
import QtQuick.Controls
import DCComponents
import DCStyle
import "qrc:/DuneCat/imports/qml/components"
Rectangle {
    id:root
    property var dateOrderList: []
    property color styleColor: DCStyle.primaryColor
    property color inactiveColor: "lightgrey"
    property int fontPixelSize: DCStyle.font.pixelSize.input
    property var currentLocale: Qt.locale()
    property bool active
    property string dateDelim:getDateDelimeter(localeDateFormat)
    property string localeDateFormat: currentLocale.dateFormat(Locale.ShortFormat)
    property date minDate: new Date(2023,0,1)
    property date maxDate: new Date(2099,0,1)
    property date currentDate: new Date()
    property int iconWidth:24
    property int iconHeight: 24
    implicitWidth: inputButtonRect.width + 20
    implicitHeight: inputButtonRect.height + 10
    activeFocusOnTab: true
    border.color: inactiveColor
    onActiveChanged: root.border.color = getRelevantBorderColor()
    Component.onCompleted: {
        setDateOrderList(localeDateFormat)
        internal.dtFormat = getDateFormat(dateOrderList)
    }
    enum Date{
        Day,
        Month,
        Year
    }
    QtObject{
        id:internal
        property string dtFormat
    }
    function getDateFormat(dateOrder)
    {

        var result = ""
        if(dateOrder[DCDatePicker.Date.Day] === 0)
           result += "dd"
        else if(dateOrder[DCDatePicker.Date.Month] === 0)
           result += "MM"
        else
           result += "yyyy"

       result += dateDelim

        if(dateOrder[DCDatePicker.Date.Day] === 1)
           result += "dd"
        else if(dateOrder[DCDatePicker.Date.Month] === 1)
           result += "MM"
        else
           result += "yyyy"

       result += dateDelim

        if(dateOrder[DCDatePicker.Date.Day] === 2)
           result += "dd"
        else if(dateOrder[DCDatePicker.Date.Month] === 2)
           result += "MM"
        else
           result += "yyyy"

        return result
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

    function dateFromatToInputMask(dtFormat)
    {
        var mask = internal.dtFormat.replace("MM","99")
        mask = mask.replace("dd","99")
        mask = mask.replace("yyyy","2999")
        return mask
    }

    function dateToInputText(date,dtFormat)
    {
        var txt = internal.dtFormat.replace("MM",(date.getMonth()+1) <= 10 ? ("0" + (date.getMonth()+1).toString()) : (date.getMonth()+1).toString())
        txt  = txt.replace("dd",date.getDate() <= 10 ? ("0" + date.getDate().toString()) : (date.getDate().toString()))
        txt = txt.replace("yyyy",date.getFullYear())
        return txt
    }

    function getRelevantBorderColor()
    {
        if(active)
            return styleColor
        else if(mainRectMouseArea.containsMouse || inputButtonRect.textInputMouseArea.containsMouse || calendarButton.hovered)
            return "grey"
        else
            return "lightgrey"
    }

    MouseArea{
        id: mainRectMouseArea
        anchors.fill:parent
        hoverEnabled: true
        propagateComposedEvents: true
        onHoveredChanged: {
            root.border.color = getRelevantBorderColor()
        }
        onClicked: (mouse)=>{mouse.accepted = false;}
        onPressed: (mouse)=>{mouse.accepted = false;}
        onReleased: (mouse)=>{mouse.accepted = false;}
        onDoubleClicked: (mouse)=>{mouse.accepted = false;}
        onPositionChanged: (mouse)=>{mouse.accepted = false;}
        onPressAndHold:(mouse)=>{mouse.accepted = false;}
    }

    Rectangle{
        id: inputButtonRect
        anchors.centerIn: parent
        width: calendarButton.width + textInput.width + textInput.anchors.leftMargin
        height: calendarButton.height
        property alias textInputMouseArea: textInput.textInputMouseArea
        property alias calendarButton: calendarButton
        RoundButton{
            id:calendarButton
            width:iconWidth
            height:iconHeight
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            checkable: true
            flat:true
            leftPadding:0
            rightPadding: 0
            topPadding: 0
            bottomPadding:0
            onCheckedChanged:{calendar.visible = checked; calendar.focus = checked}
            background: IconImage{
                name: "calendar_month"
                color: calendarButton.checked ? styleColor : "grey"
                width: calendarButton.width
                height: width
                anchors.centerIn: parent
            }

        }
        DCCalendar{
            id:calendar
            z:4
            anchors.top:calendarButton.bottom
            visible: false
            onSelected: (date)=>{root.currentDate = date}
            onClosed: {visible = false; calendarButton.checked = false}
        }
        TextInput{
            id: textInput
            property alias textInputMouseArea : textInputMouseArea
            anchors.left: calendarButton.right
            anchors.leftMargin: 3
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: fontPixelSize
            validator: DCDateValidator{}
            inputMask: dateFromatToInputMask(internal.dtFormat)
            text: dateToInputText(currentDate,internal.dtFormat)
            selectByMouse : false
            selectionColor: styleColor
            onFocusChanged: root.active = focus
            Connections{
                target:internal
                function onDtFormatChanged(){
                    textInput.validator.setDateFormat(internal.dtFormat)
                    textInput.text = dateToInputText(currentDate,internal.dtFormat)
                    textInput.inputMask =  dateFromatToInputMask(internal.dtFormat)
                }
            }
            Connections{
                target:root
                function onCurrentDateChanged(){
                    textInput.text = dateToInputText(currentDate,internal.dtFormat)
                }
            }

            MouseArea
            {
                id:textInputMouseArea
                propagateComposedEvents: true
                anchors.fill: parent
                hoverEnabled: true
                onHoveredChanged:{
                    cursorShape = containsMouse ?  Qt.IBeamCursor : Qt.ArrowCursor
                    root.border.color = getRelevantBorderColor()
                }
                onClicked: (mouse)=>{mouse.accepted = false;}
                onPressed: (mouse)=>{mouse.accepted = false;}
                onReleased: (mouse)=>{mouse.accepted = false;}
                onDoubleClicked: (mouse)=>{mouse.accepted = false;}
                onPositionChanged: (mouse)=>{mouse.accepted = false;}
                onPressAndHold:(mouse)=>{mouse.accepted = false;}
            }
            Component.onCompleted: {
                validator.setMinDate(minDate)
                validator.setMaxDate(maxDate)
                validator.setDateFormat(internal.dtFormat)
            }
        }
    }


//    function deselectAllInputs()
//    {
//        inputDay.focus = false
//        inputDay.deselect()
//        inputMonth.deselect()
//        inputMonth.focus = false
//        inputYear.deselect()
//        inputYear.focus = false

//    }

//    Text{
//        id:dateFirstDelim
//        text:dateDelim
//        anchors.top: root.top
//        anchors.topMargin: internal.topMargin
//        font.pointSize: fontPointSize
//        Component.onCompleted: {
//            if(dateOrderList[DCDatePicker.Date.Day] === 0)
//                anchors.left = Qt.binding(function(){return inputDay.right})
//            else if(dateOrderList[DCDatePicker.Date.Month] === 0)
//                anchors.left = Qt.binding(function(){return inputMonth.right})
//            else
//                anchors.left = Qt.binding(function(){return inputYear.right})
//        }

//    }
//    Text{
//        id:dateSecondDelim
//        text:dateDelim
//        anchors.top: root.top
//        anchors.topMargin: internal.topMargin
//        font.pointSize: fontPointSize
//        Component.onCompleted: {
//            if(dateOrderList[DCDatePicker.Date.Day] === 1)
//                anchors.left = Qt.binding(function(){return inputDay.right})
//            else if(dateOrderList[DCDatePicker.Date.Month] === 1)
//                anchors.left = Qt.binding(function(){return inputMonth.right})
//            else
//                anchors.left = Qt.binding(function(){return inputYear.right})
//        }
//    }
//    TextInput{
//        id:inputDay
//        z:2
//        anchors.top: root.top
//        anchors.topMargin: internal.topMargin
//        minTextLength: 2
//        selectionColor: styleColor
//        property var dcValidator : DCDateValidator{}
//        Component.onCompleted: {
//            if(dateOrderList[DCDatePicker.Date.Day] === 0)
//            {
//                anchors.left = Qt.binding(function(){return root.left})
//                anchors.leftMargin = internal.leftMargin
//            }
//            else if(dateOrderList[DCDatePicker.Date.Day] === 1)
//                anchors.left = Qt.binding(function(){return dateFirstDelim.right})
//            else
//                anchors.left = Qt.binding(function(){return dateSecondDelim.right})

//        }
//        onTextEdited: {
//            if(text.length < minTextLength)
//                undo()
//        }

//        font.pointSize: fontPointSize
//        inputMask:"99"
//        height: root.height
//        text:"01"
//        onFocusChanged: {
//            if(focus)
//            {
//                inputDay.deselect()
//                inputYear.deselect()
//                root.active = true
//            }
//            else
//            {
//                root.active = false
//            }
//        }

//        MouseArea
//        {
//            propagateComposedEvents: true
//            anchors.fill: parent
//            hoverEnabled: true
//            onHoveredChanged: {
//                if(containsMouse)
//                    cursorShape = Qt.IBeamCursor
//                else
//                    cursorShape = Qt.ArrowCursor
//            }
//            onClicked: (mouse)=>{mouse.accepted = false;}
//            onPressed: (mouse)=>{mouse.accepted = false;}
//            onReleased: (mouse)=>{mouse.accepted = false;}
//            onDoubleClicked: (mouse)=>{mouse.accepted = false;}
//            onPositionChanged: (mouse)=>{mouse.accepted = false;}
//            onPressAndHold:(mouse)=>{mouse.accepted = false;}
//        }
//    }
//    TextInput{
//        id:inputMonth
//        z:2
//        anchors.top: root.top
//        anchors.topMargin: internal.topMargin
//        minTextLength: 2
//        selectionColor: styleColor
//        property var dcValidator : DCDateValidator{}

//        Component.onCompleted: {
//            if(dateOrderList[DCDatePicker.Date.Month] === 0)
//            {
//                anchors.left = Qt.binding(function(){return root.left})
//                anchors.leftMargin = internal.leftMargin
//            }
//            else if(dateOrderList[DCDatePicker.Date.Month] === 1)
//                anchors.left = Qt.binding(function(){return dateFirstDelim.right})
//            else
//                anchors.left = Qt.binding(function(){return dateSecondDelim.right})
//        }
//        font.pointSize: fontPointSize
//        height: root.height
//        inputMask:"99"
//        text:"01"
//        onFocusChanged: {
//            if(focus)
//            {
//                inputDay.deselect()
//                inputYear.deselect()
//                root.active = true
//            }
//            else
//            {
//                root.active = false
//            }
//        }

//        MouseArea
//        {
//            propagateComposedEvents: true
//            anchors.fill: parent
//            hoverEnabled: true
//            onHoveredChanged: {
//                if(containsMouse)
//                    cursorShape = Qt.IBeamCursor
//                else
//                    cursorShape = Qt.ArrowCursor
//            }
//            onClicked: (mouse)=>{mouse.accepted = false;}
//            onPressed: (mouse)=>{mouse.accepted = false;}
//            onReleased: (mouse)=>{mouse.accepted = false;}
//            onDoubleClicked: (mouse)=>{mouse.accepted = false;}
//            onPositionChanged: (mouse)=>{mouse.accepted = false;}
//            onPressAndHold:(mouse)=>{mouse.accepted = false;}
//        }
//    }
//    TextInput{
//        id:inputYear
//        z:2
//        minTextLength: 4
//        anchors.top: root.top
//        anchors.topMargin: internal.topMargin
//        selectionColor: styleColor
//        property var dcValidator : DCDateValidator{}
//        property string initialText
//        onEditingFinished: {
//            if(!dcValidator.validate(text))
//            {

//            }
//        }
//        onTextEdited: {
//            initialText = text
//        }

//        Component.onCompleted: {
//            if(dateOrderList[DCDatePicker.Date.Year] === 0)
//            {
//                anchors.left = Qt.binding(function(){return root.left})
//                anchors.leftMargin = internal.leftMargin
//                dcValidator.setDateFormat()
//            }
//            else if(dateOrderList[DCDatePicker.Date.Year] === 1)
//                anchors.left = Qt.binding(function(){return dateFirstDelim.right})
//            else
//                anchors.left = Qt.binding(function(){return dateSecondDelim.right})
//        }

//        font.pointSize: fontPointSize
//        height: root.height
//        //using 0099 mask for correct cursor position on first digit
//        inputMask:"0099"
//        text:"2023"
//        onFocusChanged: {
//            if(focus)
//            {
//                inputDay.deselect()
//                inputYear.deselect()
//                root.active = true
//            }
//            else
//            {
//                root.active = false
//            }
//        }

//        MouseArea
//        {
//            propagateComposedEvents: true
//            anchors.fill: parent
//            hoverEnabled: true
//            onHoveredChanged: {
//                if(containsMouse)
//                    cursorShape = Qt.IBeamCursor
//                else
//                    cursorShape = Qt.ArrowCursor
//            }
//            onClicked: (mouse)=>{mouse.accepted = false;}
//            onPressed: (mouse)=>{mouse.accepted = false;}
//            onReleased: (mouse)=>{mouse.accepted = false;}
//            onDoubleClicked: (mouse)=>{mouse.accepted = false;}
//            onPositionChanged: (mouse)=>{mouse.accepted = false;}
//            onPressAndHold:(mouse)=>{mouse.accepted = false;}
//        }
//    }
}
