import QtQuick
import QtQuick.Controls
import YT_Player

TextField {
    id: root
    hoverEnabled: true

    padding: YT_Info.MarginSmall
    leftPadding: padding
    rightPadding: padding
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignLeft

    color: YT_Info.FontColor
    placeholderTextColor: color

    background: YT_Rectangle {
        color: (parent.focus || parent.hovered) ? YT_Info.ItemFocusColor : YT_Info.ItemColor
    }
    // onEditingFinished: focus = false
}
