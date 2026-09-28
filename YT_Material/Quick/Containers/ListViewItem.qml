pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import YT_Material as YT

YT.AbstractButton {
    id: root

    indicator: null
    contentItem: GridLayout {
        rowSpacing: YT.Theme.Space0
        columnSpacing: root.spacing 
        Text {
            text: root.text
            font: root.font
            color: root.labelColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            Layout.column: 1
            Layout.fillWidth: true
        }
    }

    checked: isCurrentItem
    property bool isCurrentItem: ListView.isCurrentItem
    property bool draging
    property bool droping
    property bool expandingV
    property bool expandingH

    property Item leadingItem: null
    onLeadingItemChanged: {
        if (leadingItem && leadingItem.parent == null && root.contentItem instanceof GridLayout) {
            leadingItem.parent = root.contentItem;
            leadingItem.Layout.column = 0;
        }
    }
    property Item trailingItem: null
    onTrailingItemChanged: {
        if (trailingItem && trailingItem.parent == null && root.contentItem instanceof GridLayout) {
            leadingItem.parent = root.contentItem;
            leadingItem.Layout.column = 2;
        }
    }

    elevation: draging ? YT.Theme.Elevation5 : YT.Theme.Elevation0
    spacing: YT.Theme.Space150
    implicitWidth: leftPadding + implicitContentWidth + rightPadding
    implicitHeight: topPadding + implicitContentHeight + bottomPadding
    verticalPadding: YT.Theme.Space125
    horizontalPadding: YT.Theme.Space200

    radius: {
        if (enabled == false) {
            return YT.Theme.RadiusExtraSmall;
        }
        if (checked) {
            return YT.Theme.RadiusLarge;
        } else if (pressed) {
            return YT.Theme.RadiusLarge;
        } else if (hovered) {
            return YT.Theme.RadiusMedium;
        }
        return 0;
    }
    labelColor: {
        if (enabled == false) {
            return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.38);
        }
        if (draging) {
            return YT.Theme.OnTertiaryContainer;
        } else if (checked) {
            return YT.Theme.OnSecondaryContainer;
        }
        return YT.Theme.OnSurface;
    }
    containerColor: {
        if (enabled == false) {
            return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.10);
        }
        if (draging) {
            return YT.Theme.TertiaryContainer;
        } else if (checked) {
            return YT.Theme.SecondaryContainer;
        }
        return YT.Theme.Surface;
    }
    containerLayerColor: {
        return enabled ? labelColor : YT.Theme.Transparent;
    }
}
