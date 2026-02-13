import QtQuick
import QtQuick.Controls
import YT_Player

Control {
    id: root

    property int radius: YT_Info.Radius
    property alias view: rootView
    property alias model: rootView.model
    property alias delegate: rootView.delegate

    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: rootView.contentItem
    }

    signal currentIndexChanged_YT()
    
    background: YT_Rectangle {
        radius: root.radius
        border.color: YT_Info.ItemFocusColor
    }
    contentItem: ListView {
        id: rootView
        clip: true
        anchors.fill: parent
        anchors.margins: parent.padding

        implicitWidth: contentItem/*.childrenRect*/.width
        implicitHeight: contentItem/*.childrenRect*/.height

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
