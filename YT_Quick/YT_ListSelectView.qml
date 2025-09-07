import QtQuick
import QtQuick.Controls.Basic

import YT_Player

Rectangle {
    id: root

    radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
    color: YT_ConfigureInfo.getData(YT_ConfigureInfo.BackgroundColor)
    border.color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)

    property int padding: 0
    property alias view: rootView
    property alias model: rootView.model
    property alias delegate: rootView.delegate
    property alias contentItem: rootView.contentItem

    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: rootView.contentItem
    }

    signal currentIndexChanged_YT()
    ListView {
        id: rootView
        clip: true
        anchors.fill: parent
        anchors.margins: parent.padding

        property int margins: 0
        topMargin: margins
        leftMargin: margins
        rightMargin: margins
        bottomMargin: margins

        currentIndex: 0
        highlight: null
        highlightRangeMode: ListView.StrictlyEnforceRange
        preferredHighlightBegin: preferredHighlightCenter
        preferredHighlightEnd: preferredHighlightCenter
        property real preferredHighlightCenter: {
            if (orientation === Qt.Horizontal) {
                return width / 2 - (currentItem ? currentItem.width / 2 : 0)
            } else if (orientation === Qt.Vertical) {
                return height / 2 - (currentItem ? currentItem.height / 2 : 0)
            }
            return 0
        }

        onMovementEnded: currentIndexChanged_YT()

        WheelHandler {
            onWheel: function(event) {
                if (event.angleDelta.y < 0) {
                    rootView.incrementCurrentIndex()
                } else if (event.angleDelta.y > 0) {
                    rootView.decrementCurrentIndex()
                }
                currentIndexChanged_YT()
            }
        }

        Keys.onPressed: function(event) {
            console.log(event.accepted, event.key)
            if (event.key === Qt.Key_Down) {
                rootView.incrementCurrentIndex()
            } else if (event.key === Qt.Key_Up) {
                rootView.decrementCurrentIndex()
            } else if (event.key === Qt.Key_Right) {
                rootView.incrementCurrentIndex()
            } else if (event.key === Qt.Key_Left) {
                rootView.decrementCurrentIndex()
            } else return
            event.accepted = true
            currentIndexChanged_YT()
        }
    }
}
