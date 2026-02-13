import QtQuick
import QtQuick.Controls.Basic

import YT_Player

TextField {
    id: root
    hoverEnabled: true

    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignLeft

    placeholderText: "输入框"
    placeholderTextColor: color
    color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)

    background: Rectangle {
        id: rootBackground
        anchors.fill: parent
        color: hovered
               ? YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)
               : Qt.rgba(0, 0, 0, 0)
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
    }
    onEditingFinished: focus = false
}
