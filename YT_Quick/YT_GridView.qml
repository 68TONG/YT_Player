import QtQuick
import QtQuick.Controls
import YT_Player

Control {
    id: root

    property int radius: YT_Info.Radius

    property alias view: rootView
    property alias model: rootView.model
    property alias delegate: rootView.delegate
    property alias selectionModel: selectionModel

    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: rootView.contentItem
    }

    background: YT_Rectangle {
        radius: root.radius
        border.color: YT_Info.ItemFocusColor
    }
    contentItem: GridView {
        id: rootView

        clip: true
        property int margins: 0
        property int scrollbar_margin: ScrollBar.vertical && ScrollBar.vertical.parent === this ? ScrollBar.vertical.width + ScrollBar.vertical.anchors.margins * 2 : 0
        topMargin: margins
        leftMargin: margins
        rightMargin: margins ? margins : scrollbar_margin
        bottomMargin: margins
    }

    YT_ItemSelectionModel {
        id: selectionModel

        // model: rootView.model === null ? null : rootView.model
        // getItem_ModelIndex: function (index) {
        //     return rootView.itemAtIndex(index.row);
        // }
    }
}
