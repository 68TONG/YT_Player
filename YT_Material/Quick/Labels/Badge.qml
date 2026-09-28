import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.Control {
    id: root

    readonly property real implicitSize: {
        if (dot) {
            return 6;
        } else if (count < 10) {
            return 16;
        }
        return implicitContentWidth + topPadding + bottomPadding;
    }
    padding: 4
    implicitWidth: implicitSize
    implicitHeight: Math.min(16, implicitSize)

    property bool dot: false
    property int count: 0
    property int maxCount: 99

    contentItem: Text {
        text: root.count
        font: root.font
        color: root.labelColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        lineHeight: 16
        lineHeightMode: Text.FixedHeight
    }

    background: Rectangle {
        radius: root.radius
        color: root.containerColor
    }

    font.pixelSize: 11
    property real radius: YT.Theme.RadiusFull
    property color labelColor: YT.Theme.OnError
    property color containerColor: YT.Theme.Error
}
