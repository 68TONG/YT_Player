pragma ComponentBehavior: Bound
import QtQuick
import YT_Material as YT_M

YT_M.AbstractButton {
    id: root

    required property int depth
    required property bool expanded
    required property bool hasChildren

    property int indent: indicator.width
    radius: pressed || checked ? YT_M.Theme.RadiusSmall : YT_M.Theme.RadiusFull
    leftPadding: depth * indent + horizontalPadding
    rightPadding: horizontalPadding
    verticalPadding: YT_M.Theme.Space75
    horizontalPadding: YT_M.Theme.Space25

    property bool indicatorHovered
    signal indicatorClicked
    indicator: YT_M.Icon {
        enabled: true
        anchors.left: root.left
        anchors.leftMargin: root.leftPadding
        anchors.verticalCenter: root.verticalCenter

        fontFill: root.iconFill
        iconSize: root.iconSize
        iconColor: root.iconColor
        iconSource: root.iconSource

        rotation: root.expanded ? 90 : 0
        Behavior on rotation {
            NumberAnimation {
                duration: 180
                easing.type: Easing.OutInQuad
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onContainsMouseChanged: {
                root.indicatorHovered = containsMouse;
            }
            onClicked: {
                root.indicatorClicked();
            }
        }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        elide: Text.ElideRight
        color: root.labelColor
        leftPadding: root.indicator.width + root.spacing
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }

    background: YT_M.Background {
        initProperty: root
        Repeater {
            model: root.depth
            delegate: Rectangle {
                required property int index
                x: root.horizontalPadding + (root.indicator.width / 2) + (root.indent * index)
                width: +true
                height: parent.height
                color: YT_M.Theme.OutlineVariant
            }
        }
    }

    iconSource: YT_M.IconProvider.KeyboardArrowRight
    iconColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
        }
        if (indicatorHovered) {
            return YT_M.Theme.Primary;
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
        return YT_M.Theme.Transparent;
    }
    containerLayerColor: {
        return enabled ? labelColor : YT_M.Theme.Transparent;
    }
}
