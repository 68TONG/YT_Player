import QtQuick
import YT_Material as YT

YT.AbstractButton {
    id: root

    property int styleType: (ListView.view.parent as YT.NavigationBar).styleType
    display: [YT.NavigationBar.ViewH_ItemH, YT.NavigationBar.ViewV_ItemH].includes(styleType) ? AbstractButton.TextBesideIcon : AbstractButton.TextUnderIcon
    radius: YT.Theme.RadiusFull
    spacing: ListView.view.orientation == Qt.Vertical ? YT.Theme.Space100 : YT.Theme.Space50
    verticalPadding: YT.Theme.Space75
    horizontalPadding: YT.Theme.Space200
    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: {
        if (styleType == YT.NavigationBar.ViewV_ItemH) {
            return 56;
        } else if (styleType == YT.NavigationBar.ViewV_ItemV) {
            return 64;
        }
        return implicitContentHeight + topPadding + bottomPadding;
    }

    iconSize: 24
    iconColor: labelColor
    labelColor: {
        if (ListView.isCurrentItem) {
            return YT.Theme.Secondary;
        }
        return YT.Theme.OnSurfaceVariant;
    }

    property bool isVertical: display == AbstractButton.TextUnderIcon
    property bool isHorizontal: display == AbstractButton.TextBesideIcon

    property real containerWidth: 56
    property real containerHeight: 32
    containerColor: {
        if (ListView.isCurrentItem) {
            return YT.Theme.SecondaryContainer;
        }
        return YT.Theme.Transparent;
    }
    containerLayerColor: {
        return YT.Theme.OnSecondaryContainer;
    }

    indicator: null

    contentItem: Item {
        implicitWidth: iconItem.implicitWidth + (root.isHorizontal ? textItem.implicitWidth + root.spacing : 0)
        implicitHeight: iconItem.implicitHeight
        YT.Icon {
            id: iconItem
            fontFill: root.ListView.isCurrentItem
            iconSize: root.iconSize
            iconColor: root.labelColor || root.icon.color
            iconSource: {
                if (root.iconSource != null)
                    return root.iconSource;
                if (root.icon.name != "")
                    return root.icon.name;
                return root.icon.source;
            }
            states: State {
                when: root.isHorizontal
                AnchorChanges {
                    target: iconItem
                    anchors.verticalCenter: iconItem.parent.verticalCenter
                }
            }
        }

        Text {
            id: textItem
            text: root.text
            font: root.font
            color: root.labelColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            anchors.topMargin: root.isVertical ? root.spacing : 0
            anchors.leftMargin: root.isHorizontal ? root.spacing : 0

            states: [
                State {
                    when: root.isVertical
                    AnchorChanges {
                        target: textItem
                        anchors.top: iconItem.bottom
                        anchors.horizontalCenter: iconItem.horizontalCenter
                    }
                },
                State {
                    when: root.isHorizontal
                    AnchorChanges {
                        target: textItem
                        anchors.left: iconItem.right
                        anchors.verticalCenter: iconItem.verticalCenter
                    }
                }
            ]
        }
    }

    background: YT.ControlBackground {
        radius: root.radius
        background.color: root.containerColor

        defaultLayer.color: root.containerLayerColor
        defaultLayer.visible: root.hovered

        diffusionLayerColor: root.containerLayerColor
        diffusionLayerPoint: mapFromItem(root, root.pressX, root.pressY)
        diffusionLayerVisible: root.pressed

        states: [
            State {
                when: root.styleType == YT.NavigationBar.ViewV_ItemV
                PropertyChanges {
                    root.background.x: root.leftPadding + iconItem.x + (iconItem.width - root.background.width) / 2
                    root.background.y: root.topPadding + iconItem.y + (iconItem.height - root.background.height) / 2
                    root.background.width: 56
                    root.background.height: 32
                }
            },
            State {
                when: root.styleType == YT.NavigationBar.ViewV_ItemH
                PropertyChanges {
                    root.background.x: 0
                    root.background.y: 0
                    root.background.width: root.width
                    root.background.height: root.height
                }
            },
            State {
                when: root.styleType == YT.NavigationBar.ViewH_ItemV
                PropertyChanges {
                    root.background.x: root.leftPadding + iconItem.x + (iconItem.width - root.background.width) / 2
                    root.background.y: root.topPadding + iconItem.y + (iconItem.height - root.background.height) / 2
                    root.background.width: 56
                    root.background.height: 32
                }
            },
            State {
                when: root.styleType == YT.NavigationBar.ViewH_ItemH
                PropertyChanges {
                    root.background.x: 0
                    root.background.y: root.topPadding + iconItem.y + (iconItem.height - root.background.height) / 2
                    root.background.width: root.width
                    root.background.height: 40
                }
            }
        ]
    }
}
