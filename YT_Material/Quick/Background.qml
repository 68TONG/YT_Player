pragma ComponentBehavior: Bound
import QtQuick
import YT_Material as YT_M

Item {
    id: root

    property variant initProperty: null
    property real radius: initProperty ? initProperty.radius : 0
    property real topLeftRadius: initProperty ? initProperty.topLeftRadius : 0
    property real topRightRadius: initProperty ? initProperty.topRightRadius : 0
    property real bottomLeftRadius: initProperty ? initProperty.bottomLeftRadius : 0
    property real bottomRightRadius: initProperty ? initProperty.bottomRightRadius : 0
    property real outlineWidth: initProperty ? initProperty.outlineWidth : 0
    property color outlineColor: initProperty ? initProperty.outlineColor : YT_M.Theme.OutlineVariant

    property alias background: background
    Rectangle {
        id: background
        anchors.fill: parent
        color: root.compositeColors(root.containerColor, root.containerLayerColors)
        border.color: root.outlineColor
        border.width: root.outlineWidth

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

    property color containerColor: initProperty ? initProperty.containerColor : YT_M.Theme.Transparent
    property color containerLayerColor: initProperty ? initProperty.containerLayerColor : YT_M.Theme.Transparent
    property list<color> containerLayerColors: {
        let ret = [];
        ret.push(Qt.rgba(hoveredColor.r, hoveredColor.g, hoveredColor.b, hoveredColor.a * 0.08));
        ret.push(Qt.rgba(dragingColor.r, dragingColor.g, dragingColor.b, dragingColor.a * 0.16));
        return ret;
    }
    property color hoveredColor: initProperty && initProperty.hovered ? containerLayerColor : YT_M.Theme.Transparent
    property color dragingColor: initProperty && initProperty.draging ? containerLayerColor : YT_M.Theme.Transparent

    function compositeColors(baseColor: color, overlays: list<color>): color {
        let result = baseColor;
        for (let i = 0; i < overlays.length; ++i) {
            let overlay = overlays[i];
            let a = overlay.a + result.a * (1.0 - overlay.a);
            if (a <= 0.0) {
                result = Qt.rgba(0, 0, 0, 0);
                continue;
            }

            let r = (overlay.r * overlay.a + result.r * result.a * (1.0 - overlay.a)) / a;
            let g = (overlay.g * overlay.a + result.g * result.a * (1.0 - overlay.a)) / a;
            let b = (overlay.b * overlay.a + result.b * result.a * (1.0 - overlay.a)) / a;
            result = Qt.rgba(r, g, b, a);
        }
        return result;
    }

    property int diffusionLayerCount: 0
    property bool diffusionLayerVisible: initProperty ? initProperty.down : false
    property color diffusionLayerColor: initProperty ? containerLayerColor : YT_M.Theme.Transparent
    property point diffusionLayerPoint: initProperty ? mapFromItem(initProperty, initProperty.downX, initProperty.downY) : Qt.point(0, 0)
    property variant diffusionLayerProgress: initProperty ? initProperty.autoRepeatProgress : undefined
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
    property int elevation: initProperty ? initProperty.elevation : YT_M.Theme.ElevationNone
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
