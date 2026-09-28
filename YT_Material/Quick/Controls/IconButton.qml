import QtQuick
import YT_Material as YT

YT.Button {
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
    contentItem: YT.Icon {
        iconSize: root.iconSize
        iconSource: root.iconSource
        iconColor: root.iconColor || root.labelColor

        // Rectangle {
        //     anchors.fill: parent
        //     color: 'red'
        // }
    }

    verticalPadding: {
        return (height - iconSize) / 2;
    }
    horizontalPadding: {
        if (widthType == IconButton.Default) {
            if (sizeType == IconButton.Xsmall) {
                return YT.Theme.Space75;
            } else if (sizeType == IconButton.Small) {
                return YT.Theme.Space100;
            } else if (sizeType == IconButton.Medium) {
                return YT.Theme.Space200;
            } else if (sizeType == IconButton.Large) {
                return YT.Theme.Space400;
            } else if (sizeType == IconButton.Xlarge) {
                return YT.Theme.Space600;
            }
        } else if (widthType == IconButton.Narrow) {
            if (sizeType == IconButton.Xsmall) {
                return YT.Theme.Space50;
            } else if (sizeType == IconButton.Small) {
                return YT.Theme.Space50;
            } else if (sizeType == IconButton.Medium) {
                return YT.Theme.Space150;
            } else if (sizeType == IconButton.Large) {
                return YT.Theme.Space200;
            } else if (sizeType == IconButton.Xlarge) {
                return YT.Theme.Space400;
            }
        } else if (widthType == IconButton.Wide) {
            if (sizeType == IconButton.Xsmall) {
                return YT.Theme.Space125;
            } else if (sizeType == IconButton.Small) {
                return YT.Theme.Space175;
            } else if (sizeType == IconButton.Medium) {
                return YT.Theme.Space300;
            } else if (sizeType == IconButton.Large) {
                return YT.Theme.Space600;
            } else if (sizeType == IconButton.Xlarge) {
                return YT.Theme.Space900;
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
            return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.38);
        }

        if (styleType == IconButton.Standard) {
            if (checkable == false) {
                return YT.Theme.OnSurfaceVariant;
            } else if (checked == false) {
                return YT.Theme.OnSurfaceVariant;
            } else {
                return YT.Theme.Primary;
            }
        }

        return getLabelColor_Button();
    }
}
