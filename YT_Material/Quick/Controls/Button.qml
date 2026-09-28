pragma ComponentBehavior: Bound
import QtQuick
import YT_Material as YT

YT.AbstractButton {
    id: root

    styleType: Button.Filled
    property bool styleType_Round: true
    enum StyleT {
        Elevated,
        Filled,
        Tonal,
        Outlined,
        Text
    }

    sizeType: Button.Xsmall
    enum SizeT {
        Xsmall,
        Small,
        Medium,
        Large,
        Xlarge
    }

    elevation: {
        if (styleType != Button.Elevated || enabled == false) {
            return YT.Theme.Elevation0;
        }
        return YT.Theme.Elevation1;
    }

    font: getFont_Button()
    radius: getRadius_Button()
    spacing: getSpacing_Button()
    implicitHeight: getImplicitHeight_Button()
    verticalPadding: getVerticalPadding_Button()
    horizontalPadding: getHorizontalPadding_Button()

    outlineWidth: getOutlineWidth_Button()
    iconSize: getIconSize_Button()
    iconColor: labelColor

    labelColor: getLabelColor_Button()
    containerColor: getContainerColor_Button()
    containerLayerColor: getContainerLayerColor_Button()

    function getFont_Button() {
        if (sizeType == Button.Xsmall) {
            return YT.Theme.FontBodySmall;
        } else if (sizeType == Button.Small) {
            return YT.Theme.FontBodySmall;
        } else if (sizeType == Button.Medium) {
            return YT.Theme.FontBodyMedium;
        } else if (sizeType == Button.Large) {
            return YT.Theme.FontBodyLarge;
        } else if (sizeType == Button.Xlarge) {
            return YT.Theme.FontBodyLarge;
        }
        return undefined;
    }
    function getSpacing_Button() {
        if (sizeType == Button.Xsmall) {
            return YT.Theme.Space50;
        } else if (sizeType == Button.Small) {
            return YT.Theme.Space100;
        } else if (sizeType == Button.Medium) {
            return YT.Theme.Space100;
        } else if (sizeType == Button.Large) {
            return YT.Theme.Space150;
        } else if (sizeType == Button.Xlarge) {
            return YT.Theme.Space200;
        }
        return 0;
    }
    function getImplicitHeight_Button() {
        if (sizeType == Button.Xsmall) {
            return 32;
        } else if (sizeType == Button.Small) {
            return 40;
        } else if (sizeType == Button.Medium) {
            return 56;
        } else if (sizeType == Button.Large) {
            return 96;
        } else if (sizeType == Button.Xlarge) {
            return 136;
        }
        return 0;
    }
    function getVerticalPadding_Button() {
        return 0;
    }
    function getHorizontalPadding_Button() {
        if (sizeType == Button.Xsmall) {
            return YT.Theme.Space150;
        } else if (sizeType == Button.Small) {
            return YT.Theme.Space200;
        } else if (sizeType == Button.Medium) {
            return YT.Theme.Space300;
        } else if (sizeType == Button.Large) {
            return YT.Theme.Space600;
        } else if (sizeType == Button.Xlarge) {
            return YT.Theme.Space800;
        }
        return 0;
    }
    function getRadius_Button() {
        if (pressed == false && checked == false) {
            if (styleType_Round == true) {
                return YT.Theme.RadiusFull;
            }

            if (sizeType == Button.Xsmall) {
                return YT.Theme.RadiusMedium;
            } else if (sizeType == Button.Small) {
                return YT.Theme.RadiusMedium;
            } else if (sizeType == Button.Medium) {
                return YT.Theme.RadiusLarge;
            } else if (sizeType == Button.Large) {
                return YT.Theme.RadiusExtraLarge;
            } else if (sizeType == Button.Xlarge) {
                return YT.Theme.RadiusExtraLarge;
            }
        }
        if (sizeType == Button.Xsmall) {
            return YT.Theme.RadiusSmall;
        } else if (sizeType == Button.Small) {
            return YT.Theme.RadiusSmall;
        } else if (sizeType == Button.Medium) {
            return YT.Theme.RadiusMedium;
        } else if (sizeType == Button.Large) {
            return YT.Theme.RadiusLarge;
        } else if (sizeType == Button.Xlarge) {
            return YT.Theme.RadiusLarge;
        }
        return 0;
    }
    function getIconSize_Button() {
        if (sizeType == Button.Xsmall || sizeType == Button.Small) {
            return 20;
        } else if (sizeType == Button.Medium) {
            return 24;
        } else if (sizeType == Button.Large) {
            return 32;
        } else if (sizeType == Button.Xlarge) {
            return 40;
        }
        return 0;
    }
    function getOutlineWidth_Button() {
        if (enabled == false) {
            return 0;
        }
        if (styleType != Button.Outlined) {
            return 0;
        } else if (sizeType == Button.Large) {
            return 2;
        } else if (sizeType == Button.Xlarge) {
            return 3;
        }
        return 1;
    }
    function getLabelColor_Button() {
        if (enabled == false) {
            return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.38);
        }
        if (styleType == Button.Elevated) {
            if (checkable == false) {
                return YT.Theme.Primary;
            } else if (checked == false) {
                return YT.Theme.Primary;
            } else {
                return YT.Theme.OnPrimary;
            }
        } else if (styleType == Button.Filled) {
            if (checkable == false) {
                return YT.Theme.OnPrimary;
            } else if (checked == false) {
                return YT.Theme.OnSurfaceVariant;
            } else {
                return YT.Theme.OnPrimary;
            }
        } else if (styleType == Button.Tonal) {
            if (checkable == false) {
                return YT.Theme.OnSecondaryContainer;
            } else if (checked == false) {
                return YT.Theme.OnSecondaryContainer;
            } else {
                return YT.Theme.OnSecondary;
            }
        } else if (styleType == Button.Outlined) {
            if (checkable == false) {
                return YT.Theme.OnSurfaceVariant;
            } else if (checked == false) {
                return YT.Theme.OnSurfaceVariant;
            } else {
                return YT.Theme.InverseOnSurface;
            }
        } else if (styleType == Button.Text) {
            return YT.Theme.Primary;
        }
        return YT.Theme.Transparent;
    }
    function getContainerColor_Button() {
        if (enabled == false) {
            return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.1);
        }
        if (styleType == Button.Elevated) {
            if (checkable == false) {
                return YT.Theme.SurfaceContainerLow;
            } else if (checked == false) {
                return YT.Theme.SurfaceContainerLow;
            } else {
                return YT.Theme.Primary;
            }
        } else if (styleType == Button.Filled) {
            if (checkable == false) {
                return YT.Theme.Primary;
            } else if (checked == false) {
                return YT.Theme.SurfaceContainer;
            } else {
                return YT.Theme.Primary;
            }
        } else if (styleType == Button.Tonal) {
            if (checkable == false) {
                return YT.Theme.SecondaryContainer;
            } else if (checked == false) {
                return YT.Theme.SecondaryContainer;
            } else {
                return YT.Theme.Secondary;
            }
        } else if (styleType == Button.Outlined) {
            if (checkable == false) {
                return YT.Theme.Transparent;
            } else if (checked == false) {
                return YT.Theme.Transparent;
            } else {
                return YT.Theme.InverseSurface;
            }
        }
        return YT.Theme.Transparent;
    }
    function getContainerLayerColor_Button() {
        return enabled ? labelColor : YT.Theme.Transparent;
    }
}
