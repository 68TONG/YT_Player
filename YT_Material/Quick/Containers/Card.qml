import QtQuick
import YT_Material as YT_M

YT_M.AbstractButton {
    id: root

    property bool draging: false

    styleType: Card.Filled
    enum StyleT {
        Elevated,
        Filled,
        Outlined
    }

    radius: YT_M.Theme.RadiusMedium
    elevation: {
        if (styleType == Card.Elevated) {
            if (draging) {
                return YT_M.Theme.Elevation4;
            } else if (pressed) {
                return YT_M.Theme.Elevation1;
            } else if (hovered) {
                return YT_M.Theme.Elevation2;
            } else {
                return YT_M.Theme.Elevation1;
            }
        } else if (styleType == Card.Filled) {
            if (draging) {
                return YT_M.Theme.Elevation3;
            } else if (pressed) {
                return YT_M.Theme.Elevation0;
            } else if (hovered) {
                return YT_M.Theme.Elevation1;
            } else {
                return YT_M.Theme.Elevation0;
            }
        } else if (styleType == Card.Outlined) {
            if (draging) {
                return YT_M.Theme.Elevation3;
            } else if (pressed) {
                return YT_M.Theme.Elevation0;
            } else if (hovered) {
                return YT_M.Theme.Elevation1;
            } else {
                return YT_M.Theme.Elevation0;
            }
        }
        return YT_M.Theme.Elevation0;
    }

    iconSize: 24
    iconColor: YT_M.Theme.Primary

    outlineWidth: styleType != Card.Outlined ? 0 : 1
    outlineColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.Outline, 0.12);
        }
        return YT_M.Theme.OutlineVariant;
    }

    containerColor: {
        if (styleType == Card.Elevated) {
            if (enabled == false) {
                return YT_M.Theme.setAlphaF(YT_M.Theme.Surface, 0.38);
            }
            return YT_M.Theme.SurfaceContainerLow;
        } else if (styleType == Card.Filled) {
            if (enabled == false) {
                return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
            }
            return YT_M.Theme.SurfaceContainerHighest;
        } else if (styleType == Card.Outlined) {
            if (enabled == false) {
                return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
            }
            return YT_M.Theme.Surface;
        }
        return YT_M.Theme.Transparent;
    }
    containerLayerColor: {
        return YT_M.Theme.OnSurface;
    }
}
