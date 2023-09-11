import QtQuick
import QtQuick.Controls
import DCComponents
TextInput {
    property var dcValidator: DCIntValidator{}
    property int minTextLength: Number.MAX_VALUE
    QtObject{
        id:saved
        property string savedText:savedText = text
    }
    //Component.onCompleted: minTextLength = inputMask.length ? inputMask.length : Number.MAX_VALUE
    onTextEdited: {
        console.log(text.length)
        if(text.length < minTextLength || dcValidator.validate(text) === false)
        {
            var oldCursorPosition = cursorPosition
            if(text.length < minTextLength)
                oldCursorPosition += 2
            text = saved.savedText
            cursorPosition = oldCursorPosition - 1

        }
        else
        {
            saved.savedText = text
        }
    }
}
