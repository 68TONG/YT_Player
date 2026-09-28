pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.CheckBox {
    id: root

    property bool styleType_Error: false

    padding: YT.Theme.Space25
    spacing: YT.Theme.Space100
    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding

    indicator: Loader {
        active: root.text != ""
        anchors.left: parent.left
        anchors.leftMargin: root.iconLayerSize + root.spacing + root.leftPadding
        anchors.verticalCenter: root.contentItem.verticalCenter

        sourceComponent: Text {
            text: root.text
            font: root.font
            color: root.labelColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
    }

    contentItem: Item {
        implicitWidth: root.iconLayerSize + (root.indicator && root.indicator.width > 0 ? root.indicator.width + root.spacing : 0)
        implicitHeight: root.iconLayerSize

        Item {
            width: root.iconLayerSize
            height: root.iconLayerSize
            anchors.verticalCenter: parent.verticalCenter

            Rectangle {
                anchors.centerIn: parent
                width: root.iconSize
                height: root.iconSize
                radius: 2
                color: root.containerColor
                border.color: root.outlineColor
                border.width: root.checkState == Qt.Unchecked ? 2 : 0
            }

            YT.Icon {
                anchors.centerIn: parent
                iconSize: root.iconSize
                iconColor: root.iconColor
                iconSource: {
                    if (root.checkState == Qt.Checked) {
                        return YT.IconProvider.Check;
                    } else if (root.checkState == Qt.PartiallyChecked) {
                        return YT.IconProvider.CheckIndeterminateSmall;
                    }
                    return null;
                }
            }

            YT.ControlBackground {
                anchors.fill: parent
                radius: YT.Theme.RadiusFull

                background.visible: false

                defaultLayer.color: root.containerLayerColor
                defaultLayer.visible: root.hovered

                diffusionLayerColor: root.containerLayerColor
                diffusionLayerPoint: Qt.point(width / 2, height / 2)
                diffusionLayerVisible: root.pressed
            }
        }
    }

    background: null

    property real iconSize: 18
    property real iconLayerSize: 40

    property color iconColor: {
        return YT.Theme.OnPrimary;
    }
    property color labelColor: {
        return YT.Theme.OnSurface;
    }
    property color outlineColor: {
        if (styleType_Error) {
            return YT.Theme.Error;
        } else if (hovered) {
            return YT.Theme.OnSurface;
        }
        return YT.Theme.OnSurfaceVariant;
    }
    property color containerColor: {
        if (checkState == Qt.Unchecked) {
            return YT.Theme.Transparent;
        }
        return styleType_Error ? YT.Theme.Error : YT.Theme.Primary;
    }
    property color containerLayerColor: {
        if (checkState == Qt.Unchecked) {
            return YT.Theme.OnSurface;
        }
        return styleType_Error ? YT.Theme.Error : YT.Theme.Primary;
    }
}
