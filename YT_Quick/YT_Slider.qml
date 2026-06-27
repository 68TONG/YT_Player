import QtQuick
import QtQuick.Controls
import YT_Player

Slider {
    id: root
    value: 0
    padding: 0
    handle: null

    property int backgroundSize: 4
    property int backgroundRadius: background.height / 2
    property color backgroundColor: YT_Info.ItemColor
    property color backgroundThroughColor: YT_Info.ItemFocusColor
    background: Rectangle {
        width: parent.availableWidth
        height: parent.backgroundSize
        color: root.backgroundColor
        radius: root.backgroundRadius
        Rectangle {
            width: root.visualPosition * parent.width
            height: parent.height
            color: root.backgroundThroughColor
            radius: root.backgroundRadius
        }
    }
    HoverHandler {
        cursorShape: Qt.PointingHandCursor // 设置鼠标悬停时的样式为手形
    }
}
