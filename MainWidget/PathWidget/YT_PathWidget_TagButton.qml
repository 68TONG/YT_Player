import QtQuick
import YT_Material as YT_M

YT_M.AbstractButton {
    id: root

    width: Math.min(parent ? parent.width : implicitWidth, implicitWidth)
    height: YT_M.Font.LabelLargeLineHeight + topPadding + bottomPadding

    indicator: null
    contentItem: Text {
        text: root.text
        font: root.font
        color: root.labelColor
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }

    font: YT_M.Font.LabelLarge
    verticalPadding: YT_M.Theme.Space50
    horizontalPadding: YT_M.Theme.Space75

    radius: pressed ? YT_M.Theme.RadiusSmall : YT_M.Theme.RadiusMedium
    labelColor: hovered ? YT_M.Theme.Primary : YT_M.Theme.OnSurfaceVariant
    containerColor: YT_M.Theme.Transparent
    containerLayerColor: enabled ? labelColor : YT_M.Theme.Transparent

    visible: opacity != 0.0
}
