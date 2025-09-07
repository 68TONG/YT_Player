import QtQuick
import QtQuick.Controls.Basic

import YT_Player

Rectangle {
    id: root

    radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
    color: YT_ConfigureInfo.getData(YT_ConfigureInfo.BackgroundColor)
    border.color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)

    property int padding: 0
    property int spacing: 0
    property Item header: null

    property alias view: rootView
    property alias model: rootView.model
    property alias delegate: rootView.delegate
    property alias selectionModel: selectionModel

    property YT_ItemDelegate.YT_DelegateInfo delegateInfo: YT_ItemDelegate.YT_DelegateInfo {
        view: rootView
        model: rootView.model
    }

    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: rootView.contentItem
    }

    ListView {
        id: rootView
        clip: true
        anchors.fill: parent
        anchors.margins: parent.padding

        property int margins: 0
        property int scrollbar_margin: ScrollBar.vertical && ScrollBar.vertical.parent === this
                                       ? ScrollBar.vertical.width + ScrollBar.vertical.anchors.margins * 2
                                       : 0

        topMargin: margins
        leftMargin: margins
        rightMargin: margins ? margins : scrollbar_margin
        bottomMargin: margins

        highlightFollowsCurrentItem: false
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: 300; easing.type: Easing.OutCubic}
        }
        move: Transition {
            NumberAnimation { properties: "x,y"; duration: 300; easing.type: Easing.OutCubic}
        }
    }

    states: [
        State {
            name: "header_view"
            when: root.header !== null
            PropertyChanges {
                target: root.header
                anchors.margins: root.padding
                anchors.bottomMargin: 0

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
            }
            PropertyChanges {
                target: rootView
                anchors.margins: root.padding
                anchors.topMargin: root.spacing

                anchors.fill: undefined
                anchors.top: root.header.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
        }
    ]

    YT_ItemSelectionModel {
        id: selectionModel

        model: rootView.model === null ? null : rootView.model
        getItem_ModelIndex: function (index) {
            return rootView.itemAtIndex(index.row)
        }
    }
}




