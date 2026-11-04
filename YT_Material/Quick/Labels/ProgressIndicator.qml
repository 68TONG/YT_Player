import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT_M

T.Slider {
    id: root

    handle: null
    background: Item {
        Rectangle {
            x: 0
            y: 0
            width: root.lPosition
            height: parent.height
            color: root.activeContainerColor
            radius: YT_M.Theme.RadiusFull
        }
        Rectangle {
            x: root.rPosition
            y:  0
            width: parent.width - root.rPosition
            height: parent.height
            color: root.inactiveContainerColor
            radius: YT_M.Theme.RadiusFull
        }
        Rectangle {
            anchors.right: parent.right
            anchors.rightMargin: root.implicitSize == 4 ? 0 : 2
            anchors.verticalCenter: parent.verticalCenter
            width: root.indicatorSize
            height: root.indicatorSize
            color: root.activeContainerColor
            radius: YT_M.Theme.RadiusFull
        }
    }

    spacing: 4
    implicitWidth: 240
    implicitHeight: implicitSize
    property real implicitSize: 4
    property real indicatorSize: 4

    property real mPosition: position * (width - implicitSize) + implicitSize
    property real lPosition: mPosition
    property real rPosition: mPosition + spacing

    property color activeContainerColor: YT_M.Theme.Primary
    property color inactiveContainerColor: YT_M.Theme.SecondaryContainer
}
