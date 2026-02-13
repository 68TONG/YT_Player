import QtQuick
import QtQuick.Controls
import YT_Player

Rectangle {
    id: root
    radius: YT_Info.Radius
    color: YT_Info.BackgroundColor
    border.color: YT_Info.ItemFocusColor

    property int padding: 0
    property int spacing: 0

    property alias view: rootView
    property alias model: rootView.model
    property alias delegate: rootView.delegate
    property alias selectionModel: selectionModel

    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: rootView.contentItem
    }

    TreeView {
        id: rootView
        anchors.fill: parent
        anchors.margins: parent.padding

        clip: true
        rowSpacing: 3
        contentWidth: width - rightMargin

        property int margins: 0
        property int scrollbar_margin: ScrollBar.vertical && ScrollBar.vertical.parent === this
                                       ? ScrollBar.vertical.width + ScrollBar.vertical.anchors.margins * 2
                                       : 0
        topMargin: margins
        leftMargin: margins
        rightMargin: margins ? margins : scrollbar_margin
        bottomMargin: margins

        flickableDirection: Flickable.VerticalFlick
        ScrollBar.vertical: YT_ScrollBar {
            anchors.margins: 2
            anchors.right: parent.right
        }

        delegate: YT_TreeViewDelegate {}
    }

    component YT_TreeViewDelegate: TreeViewDelegate {
        required property int index
        // required property var model
        implicitWidth: parent ? parent.width : 0
        implicitHeight: contentItem.implicitHeight

        padding: 0
        leftPadding: indentation * depth + height
        indentation: 10

        background: null
        indicator: Loader {
            active: parent.hasChildren
            x: indentation * depth
            width: parent.height
            height: parent.height

            sourceComponent: YT_Button {
                padding: YT_Info.MarginSmall
                modelData: "qrc:/Resource_UI/next_page.png"
                contentItem: YT_Button.LoadImage {}
                background: null

                onClicked: treeView.toggleExpanded(index)

                // transform: Rotation {
                //     origin.x: parent.width / 2
                //     origin.y: parent.height / 2
                //     angle: parent.parent.expaned ? 180 : 0

                //     Behavior on angle {
                //         NumberAnimation { duration: 150; easing.type: Easing.InOutQuad }
                //     }
                // }
            }
        }
        contentItem: YT_Text {
            text: model.display
        }
    }

    YT_ItemSelectionModel {
        id: selectionModel

        model: rootView.model === null ? null : rootView.model
        getItem_ModelIndex: function (index) {
            return rootView.itemAtIndex(index.row)
        }
    }
}




