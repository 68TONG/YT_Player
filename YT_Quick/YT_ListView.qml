import QtQuick
import QtQuick.Controls
import YT_Player

Control {
    id: root

    property int radius: YT_Info.Radius
    property Item header: null
    onHeaderChanged: {
        if (header == null) {
            rootView.anchors.fill = root;
            rootView.anchors.margins = root.padding;
        } else if (header != null && header.parent == root) {
            header.anchors.margins = root.padding;
            header.anchors.bottomMargin = 0;
            header.anchors.top = root.top;
            header.anchors.left = root.left;
            header.anchors.right = root.right;

            rootView.anchors.margins = root.padding;
            rootView.anchors.topMargin = root.spacing;
            rootView.anchors.fill = undefined;
            rootView.anchors.top = header.bottom;
            rootView.anchors.left = root.left;
            rootView.anchors.right = root.right;
            rootView.anchors.bottom = root.bottom;
        }
    }

    property Item footer: null
    onFooterChanged: {
        if (footer != null && footer.parent == root) {
            footer.z = Qt.binding(function () {
                return rootView.z + true;
            });
            // footer.anchors.horizontalCenter = root.horizontalCenter;
            footer.anchors.bottomMargin = root.padding + YT_Info.Margin;
            footer.anchors.bottom = root.bottom;
        }
    }

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

    background: YT_Rectangle {
        radius: root.radius
        border.color: YT_Info.ItemFocusColor
    }
    contentItem: ListView {
        id: rootView
        clip: true
        
        implicitWidth: contentItem/*.childrenRect*/.width
        implicitHeight: contentItem/*.childrenRect*/.height

        property int margins: 0
        property int scrollbar_margin: ScrollBar.vertical && ScrollBar.vertical.parent === this ? ScrollBar.vertical.width + ScrollBar.vertical.anchors.margins * 2 : 0

        topMargin: margins
        leftMargin: margins
        rightMargin: margins ? margins : scrollbar_margin
        bottomMargin: margins

        highlightFollowsCurrentItem: false
        displaced: Transition {
            NumberAnimation {
                properties: "x,y"
                duration: 300
                easing.type: Easing.OutCubic
            }
        }
        move: Transition {
            NumberAnimation {
                properties: "x,y"
                duration: 300
                easing.type: Easing.OutCubic
            }
        }
    }

    YT_ItemSelectionModel {
        id: selectionModel

        model: rootView.model === null ? null : rootView.model
        getItem_ModelIndex: function (index) {
            return rootView.itemAtIndex(index.row);
        }
    }
}
