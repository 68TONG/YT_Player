pragma ComponentBehavior: Bound
import QtQuick
import YT_Material as YT_M

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
        border.color: YT_M.Theme.OutlineVariant
        border.width: 0

        readonly property real radiusFull: Math.min(width, height) / 2
        radius: root.radius == YT_M.Theme.RadiusFull ? radiusFull : root.radius
        topLeftRadius: root.topLeftRadius == YT_M.Theme.RadiusFull ? radiusFull : root.topLeftRadius
        topRightRadius: root.topRightRadius == YT_M.Theme.RadiusFull ? radiusFull : root.topRightRadius
        bottomLeftRadius: root.bottomLeftRadius == YT_M.Theme.RadiusFull ? radiusFull : root.bottomLeftRadius
        bottomRightRadius: root.bottomRightRadius == YT_M.Theme.RadiusFull ? radiusFull : root.bottomRightRadius
        Behavior on radius {
            NumberAnimation {
                easing: YT_M.Easing.Standard
                duration: YT_M.Easing.Short4
            }
        }
        Behavior on topLeftRadius {
            NumberAnimation {
                easing: YT_M.Easing.Standard
                duration: YT_M.Easing.Short4
            }
        }
        Behavior on topRightRadius {
            NumberAnimation {
                easing: YT_M.Easing.Standard
                duration: YT_M.Easing.Short4
            }
        }
        Behavior on bottomLeftRadius {
            NumberAnimation {
                easing: YT_M.Easing.Standard
                duration: YT_M.Easing.Short4
            }
        }
        Behavior on bottomRightRadius {
            NumberAnimation {
                easing: YT_M.Easing.Standard
                duration: YT_M.Easing.Short4
            }
        }
        Behavior on opacity {
            NumberAnimation {
                easing: YT_M.Easing.Standard
                duration: YT_M.Easing.Short4
            }
        }
        Behavior on color {
            ColorAnimation {
                easing: YT_M.Easing.Standard
                duration: YT_M.Easing.Short4
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

    property alias dragingLayer: dragingLayer
    Rectangle {
        id: dragingLayer
        anchors.fill: parent
        topLeftRadius: parent.topLeftRadius
        topRightRadius: parent.topRightRadius
        bottomLeftRadius: parent.bottomLeftRadius
        bottomRightRadius: parent.bottomRightRadius
        visible: false
        opacity: 0.16
    }

    property int diffusionLayerCount: 0
    property bool diffusionLayerVisible: false
    property color diffusionLayerColor: YT_M.Theme.Transparent
    property point diffusionLayerPoint: Qt.point(0, 0)
    property variant diffusionLayerProgress: undefined
    onDiffusionLayerVisibleChanged: {
        if (diffusionLayerVisible == false) {
            return;
        }
        if (diffusionLayerCount >= 2) {
            return;
        }

        // === 最简计算：取对角 ===
        const _point = diffusionLayerPoint;
        let farX = _point.x > width / 2 ? 0 : width;
        let farY = _point.y > height / 2 ? 0 : height;
        let dx = _point.x - farX;
        let dy = _point.y - farY;
        let size = 2 * Math.sqrt(dx * dx + dy * dy);
        diffusionLayer_Com.createObject(this, {
            targetSize: size
        });
    }
    Component {
        id: diffusionLayer_Com
        YT_M.CircularMask {
            id: diffusionLayer
            anchors.fill: parent
            topLeftRadius: background.topLeftRadius
            topRightRadius: background.topRightRadius
            bottomLeftRadius: background.bottomLeftRadius
            bottomRightRadius: background.bottomRightRadius

            required property real targetSize
            readonly property real currentSize: targetSize * progress
            property real progress: root.diffusionLayerProgress ?? 0.0
            opacity: (progress != 1.0 || root.diffusionLayerVisible) ? 0.10 : 0.0

            Component.onCompleted: {
                if (root.diffusionLayerProgress == undefined)
                    progress = 1.0;
                root.diffusionLayerCount = root.diffusionLayerCount + 1;
            }
            Component.onDestruction: {
                root.diffusionLayerCount = root.diffusionLayerCount - 1;
            }
            Behavior on progress {
                NumberAnimation {
                    easing: YT_M.Easing.SpringFast
                    duration: YT_M.Easing.Medium3
                }
            }
            Behavior on opacity {
                NumberAnimation {
                    easing: YT_M.Easing.Standard
                    duration: YT_M.Easing.Short4
                    onRunningChanged: {
                        if (diffusionLayer.opacity == 0.0)
                            diffusionLayer.destroy();
                    }
                }
            }
            Connections {
                target: root
                function onDiffusionLayerVisibleChanged() {
                    if (root.diffusionLayerVisible == false && root.diffusionLayerProgress != undefined)
                        diffusionLayer.progress = 1.0;
                }
            }

            targetItem: Item {
                anchors.fill: parent
                visible: false
                Rectangle {
                    x: root.diffusionLayerPoint.x - width / 2
                    y: root.diffusionLayerPoint.y - height / 2
                    width: diffusionLayer.currentSize
                    height: diffusionLayer.currentSize
                    color: root.diffusionLayerColor
                    radius: YT_M.Theme.RadiusFull
                }
            }
        }
    }

    property alias shadowLayer: shadowLayer
    property int elevation: YT_M.Theme.ElevationNone
    Loader {
        id: shadowLayer
        z: -true
        anchors.fill: parent

        active: root.elevation != YT_M.Theme.ElevationNone
        sourceComponent: YT_M.ElevationEffect {
            radius: root.radius
            elevation: root.elevation
        }
    }
}
