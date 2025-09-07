import QtQuick
import QtQuick.Controls.Basic

import YT_Player

Slider {
    id: root
    value: 0
    padding: 0

    property int contentItem_Size: 4
    background: Rectangle {
        // y: parent.topPadding + parent.availableHeight - height
        width: parent.availableWidth
        height: parent.contentItem_Size

        radius: height / 2
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemColor)

        Rectangle {
            width: root.visualPosition * parent.width
            height: parent.height
            color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)
            radius: parent.radius
        }
    }
    handle: null

    HoverHandler {
        cursorShape: Qt.PointingHandCursor // 设置鼠标悬停时的样式为手形
    }
}
