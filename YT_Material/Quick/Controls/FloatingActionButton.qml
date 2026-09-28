import QtQuick
import YT_Material as YT

YT.Button {
    id: root

    enum StyleT {
        Primary,
        Secondary,
        Tertiary,
        PrimaryContainer,
        SecondaryContainer,
        TertiaryContainer
    }

    enum SizeT {
        Default,
        Medium,
        Large
    }

    indicator: null
    contentItem: YT.Icon {
        iconSize: root.iconSize
        iconSource: root.iconSource
        iconColor: root.labelColor
    }

    sizeType: FloatingActionButton.Default
    styleType: FloatingActionButton.Primary
    elevation: YT.Theme.Elevation3

    font: undefined
    spacing: 0
    implicitWidth: implicitHeight
    implicitHeight: {
        if (sizeType == FloatingActionButton.Default) {
            return 56;
        } else if (sizeType == FloatingActionButton.Medium) {
            return 80;
        } else if (sizeType == FloatingActionButton.Large) {
            return 96;
        }
        return 0;
    }
    verticalPadding: 0
    horizontalPadding: 0

    radius: {
        if (sizeType == FloatingActionButton.Default) {
            return YT.Theme.RadiusLarge;
        } else if (sizeType == FloatingActionButton.Medium) {
            return YT.Theme.RadiusLargeIncreased;
        } else if (sizeType == FloatingActionButton.Large) {
            return YT.Theme.RadiusExtraLarge;
        }
        return 0;
    }

    iconSize: {
        if (sizeType == FloatingActionButton.Default) {
            return 24;
        } else if (sizeType == FloatingActionButton.Medium) {
            return 28;
        } else if (sizeType == FloatingActionButton.Large) {
            return 36;
        }
        return 0;
    }

    outlineWidth: {
        return 0;
    }

    labelColor: {
        if (styleType == FloatingActionButton.Primary) {
            return YT.Theme.OnPrimary;
        } else if (styleType == FloatingActionButton.Secondary) {
            return YT.Theme.OnSecondary;
        } else if (styleType == FloatingActionButton.Tertiary) {
            return YT.Theme.OnTertiary;
        } else if (styleType == FloatingActionButton.PrimaryContainer) {
            return YT.Theme.OnPrimaryContainer;
        } else if (styleType == FloatingActionButton.SecondaryContainer) {
            return YT.Theme.OnSecondaryContainer;
        } else if (styleType == FloatingActionButton.TertiaryContainer) {
            return YT.Theme.OnTertiaryContainer;
        }
        return YT.Theme.Transparent;
    }
    containerColor: {
        if (styleType == FloatingActionButton.Primary) {
            return YT.Theme.Primary;
        } else if (styleType == FloatingActionButton.Secondary) {
            return YT.Theme.Secondary;
        } else if (styleType == FloatingActionButton.Tertiary) {
            return YT.Theme.Tertiary;
        } else if (styleType == FloatingActionButton.PrimaryContainer) {
            return YT.Theme.PrimaryContainer;
        } else if (styleType == FloatingActionButton.SecondaryContainer) {
            return YT.Theme.SecondaryContainer;
        } else if (styleType == FloatingActionButton.TertiaryContainer) {
            return YT.Theme.TertiaryContainer;
        }
        return YT.Theme.Transparent;
    }
}
