import QtQuick
import YT_Material as YT_M

YT_M.AbstractButton {
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
            return YT_M.Theme.ElevationNone;
        }
        return YT_M.Theme.Elevation1;
    }

    font: getFont_Button()
    radius: getRadius_Button()
    spacing: getSpacing_Button()
    implicitHeight: getImplicitHeight_Button()
    verticalPadding: getVerticalPadding_Button()
    horizontalPadding: getHorizontalPadding_Button()

    iconSize: getIconSize_Button()
    outlineWidth: getOutlineWidth_Button()

    labelColor: getLabelColor_Button()
    containerColor: getContainerColor_Button()
    containerLayerColor: getContainerLayerColor_Button()

    function getFont_Button() {
        if (sizeType == Button.Xsmall) {
            return YT_M.Font.LabelLarge;
        } else if (sizeType == Button.Small) {
            return YT_M.Font.LabelLarge;
        } else if (sizeType == Button.Medium) {
            return YT_M.Font.TitleMedium;
        } else if (sizeType == Button.Large) {
            return YT_M.Font.HeadlineSmall;
        } else if (sizeType == Button.Xlarge) {
            return YT_M.Font.HeadlineLarge;
        }
        return undefined;
    }
    function getSpacing_Button() {
        if (sizeType == Button.Xsmall) {
            return YT_M.Theme.Space50;
        } else if (sizeType == Button.Small) {
            return YT_M.Theme.Space100;
        } else if (sizeType == Button.Medium) {
            return YT_M.Theme.Space100;
        } else if (sizeType == Button.Large) {
            return YT_M.Theme.Space150;
        } else if (sizeType == Button.Xlarge) {
            return YT_M.Theme.Space200;
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
            return YT_M.Theme.Space150;
        } else if (sizeType == Button.Small) {
            return YT_M.Theme.Space200;
        } else if (sizeType == Button.Medium) {
            return YT_M.Theme.Space300;
        } else if (sizeType == Button.Large) {
            return YT_M.Theme.Space600;
        } else if (sizeType == Button.Xlarge) {
            return YT_M.Theme.Space800;
        }
        return 0;
    }
    function getRadius_Button() {
        if (pressed == false && checked == false) {
            if (styleType_Round == true) {
                return YT_M.Theme.RadiusFull;
            }

            if (sizeType == Button.Xsmall) {
                return YT_M.Theme.RadiusMedium;
            } else if (sizeType == Button.Small) {
                return YT_M.Theme.RadiusMedium;
            } else if (sizeType == Button.Medium) {
                return YT_M.Theme.RadiusLarge;
            } else if (sizeType == Button.Large) {
                return YT_M.Theme.RadiusExtraLarge;
            } else if (sizeType == Button.Xlarge) {
                return YT_M.Theme.RadiusExtraLarge;
            }
        }
        if (sizeType == Button.Xsmall) {
            return YT_M.Theme.RadiusSmall;
        } else if (sizeType == Button.Small) {
            return YT_M.Theme.RadiusSmall;
        } else if (sizeType == Button.Medium) {
            return YT_M.Theme.RadiusMedium;
        } else if (sizeType == Button.Large) {
            return YT_M.Theme.RadiusLarge;
        } else if (sizeType == Button.Xlarge) {
            return YT_M.Theme.RadiusLarge;
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
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
        }
        if (styleType == Button.Elevated) {
            if (checkable == false) {
                return YT_M.Theme.Primary;
            } else if (checked == false) {
                return YT_M.Theme.Primary;
            } else {
                return YT_M.Theme.OnPrimary;
            }
        } else if (styleType == Button.Filled) {
            if (checkable == false) {
                return YT_M.Theme.OnPrimary;
            } else if (checked == false) {
                return YT_M.Theme.OnSurfaceVariant;
            } else {
                return YT_M.Theme.OnPrimary;
            }
        } else if (styleType == Button.Tonal) {
            if (checkable == false) {
                return YT_M.Theme.OnSecondaryContainer;
            } else if (checked == false) {
                return YT_M.Theme.OnSecondaryContainer;
            } else {
                return YT_M.Theme.OnSecondary;
            }
        } else if (styleType == Button.Outlined) {
            if (checkable == false) {
                return YT_M.Theme.OnSurfaceVariant;
            } else if (checked == false) {
                return YT_M.Theme.OnSurfaceVariant;
            } else {
                return YT_M.Theme.InverseOnSurface;
            }
        } else if (styleType == Button.Text) {
            return YT_M.Theme.Primary;
        }
        return YT_M.Theme.Transparent;
    }
    function getContainerColor_Button() {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.1);
        }
        if (styleType == Button.Elevated) {
            if (checkable == false) {
                return YT_M.Theme.SurfaceContainerLow;
            } else if (checked == false) {
                return YT_M.Theme.SurfaceContainerLow;
            } else {
                return YT_M.Theme.Primary;
            }
        } else if (styleType == Button.Filled) {
            if (checkable == false) {
                return YT_M.Theme.Primary;
            } else if (checked == false) {
                return YT_M.Theme.SurfaceContainer;
            } else {
                return YT_M.Theme.Primary;
            }
        } else if (styleType == Button.Tonal) {
            if (checkable == false) {
                return YT_M.Theme.SecondaryContainer;
            } else if (checked == false) {
                return YT_M.Theme.SecondaryContainer;
            } else {
                return YT_M.Theme.Secondary;
            }
        } else if (styleType == Button.Outlined) {
            if (checkable == false) {
                return YT_M.Theme.Transparent;
            } else if (checked == false) {
                return YT_M.Theme.Transparent;
            } else {
                return YT_M.Theme.InverseSurface;
            }
        }
        return YT_M.Theme.Transparent;
    }
    function getContainerLayerColor_Button() {
        return enabled ? labelColor : YT_M.Theme.Transparent;
    }
}
