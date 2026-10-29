import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT_M

T.AbstractButton {
    id: root

    property int sizeType
    property int styleType
    property int elevation: YT_M.Theme.ElevationNone

    property real inset: 0
    topInset: inset
    leftInset: inset
    rightInset: inset
    bottomInset: inset

    property real radius
    property real topLeftRadius: radius
    property real topRightRadius: radius
    property real bottomLeftRadius: radius
    property real bottomRightRadius: radius

    property real outlineWidth
    property color outlineColor: YT_M.Theme.OutlineVariant

    property bool iconFill: false
    property real iconSize: height - topPadding - bottomPadding
    property color iconColor: labelColor
    property variant iconSource

    property color labelColor
    property real labelLineHeight: 1.0

    property color containerColor
    property color containerLayerColor

    property real downX: pressX
    property real downY: pressY

    display: Qt.LeftEdge
    indicator: YT_M.Icon {
        enabled: false
        fontFill: root.iconFill
        iconSize: root.iconSize
        iconColor: root.iconColor || root.labelColor || root.icon.color
        iconSource: {
            if (root.iconSource != null)
                return root.iconSource;
            if (root.icon.name != "")
                return root.icon.name;
            return root.icon.source;
        }

        states: [
            State {
                when: root.display == Qt.TopEdge
                AnchorChanges {
                    target: root.indicator
                    anchors.top: root.top
                    anchors.horizontalCenter: root.horizontalCenter
                }
                PropertyChanges {
                    root.indicator.anchors.topMargin: root.topPadding
                }
            },
            State {
                when: root.display == Qt.LeftEdge
                AnchorChanges {
                    target: root.indicator
                    anchors.left: root.left
                    anchors.verticalCenter: root.verticalCenter
                }
                PropertyChanges {
                    root.indicator.anchors.leftMargin: root.leftPadding
                }
            },
            State {
                when: root.display == Qt.RightEdge
                AnchorChanges {
                    target: root.indicator
                    anchors.right: root.right
                    anchors.verticalCenter: root.verticalCenter
                }
                PropertyChanges {
                    root.indicator.anchors.rightMargin: root.rightPadding
                }
            }
        ]
    }

    contentItem: Text {
        property bool isIndicator: root.indicator && root.indicator.width > 0 && root.indicator.height > 0
        text: root.text
        font: root.font
        color: root.labelColor
        topPadding: isIndicator && root.display == Qt.TopEdge ? root.indicator.height + root.spacing : 0
        leftPadding: isIndicator && root.display == Qt.LeftEdge ? root.indicator.width + root.spacing : 0
        rightPadding: isIndicator && root.display == Qt.RightEdge ? root.indicator.width + root.spacing : 0
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        lineHeight: root.labelLineHeight
        lineHeightMode: lineHeight != 1.0 ? Text.FixedHeight : Text.ProportionalHeight
    }

    background: YT_M.Background {
        initProperty: root
    }

    autoRepeatDelay: 1000
    property real autoRepeatProgress: Math.min((defaultTapHandler.timeHeld * 1000) / autoRepeatDelay, 1.0)
    signal clickedEx(eventPoint: eventPoint, button: int, modifiers: int)
    TapHandler {
        id: defaultTapHandler
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        longPressThreshold: root.autoRepeatDelay
        onTapped: function (eventPoint, button) {
            root.clickedEx(eventPoint, button, Qt.NoModifier);
        }
    }
}
