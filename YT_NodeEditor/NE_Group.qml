import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import YT_Player

NE_Item {
    id: root

    property bool selected: false
    width: YT_Info.ItemSize.width
    height: titleItem.implicitHeight + contentItem.implicitHeight + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin
    YT_TransformWindow {
        z: -true
        enabled_move: false
        enabled_type: Qt.LeftEdge | Qt.RightEdge
    }

    readonly property alias background: background
    Rectangle {
        id: background
        anchors.fill: parent
        radius: YT_Info.RadiusSmall
        color: YT_Info.BackgroundColor
        border.color: selected ? YT_Info.FontFocusColor : YT_Info.ItemFocusColor
    }

    readonly property alias titleItem: titleItem
    Control {
        id: titleItem
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        enabled: false
        hoverEnabled: false
        horizontalPadding: YT_Info.Margin
        verticalPadding: YT_Info.MarginSmall

        contentItem: YT_Text {
            text: "NE_Group"
            elide: Text.ElideRight
        }
        background: Rectangle {
            radius: root.background.radius
            color: root.background.color
            border.color: root.background.border.color
        }
    }

    readonly property alias contentItem: contentItem
    default property alias contentItem_children: contentItem.children
    ColumnLayout {
        id: contentItem
        anchors.top: titleItem.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: YT_Info.Margin
        anchors.leftMargin: anchors.margins + YT_Info.MarginSmall
        anchors.rightMargin: anchors.margins + YT_Info.MarginSmall
        spacing: YT_Info.Spacing
        clip: true

        onChildrenChanged: {
            for (var i = 0; i < contentItem_children.length; i++) {
                let child = contentItem_children[i];
                if ((child instanceof NE_Data) === false)
                    continue;

                if (child.inputNode) {
                    child.inputNode.parent = root;

                    child.inputPos = Qt.binding(function () {
                        const x = root.inputPos.x;
                        const y = YT.mapFromGlobal(root, child, Qt.point(0, child.height / 2)).y;
                        return Qt.point(x, y);
                    });
                }
                if (child.outputNode) {
                    child.outputNode.parent = root;

                    child.outputPos = Qt.binding(function () {
                        const x = root.outputPos.x;
                        const y = YT.mapFromGlobal(root, child, Qt.point(0, child.height / 2)).y;
                        return Qt.point(x, y);
                    });
                }
            }
        }
    }
}
