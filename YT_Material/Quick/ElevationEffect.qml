import QtQuick
import QtQuick.Effects
import YT_Material as YT

Item {
    id: root

    z: -true
    visible: elevation != YT.Theme.Elevation0

    property real radius: 0
    property int elevation: YT.Theme.Elevation0

    RectangularShadow {
        anchors.fill: root
        radius: root.radius
        offset.x: shadowConfig.offsetX
        offset.y: shadowConfig.offsetY
        blur: 1.2 * shadowConfig.blur
        spread: shadowConfig.spread
        opacity: shadowConfig.opacity
        color: YT.Theme.Shadow

        readonly property variant shadowConfig: {
            switch (root.elevation) {
            case 1:
                return {
                    blur: 2,
                    spread: 0,
                    opacity: 0.30,
                    offsetY: 1,
                    offsetX: 0
                };
            case 3:
                return {
                    blur: 2,
                    spread: 0,
                    opacity: 0.30,
                    offsetY: 1,
                    offsetX: 0
                };
            case 6:
                return {
                    blur: 3,
                    spread: 0,
                    opacity: 0.30,
                    offsetY: 1,
                    offsetX: 0
                };
            case 8:
                return {
                    blur: 3,
                    spread: 0,
                    opacity: 0.30,
                    offsetY: 2,
                    offsetX: 0
                };
            case 12:
                return {
                    blur: 4,
                    spread: 0,
                    opacity: 0.30,
                    offsetY: 4,
                    offsetX: 0
                };
            default:
                return {
                    blur: 0.00,
                    spread: 0.00,
                    opacity: 0.00,
                    offsetY: 0,
                    offsetX: 0
                };
            }
        }
    }

    RectangularShadow {
        anchors.fill: root
        radius: root.radius
        offset.x: shadowConfig.offsetX
        offset.y: shadowConfig.offsetY
        blur: 1.2 * shadowConfig.blur
        spread: shadowConfig.spread
        opacity: shadowConfig.opacity
        color: YT.Theme.Shadow

        readonly property variant shadowConfig: {
            switch (root.elevation) {
            case 1:
                return {
                    blur: 3,
                    spread: 1,
                    opacity: 0.15,
                    offsetY: 1,
                    offsetX: 0
                };
            case 3:
                return {
                    blur: 6,
                    spread: 2,
                    opacity: 0.15,
                    offsetY: 2,
                    offsetX: 0
                };
            case 6:
                return {
                    blur: 8,
                    spread: 3,
                    opacity: 0.15,
                    offsetY: 4,
                    offsetX: 0
                };
            case 8:
                return {
                    blur: 10,
                    spread: 4,
                    opacity: 0.15,
                    offsetY: 6,
                    offsetX: 0
                };
            case 12:
                return {
                    blur: 12,
                    spread: 6,
                    opacity: 0.15,
                    offsetY: 8,
                    offsetX: 0
                };
            default:
                return {
                    blur: 0.00,
                    spread: 0.00,
                    opacity: 0.00,
                    offsetY: 0,
                    offsetX: 0
                };
            }
        }
    }
}
