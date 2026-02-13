import QtQuick
import QtQuick.Controls
import YT_Player

Popup {
    id: root
    property alias view: rootView
    property alias model: rootView.model
    property alias delegate: rootView.delegate
    function open_YT(target) {
        parent = target;
        open();
    }

    x: 0
    y: parent ? parent.height + YT_Info.SpacingSmall : 0
    width: parent ? parent.width : 0
    height: implicitContentHeight * Math.min(5, rootView.count) / rootView.count + (rootView.count ? padding * 2 : 0)
    padding: YT_Info.Margin

    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: rootView.contentItem
        followItem: rootView.currentItem
    }

    background: YT_Rectangle {
        color: YT_Info.BackgroundColor
        border.color: YT_Info.ItemFocusColor
    }
    contentItem: ListView {
        id: rootView

        clip: true
        implicitWidth: contentItem/*.childrenRect*/.width
        implicitHeight: contentItem/*.childrenRect*/.height
        highlightFollowsCurrentItem: false

        model: null
        delegate: null

        ScrollBar.vertical: YT_ScrollBar {
            parent: root.background
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 2
        }
    }

    enter: Transition {
        NumberAnimation {
            property: "height"
            duration: 300
            from: 0.0
            to: root.height
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            property: "opacity"
            duration: 300
            from: 0.0
            to: 1.0
            easing.type: Easing.OutQuad
        }
    }
    exit: Transition {
        NumberAnimation {
            property: "opacity"
            duration: 300
            from: 1.0
            to: 0.0
            easing.type: Easing.OutQuad
        }
    }
}
