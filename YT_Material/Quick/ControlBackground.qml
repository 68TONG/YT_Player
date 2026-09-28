pragma ComponentBehavior: Bound
import QtQuick
import YT_Material as YT

Item {
    id: root

    property real radius
    property real topLeftRadius: radius
    property real topRightRadius: radius
    property real bottomLeftRadius: radius
    property real bottomRightRadius: radius

    property alias background: background
    Rectangle {
        id: background
        anchors.fill: parent
        border.color: YT.Theme.OutlineVariant
        border.width: 0

        readonly property real radiusFull: height / 2
        radius: root.radius == YT.Theme.RadiusFull ? radiusFull : root.radius
        topLeftRadius: root.topLeftRadius == YT.Theme.RadiusFull ? radiusFull : root.topLeftRadius
        topRightRadius: root.topRightRadius == YT.Theme.RadiusFull ? radiusFull : root.topRightRadius
        bottomLeftRadius: root.bottomLeftRadius == YT.Theme.RadiusFull ? radiusFull : root.bottomLeftRadius
        bottomRightRadius: root.bottomRightRadius == YT.Theme.RadiusFull ? radiusFull : root.bottomRightRadius
        Behavior on radius {
            SpringAnimation {
                spring: 20
                damping: 0.9
            }
        }
        Behavior on topLeftRadius {
            SpringAnimation {
                spring: 20
                damping: 0.9
            }
        }
        Behavior on topRightRadius {
            SpringAnimation {
                spring: 20
                damping: 0.9
            }
        }
        Behavior on bottomLeftRadius {
            SpringAnimation {
                spring: 20
                damping: 0.9
            }
        }
        Behavior on bottomRightRadius {
            SpringAnimation {
                spring: 20
                damping: 0.9
            }
        }
        Behavior on color {
            ColorAnimation {
                easing.type: Easing.OutQuad
                duration: 180
            }
        }
        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: 180
            }
        }
    }

    property alias defaultLayer: defaultLayer
    Rectangle {
        id: defaultLayer
        anchors.fill: parent
        topLeftRadius: parent.topLeftRadius
        topRightRadius: parent.topRightRadius
        bottomLeftRadius: parent.bottomLeftRadius
        bottomRightRadius: parent.bottomRightRadius
        visible: false
        opacity: 0.08
    }

    property alias diffusionLayer: diffusionLayer
    property color diffusionLayerColor
    property real diffusionLayerSize: 0
    property point diffusionLayerPoint: Qt.point(0, 0)
    property bool diffusionLayerVisible: false
    Behavior on diffusionLayerSize {
        NumberAnimation {
            duration: 120
            easing.type: Easing.OutQuad
        }
    }
    onDiffusionLayerVisibleChanged: {
        if (diffusionLayerVisible == false) {
            diffusionLayerSize = 0;
            return;
        }

        // === 最简计算：取对角 ===
        const _point = diffusionLayerPoint;
        let farX = _point.x > width / 2 ? 0 : width;
        let farY = _point.y > height / 2 ? 0 : height;
        let dx = _point.x - farX;
        let dy = _point.y - farY;
        diffusionLayerSize = 2 * Math.sqrt(dx * dx + dy * dy);
    }

    YT.CircularMask {
        id: diffusionLayer
        anchors.fill: parent
        topLeftRadius: parent.topLeftRadius
        topRightRadius: parent.topRightRadius
        bottomLeftRadius: parent.bottomLeftRadius
        bottomRightRadius: parent.bottomRightRadius

        targetItem: Item {
            anchors.fill: parent
            visible: false
            Rectangle {
                x: root.diffusionLayerPoint.x - width / 2
                y: root.diffusionLayerPoint.y - height / 2
                width: root.diffusionLayerSize
                height: root.diffusionLayerSize
                color: root.diffusionLayerColor
                radius: YT.Theme.RadiusFull
                opacity: 0.10
            }
        }
    }

    property alias shadowLayer: shadowLayer
    property int elevation: YT.Theme.Elevation0
    Loader {
        id: shadowLayer
        z: -true
        anchors.fill: parent

        active: root.elevation != YT.Theme.Elevation0
        sourceComponent: YT.ElevationEffect {
            radius: root.radius
            elevation: root.elevation
        }
    }
}
