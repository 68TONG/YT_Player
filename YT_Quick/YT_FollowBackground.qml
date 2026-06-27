import QtQuick
import QtQuick.Effects
import YT_Player

Item {
    id: root
    property Item followItem: null
    property int widthPadding: 0
    property int heightPadding: 0

    property alias background: background
    property alias radius: background.radius
    property alias color: background.color
    property alias enabledShadow: loadShadow.active
    property color shadowColor

    z: -true
    x: followItem ? followItem.x + widthPadding : x
    y: followItem ? followItem.y + heightPadding : y
    width: followItem ? followItem.width - widthPadding * 2 : width
    height: followItem ? followItem.height - heightPadding * 2 : height
    opacity: followItem !== null
    visible: opacity !== 0

    Rectangle {
        id: background
        anchors.fill: parent
        radius: YT_Info.Radius
        color: YT_Info.ItemFocusColor
    }
    Loader {
        id: loadShadow
        active: false
        z: -true
        anchors.fill: background
        sourceComponent: MultiEffect {
            source: background
            shadowEnabled: true
            shadowColor: root.shadowColor
            shadowBlur: 0.3
        }
    }

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
