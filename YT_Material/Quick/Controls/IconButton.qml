import QtQuick
import YT_Material as YT_M

YT_M.Button {
    id: root

    enum StyleT {
        Filled = 1, // Button.Filled
        Tonal = 2, // Button.Tonal
        Outlined = 3, // Button.Outlined
        Standard = 5
    }
    enum WidthT {
        Default,
        Narrow,
        Wide
    }

    sizeType: IconButton.Xsmall
    styleType: IconButton.Standard
    property int widthType: IconButton.Default

    indicator: null
    contentItem: YT_M.Icon {
        fontFill: root.iconFill
        iconSize: root.iconSize
        iconColor: root.iconColor
        iconSource: root.iconSource
    }

    verticalPadding: {
        return (height - iconSize) / 2;
    }
    horizontalPadding: {
        if (widthType == IconButton.Default) {
            if (sizeType == IconButton.Xsmall) {
                return YT_M.Theme.Space75;
            } else if (sizeType == IconButton.Small) {
                return YT_M.Theme.Space100;
            } else if (sizeType == IconButton.Medium) {
                return YT_M.Theme.Space200;
            } else if (sizeType == IconButton.Large) {
                return YT_M.Theme.Space400;
            } else if (sizeType == IconButton.Xlarge) {
                return YT_M.Theme.Space600;
            }
        } else if (widthType == IconButton.Narrow) {
            if (sizeType == IconButton.Xsmall) {
                return YT_M.Theme.Space50;
            } else if (sizeType == IconButton.Small) {
                return YT_M.Theme.Space50;
            } else if (sizeType == IconButton.Medium) {
                return YT_M.Theme.Space150;
            } else if (sizeType == IconButton.Large) {
                return YT_M.Theme.Space200;
            } else if (sizeType == IconButton.Xlarge) {
                return YT_M.Theme.Space400;
            }
        } else if (widthType == IconButton.Wide) {
            if (sizeType == IconButton.Xsmall) {
                return YT_M.Theme.Space125;
            } else if (sizeType == IconButton.Small) {
                return YT_M.Theme.Space175;
            } else if (sizeType == IconButton.Medium) {
                return YT_M.Theme.Space300;
            } else if (sizeType == IconButton.Large) {
                return YT_M.Theme.Space600;
            } else if (sizeType == IconButton.Xlarge) {
                return YT_M.Theme.Space900;
            }
        }
        return 0;
    }
    iconSize: {
        if (sizeType == IconButton.Small) {
            return 24;
        }
        return getIconSize_Button();
    }
    labelColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
        }

        if (styleType == IconButton.Standard) {
            if (checkable == false) {
                return YT_M.Theme.OnSurfaceVariant;
            } else if (checked == false) {
                return YT_M.Theme.OnSurfaceVariant;
            } else {
                return YT_M.Theme.Primary;
            }
        }

        return getLabelColor_Button();
    }
}
