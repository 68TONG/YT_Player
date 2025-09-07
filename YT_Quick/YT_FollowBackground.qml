import QtQuick
import QtQuick.Controls.Basic

import YT_Player

Rectangle {
    property bool enableAnimation: true
    property Item followItem: null
    property int widthPadding: 0
    property int heightPadding: 0

    z: -true
    x: followItem ? followItem.x + widthPadding : x
    y: followItem ? followItem.y + heightPadding : y
    width: followItem ? followItem.width - widthPadding * 2 : width
    height: followItem ? followItem.height - heightPadding * 2 : height
    opacity: followItem !== null
    visible: opacity !== 0

    radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
    color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)

    Behavior on x {
        enabled: enableAnimation
        NumberAnimation { duration: 300; easing.type: Easing.OutQuad}
    }
    Behavior on y {
        enabled: enableAnimation
        NumberAnimation { duration: 300; easing.type: Easing.OutQuad}
    }
    Behavior on width {
        enabled: false
        NumberAnimation { duration: 300; easing.type: Easing.OutQuad}
    }
    Behavior on height {
        enabled: false
        NumberAnimation { duration: 300; easing.type: Easing.OutQuad}
    }
    Behavior on opacity {
        enabled: enableAnimation
        NumberAnimation { duration: 300; easing.type: Easing.OutQuad}
    }
}
