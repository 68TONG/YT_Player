import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.Drawer {
    id: root

    x: (parent.width - width) / 2
    // y: (parent.height - height)
    width: Math.min(640, implicitWidth)

    edge: Qt.BottomEdge
    padding: YT.Theme.RadiusExtraLarge
    topPadding: padding
    leftPadding: padding
    rightPadding: padding
    bottomPadding: YT.Theme.RadiusNone
    background: Rectangle {
        color: YT.Theme.SurfaceContainerLow
        topLeftRadius: YT.Theme.RadiusExtraLarge
        topRightRadius: YT.Theme.RadiusExtraLarge
        Loader {
            active: root.dim == false
            z: -true
            width: parent.width
            height: parent.height + YT.Theme.RadiusExtraLarge * 2
            sourceComponent: YT.ElevationEffect {
                radius: YT.Theme.RadiusExtraLarge
                elevation: YT.Theme.Elevation1
            }
        }
    }

    T.Overlay.modal: Rectangle {
        anchors.fill: parent
        color: YT.Theme.setAlphaF(YT.Theme.Scrim, 0.38)
    }

    enter: Transition {
        NumberAnimation {
            target: root
            property: "y"
            from: root.parent.height
            to: (root.parent.height - root.height)
            duration: 2000
        }
    }
}
