import QtQuick
import YT_Material as YT

YT.AbstractButton {
    id: root

    property bool draging: false

    styleType: Card.Filled
    enum StyleT {
        Elevated,
        Filled,
        Outlined
    }

    radius: YT.Theme.RadiusMedium
    elevation: {
        if (styleType == Card.Elevated) {
            if (draging) {
                return YT.Theme.Elevation4;
            } else if (pressed) {
                return YT.Theme.Elevation1;
            } else if (hovered) {
                return YT.Theme.Elevation2;
            } else {
                return YT.Theme.Elevation1;
            }
        } else if (styleType == Card.Filled) {
            if (draging) {
                return YT.Theme.Elevation3;
            } else if (pressed) {
                return YT.Theme.Elevation0;
            } else if (hovered) {
                return YT.Theme.Elevation1;
            } else {
                return YT.Theme.Elevation0;
            }
        } else if (styleType == Card.Outlined) {
            if (draging) {
                return YT.Theme.Elevation3;
            } else if (pressed) {
                return YT.Theme.Elevation0;
            } else if (hovered) {
                return YT.Theme.Elevation1;
            } else {
                return YT.Theme.Elevation0;
            }
        }
        return YT.Theme.Elevation0;
    }

    iconSize: 24
    iconColor: YT.Theme.Primary

    outlineWidth: styleType != Card.Outlined ? 0 : 1
    outlineColor: {
        if (enabled == false) {
            return YT.Theme.setAlphaF(YT.Theme.Outline, 0.12);
        }
        return YT.Theme.OutlineVariant;
    }

    containerColor: {
        if (styleType == Card.Elevated) {
            if (enabled == false) {
                return YT.Theme.setAlphaF(YT.Theme.Surface, 0.38);
            }
            return YT.Theme.SurfaceContainerLow;
        } else if (styleType == Card.Filled) {
            if (enabled == false) {
                return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.38);
            }
            return YT.Theme.SurfaceContainerHighest;
        } else if (styleType == Card.Outlined) {
            if (enabled == false) {
                return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.38);
            }
            return YT.Theme.Surface;
        }
        return YT.Theme.Transparent;
    }
    containerLayerColor: {
        return YT.Theme.OnSurface;
    }
}
