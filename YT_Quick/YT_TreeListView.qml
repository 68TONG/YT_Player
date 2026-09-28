import QtQuick
import QtQuick.Controls
import YT_Player

Control {
    id: root

    padding: 0
    property var model: null
    property alias delegate: rootView.delegate
    property alias view: rootView
    property alias proxyModel: proxyModel

    function depth(row) {
        return proxyModel.depth(row);
    }
    function expand(row) {
        return proxyModel.expand(row);
    }
    function collapse(row) {
        return proxyModel.collapse(row);
    }
    function isExpanded(row) {
        return proxyModel.isExpanded(row);
    }
    function toggleExpanded(row) {
        return proxyModel.toggleExpanded(row);
    }

    YT_AbstractTreeProxyModel {
        id: proxyModel
        m_sourceModel: root.model
    }

    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: rootView.contentItem
    }

    background: YT_Rectangle {
        radius: YT_Info.Radius
        color: YT_Info.BackgroundColor
        border.color: "#2f3b4c"
    }
    contentItem: ListView {
        id: rootView
        model: proxyModel

        add: Transition {
            NumberAnimation {
                properties: "opacity"
                from: 0
                to: 1
                duration: 150
                easing.type: Easing.OutQuad
            }
        }
        remove: Transition {
            NumberAnimation {
                properties: "opacity"
                from: 1
                to: 0
                duration: 150
                easing.type: Easing.OutQuad
            }
        }
        displaced: Transition {
            NumberAnimation {
                properties: "y"
                duration: 180
                easing.type: Easing.OutQuad
            }
        }
    }
}
