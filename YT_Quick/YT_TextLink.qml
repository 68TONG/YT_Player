import QtQuick

Text {
    id: root
    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter
    color: hovered ? YT_Info.FontFocusColor : YT_Info.FontColor
    font.underline: hovered

    property bool hovered: false
    MouseArea {
        hoverEnabled: true
        x: parent.leftPadding
        y: parent.topPadding
        width: parent.contentWidth
        height: parent.contentHeight

        onEntered: parent.hovered = true
        onExited: parent.hovered = false
        onClicked: parent.linkActivated(parent.text)
    }
}
