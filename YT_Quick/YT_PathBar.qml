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

    ListView {
        id: rootView
        clip: true
        anchors.fill: parent
        anchors.margins: parent.padding
        orientation: Qt.Horizontal

        property int margins: 0
        topMargin: margins
        leftMargin: margins
        rightMargin: margins
        bottomMargin: margins

        highlightFollowsCurrentItem: false
    }
}
