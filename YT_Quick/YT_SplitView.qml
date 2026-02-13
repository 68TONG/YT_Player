import QtQuick
import QtQuick.Controls
import YT_Player

SplitView {
    id: root

    spacing: YT_Info.Spacing
    handle: Item {
        implicitWidth: root.spacing
        YT_Rectangle {
            anchors.centerIn: parent
            width: parent.width - 4
            height: parent.height / 4
            radius: YT_Info.Radius
            color: (parent.SplitHandle.hovered || parent.SplitHandle.pressed)
                   ? YT_Info.ItemFocusColor
                   : null
        }
    }
}
