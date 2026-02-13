import QtQuick

Rectangle {
    id: root

    visible: opacity !== 0
    radius: YT_Info.Radius
    color: YT_Info.BackgroundColor

    property int animationDuration: 300
    Behavior on color {
        ColorAnimation {
            duration: root.animationDuration
            easing.type: Easing.OutInQuad
        }
    }
    Behavior on opacity {
        NumberAnimation {
            duration: root.animationDuration
            easing.type: Easing.OutInQuad
        }
    }
}
