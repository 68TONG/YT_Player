import QtQuick
import QtQuick
import YT_Material as YT

Text {
    id: root
    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter
    color: hovered ? YT.Theme.Primary : YT.Theme.OnSurfaceVariant
    // font.underline: hovered

    property bool hovered: false
    property alias mouseArea: mouseArea

    MouseArea {
        id: mouseArea
        x: parent.leftPadding
        y: parent.topPadding
        width: parent.contentWidth
        height: parent.contentHeight

        hoverEnabled: true
        onEntered: parent.hovered = true
        onExited: parent.hovered = false
        onClicked: parent.linkActivated(parent.text)
    }
}
