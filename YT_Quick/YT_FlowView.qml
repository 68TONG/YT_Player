import QtQuick
import QtQuick.Controls
import YT_Player

Control {
    id: root

    property int radius: YT_Info.Radius

    property alias view: rootView
    property alias model: rootInstantiator.model
    property alias delegate: rootInstantiator.delegate
    property alias selectionModel: selectionModel

    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: rootView.contentItem
    }

    background: YT_Rectangle {
        radius: root.radius
        border.color: YT_Info.ItemFocusColor
    }
    contentItem: Flickable {
        id: rootFlickable
        clip: true
        property int margins: 0
        property int scrollbar_margin: ScrollBar.vertical && ScrollBar.vertical.parent === this ? ScrollBar.vertical.width + ScrollBar.vertical.anchors.margins * 2 : 0
        topMargin: margins
        leftMargin: margins
        rightMargin: margins ? margins : scrollbar_margin
        bottomMargin: margins

        contentWidth: contentItem.childrenRect.width
        contentHeight: contentItem.childrenRect.height

        Flow {
            id: rootView
            width: rootFlickable.width
            height: implicitHeight

            move: Transition {
                NumberAnimation {
                    properties: "x,y"
                    duration: 180
                    easing.type: Easing.OutCubic
                }
            }
        }
    }

    Instantiator {
        id: rootInstantiator
        onObjectAdded: function (index, object) {
            object.parent = rootView;
        }
        onObjectRemoved: function (index, object) {
            object.parent = null;
        }
    }

    YT_ItemSelectionModel {
        id: selectionModel

        // model: rootView.model === null ? null : rootView.model
        // getItem_ModelIndex: function (index) {
        //     return rootView.itemAtIndex(index.row);
        // }
    }
}
