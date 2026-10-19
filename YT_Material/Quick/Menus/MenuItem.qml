pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT_M

T.MenuItem {
    id: root

    property int styleType: (menu instanceof YT_M.Menu) ? ((menu as YT_M.Menu).styleType) : (YT_M.Menu.Standard)

    icon.width: iconSize
    icon.height: iconSize
    icon.source: undefined
    property real iconSize: 20
    property variant iconSource

    spacing: YT_M.Theme.Space100
    verticalPadding: YT_M.Theme.Space100
    horizontalPadding: YT_M.Theme.Space150

    arrow: Loader {
        active: root.subMenu != null
        anchors.right: root.right
        anchors.rightMargin: root.rightPadding
        anchors.verticalCenter: root.verticalCenter
        sourceComponent: YT_M.Icon {
            iconSize: root.iconSize
            iconColor: root.iconColor
            iconSource: YT_M.IconProvider.KeyboardArrowRight
        }
    }

    indicator: YT_M.Icon {
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

    background: YT_M.ControlBackground {
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

    property real radius: checked ? YT_M.Theme.RadiusMedium : YT_M.Theme.RadiusExtraSmall
    property real topLeftRadius: ObjectModel.index == 0 ? YT_M.Theme.RadiusMedium : radius
    property real topRightRadius: ObjectModel.index == 0 ? YT_M.Theme.RadiusMedium : radius
    property real bottomLeftRadius: (menu && ObjectModel.index == menu.count - 1) ? YT_M.Theme.RadiusMedium : radius
    property real bottomRightRadius: (menu && ObjectModel.index == menu.count - 1) ? YT_M.Theme.RadiusMedium : radius

    property color iconColor: {
        if (styleType == YT_M.Menu.Vibrant) {
            if (enabled == false && checked) {
                return YT_M.Theme.setAlphaF(YT_M.Theme.OnTertiary, 0.38);
            } else {
                return labelColor;
            }
        } else if (styleType == YT_M.Menu.Standard) {
            if (enabled == false || checked) {
                return labelColor;
            } else {
                return YT_M.Theme.OnSurfaceVariant;
            }
        }
        return YT_M.Theme.Transparent;
    }
    property color labelColor: {
        if (styleType == YT_M.Menu.Vibrant) {
            if (enabled == false) {
                return YT_M.Theme.setAlphaF(YT_M.Theme.OnTertiaryContainer, 0.38);
            } else if (checked) {
                return YT_M.Theme.OnTertiary;
            } else {
                return YT_M.Theme.OnTertiaryContainer;
            }
        } else if (styleType == YT_M.Menu.Standard) {
            if (enabled == false && checked) {
                return YT_M.Theme.setAlphaF(YT_M.Theme.OnTertiaryContainer, 0.38);
            } else if (enabled == false) {
                return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
            } else if (checked) {
                return YT_M.Theme.OnTertiaryContainer;
            } else {
                return YT_M.Theme.OnSurface;
            }
        }
        return YT_M.Theme.Transparent;
    }
    property color containerColor: {
        if (styleType == YT_M.Menu.Vibrant) {
            if (checked) {
                return YT_M.Theme.Tertiary;
            } else {
                return YT_M.Theme.Transparent;
                return YT_M.Theme.TertiaryContainer;
            }
        } else if (styleType == YT_M.Menu.Standard) {
            if (enabled == false && checked) {
                return YT_M.Theme.setAlphaF(YT_M.Theme.TertiaryContainer, 0.38);
            } else if (enabled == false) {
                return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
            } else if (checked) {
                return YT_M.Theme.TertiaryContainer;
            } else {
                return YT_M.Theme.Transparent;
                return YT_M.Theme.SurfaceContainerLow;
            }
        }
        return YT_M.Theme.Transparent;
    }
    property color containerLayerColor: {
        return enabled ? labelColor : YT_M.Theme.Transparent;
    }
}
