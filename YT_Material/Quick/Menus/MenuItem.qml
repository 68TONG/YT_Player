pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.MenuItem {
    id: root

    property int styleType: (menu instanceof YT.Menu) ? ((menu as YT.Menu).styleType) : (YT.Menu.Standard)

    icon.width: iconSize
    icon.height: iconSize
    icon.source: undefined
    property real iconSize: 20
    property variant iconSource

    spacing: YT.Theme.Space100
    verticalPadding: YT.Theme.Space100
    horizontalPadding: YT.Theme.Space150

    arrow: Loader {
        active: root.subMenu != null
        anchors.right: root.right
        anchors.rightMargin: root.rightPadding
        anchors.verticalCenter: root.verticalCenter
        sourceComponent: YT.Icon {
            iconSize: root.iconSize
            iconColor: root.iconColor
            iconSource: YT.IconProvider.KeyboardArrowRight
        }
    }

    indicator: YT.Icon {
        anchors.left: root.left
        anchors.leftMargin: root.leftPadding
        anchors.verticalCenter: root.verticalCenter

        iconSize: root.iconSize
        iconColor: root.iconColor || root.icon.color
        iconSource: {
            if (root.iconSource != null)
                return root.iconSource;
            if (root.icon.name != "")
                return root.icon.name;
            return root.icon.source;
        }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        color: root.labelColor
        leftPadding: root.indicator && root.indicator.width > 0 ? root.indicator.width + root.spacing : 0
        rightPadding: root.arrow && root.arrow.width > 0 ? root.arrow.width + root.spacing : 0
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }

    background: YT.ControlBackground {
        radius: root.radius
        topLeftRadius: root.topLeftRadius
        topRightRadius: root.topRightRadius
        bottomLeftRadius: root.bottomLeftRadius
        bottomRightRadius: root.bottomRightRadius

        background.color: root.containerColor

        defaultLayer.color: root.containerLayerColor
        defaultLayer.visible: root.hovered || (root.subMenu && root.subMenu.visible)

        diffusionLayerColor: root.containerLayerColor
        diffusionLayerPoint: mapFromItem(root, root.pressX, root.pressY)
        diffusionLayerVisible: root.pressed
    }

    property real radius: checked ? YT.Theme.RadiusMedium : YT.Theme.RadiusExtraSmall
    property real topLeftRadius: ObjectModel.index == 0 ? YT.Theme.RadiusMedium : radius
    property real topRightRadius: ObjectModel.index == 0 ? YT.Theme.RadiusMedium : radius
    property real bottomLeftRadius: (menu && ObjectModel.index == menu.count - 1) ? YT.Theme.RadiusMedium : radius
    property real bottomRightRadius: (menu && ObjectModel.index == menu.count - 1) ? YT.Theme.RadiusMedium : radius

    property color iconColor: {
        if (styleType == YT.Menu.Vibrant) {
            if (enabled == false && checked) {
                return YT.Theme.setAlphaF(YT.Theme.OnTertiary, 0.38);
            } else {
                return labelColor;
            }
        } else if (styleType == YT.Menu.Standard) {
            if (enabled == false || checked) {
                return labelColor;
            } else {
                return YT.Theme.OnSurfaceVariant;
            }
        }
        return YT.Theme.Transparent;
    }
    property color labelColor: {
        if (styleType == YT.Menu.Vibrant) {
            if (enabled == false) {
                return YT.Theme.setAlphaF(YT.Theme.OnTertiaryContainer, 0.38);
            } else if (checked) {
                return YT.Theme.OnTertiary;
            } else {
                return YT.Theme.OnTertiaryContainer;
            }
        } else if (styleType == YT.Menu.Standard) {
            if (enabled == false && checked) {
                return YT.Theme.setAlphaF(YT.Theme.OnTertiaryContainer, 0.38);
            } else if (enabled == false) {
                return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.38);
            } else if (checked) {
                return YT.Theme.OnTertiaryContainer;
            } else {
                return YT.Theme.OnSurface;
            }
        }
        return YT.Theme.Transparent;
    }
    property color containerColor: {
        if (styleType == YT.Menu.Vibrant) {
            if (checked) {
                return YT.Theme.Tertiary;
            } else {
                return YT.Theme.Transparent;
                return YT.Theme.TertiaryContainer;
            }
        } else if (styleType == YT.Menu.Standard) {
            if (enabled == false && checked) {
                return YT.Theme.setAlphaF(YT.Theme.TertiaryContainer, 0.38);
            } else if (enabled == false) {
                return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.38);
            } else if (checked) {
                return YT.Theme.TertiaryContainer;
            } else {
                return YT.Theme.Transparent;
                return YT.Theme.SurfaceContainerLow;
            }
        }
        return YT.Theme.Transparent;
    }
    property color containerLayerColor: {
        return enabled ? labelColor : YT.Theme.Transparent;
    }
}
