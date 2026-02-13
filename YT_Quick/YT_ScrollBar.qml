import QtQuick
import QtQuick.Controls
import YT_Player

ScrollBar {
    id: root
    active: true

    contentItem: Rectangle {
        implicitWidth: 2
        implicitHeight: 100

        radius: width / 2
        color: YT_Info.ItemFocusColor
        opacity: root.policy === ScrollBar.AlwaysOn || (root.active && root.size < 1.0) ? 0.75 : 0

        Behavior on opacity {
            NumberAnimation {}
        }
    }
}
