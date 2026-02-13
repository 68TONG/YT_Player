import QtQuick
import YT_Player

Rectangle {
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

    radius: YT_Info.Radius
    color: YT_Info.ItemFocusColor

    Behavior on x {
        enabled: true
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutQuad
        }
    }
    Behavior on y {
        enabled: true
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutQuad
        }
    }
    Behavior on width {
        enabled: false
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutQuad
        }
    }
    Behavior on height {
        enabled: false
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutQuad
        }
    }
    Behavior on opacity {
        enabled: true
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutQuad
        }
    }
}
