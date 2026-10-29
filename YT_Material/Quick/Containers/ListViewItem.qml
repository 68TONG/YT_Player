import QtQuick
import YT_Material as YT_M

YT_M.AbstractButton {
    id: root

    checked: isCurrentItem
    property bool isCurrentItem: ListView.isCurrentItem
    property bool draging
    property bool droping
    property bool expandingV
    property bool expandingH

    elevation: draging ? YT_M.Theme.Elevation5 : YT_M.Theme.ElevationNone
    spacing: YT_M.Theme.Space150
    verticalPadding: YT_M.Theme.Space125
    horizontalPadding: YT_M.Theme.Space200

    radius: {
        if (enabled == false) {
            return YT_M.Theme.RadiusExtraSmall;
        }
        if (checked) {
            return YT_M.Theme.RadiusLarge;
        } else if (pressed) {
            return YT_M.Theme.RadiusLarge;
        } else if (hovered) {
            return YT_M.Theme.RadiusMedium;
        }
        return 0;
    }
    iconColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
        }
        if (checked) {
            return YT_M.Theme.OnSurface;
        }
        return YT_M.Theme.OnSurfaceVariant;
    }
    labelColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
        }
        if (checked) {
            return YT_M.Theme.OnSecondaryContainer;
        }
        return YT_M.Theme.OnSurface;
    }
    containerColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.10);
        }
        if (checked) {
            return YT_M.Theme.SecondaryContainer;
        }
        return YT_M.Theme.Surface;
    }
    containerLayerColor: {
        return enabled ? labelColor : YT_M.Theme.Transparent;
    }
}
