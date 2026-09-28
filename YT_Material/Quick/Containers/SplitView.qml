pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import YT_Material as YT

SplitView {
    id: root

    spacing: YT.Theme.Space75
    property color color: YT.Theme.Primary
    property color activeColor: YT.Theme.Primary

    property real handleSize: 2
    property real handleScale: 0.3
    handle: Item {
        opacity: SplitHandle.hovered
        implicitWidth: root.orientation == Qt.Horizontal ? root.spacing : parent.width 
        implicitHeight: root.orientation == Qt.Vertical ? root.spacing : parent.height 
        Rectangle {
            anchors.centerIn: parent
            implicitWidth: root.orientation == Qt.Horizontal ? root.handleSize : parent.width * root.handleScale
            implicitHeight: root.orientation == Qt.Vertical ? root.handleSize : parent.height * root.handleScale
            radius: YT.Theme.RadiusFull
            color: parent.SplitHandle.pressed ? root.activeColor : root.color
        }
    }
}
