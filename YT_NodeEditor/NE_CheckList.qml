import QtQuick

import YT_Player

NE_Item {
    id: root

    implicitWidth: parent ? parent.width : 0
    implicitHeight: _NE.implicitHeight

    readonly property alias _NE: contentItem
    YT_CheckList {
        id: contentItem
        anchors.fill: parent

        background: YT_Rectangle {
            color: (parent.hovered || parent.down || parent.popup.visible) ? YT_Info.ItemFocusColor : YT_Info.ItemColor
            radius: YT_Info.RadiusSmall
        }
    }
}
