import QtQuick
import QtQuick.Controls
import YT_Player

Popup {
    id: root
    x: 0
    y: parent ? parent.height + YT_Info.Spacing : 0
    width: parent ? parent.width : 0
    height: implicitHeight
    padding: YT_Info.Margin

    contentItem: null
    background: YT_Rectangle {
        color: YT_Info.BackgroundColor
        border.color: YT_Info.ItemFocusColor
    }

    enter: Transition {
        NumberAnimation {
            property: "height"
            duration: 300
            from: 0.0
            to: height
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            property: "opacity"
            duration: 300
            from: 0.0
            to: 1.0
            easing.type: Easing.OutQuad
        }
    }
    exit: Transition {
        NumberAnimation {
            property: "opacity"
            duration: 300
            from: 1.0
            to: 0.0
            easing.type: Easing.OutQuad
        }
    }
}
