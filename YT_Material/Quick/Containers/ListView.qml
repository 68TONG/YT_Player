import QtQuick
import YT_Material as YT

ListView {
    id: rootView

    spacing: YT.Theme.Space25
    highlightFollowsCurrentItem: false
    displaced: Transition {
        NumberAnimation {
            properties: "x,y"
            duration: 300
            easing.type: Easing.OutCubic
        }
    }
    move: Transition {
        NumberAnimation {
            properties: "x,y"
            duration: 300
            easing.type: Easing.OutCubic
        }
    }
}
