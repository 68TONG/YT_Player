import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.MenuSeparator {
    id: root

    implicitWidth: parent ? parent.width : 0
    implicitHeight: implicitContentHeight + topPadding + bottomPadding

    padding: 0
    verticalPadding: 0
    horizontalPadding: YT.Theme.Space100
    contentItem: Rectangle {
        implicitHeight: 1
        color: YT.Theme.OutlineVariant
    }
}
