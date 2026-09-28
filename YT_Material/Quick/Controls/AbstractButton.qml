pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.AbstractButton {
    id: root

    property int sizeType
    property int styleType
    property int elevation

    property real radius
    property real topLeftRadius: radius
    property real topRightRadius: radius
    property real bottomLeftRadius: radius
    property real bottomRightRadius: radius

    property real outlineWidth
    property color outlineColor: YT.Theme.OutlineVariant

    property real iconSize: height - topPadding - bottomPadding
    property color iconColor: labelColor
    property variant iconSource

    property color labelColor
    property color containerColor
    property color containerLayerColor

    property real downX: pressX
    property real downY: pressY

    indicator: YT.Icon {
        enabled: false
        anchors.left: root.left
        anchors.leftMargin: root.leftPadding
        anchors.verticalCenter: root.verticalCenter

        iconSize: root.iconSize
        iconColor: root.iconColor || root.labelColor || root.icon.color
        iconSource: {
            if (root.iconSource != null)
                return root.iconSource;
            if (root.icon.name != "")
                return root.icon.name;
            return root.icon.source;
        }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        color: root.labelColor
        leftPadding: root.indicator && root.indicator.width > 0 ? root.indicator.width + root.spacing : 0
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }
    
    background: YT.ControlBackground {
        radius: root.radius
        topLeftRadius: root.topLeftRadius
        topRightRadius: root.topRightRadius
        bottomLeftRadius: root.bottomLeftRadius
        bottomRightRadius: root.bottomRightRadius

        background.color: root.containerColor
        background.border.width: root.outlineWidth

        defaultLayer.color: root.containerLayerColor
        defaultLayer.visible: root.hovered

        diffusionLayerColor: root.containerLayerColor
        diffusionLayerPoint: mapFromItem(root, root.downX, root.downY)
        diffusionLayerVisible: root.down
        elevation: root.elevation
    }
}
