import QtQuick
import QtQuick.Controls.Basic

import YT_Player

SplitView {
    id: root

    spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
    handle: Item {
        implicitWidth: root.spacing
        Rectangle {
            anchors.centerIn: parent
            width: parent.width - 4
            height: parent.height / 4
            radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
            color: (parent.SplitHandle.hovered || parent.SplitHandle.pressed)
                   ? YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)
                   : null
        }
    }
}
