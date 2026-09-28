import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.Control {
    id: root
    property bool expanded: false
    property string text
    property variant iconSource

    clip: true
    spacing: YT.Theme.Space50
    implicitWidth: Math.max(foldableView.width, implicitContentWidth)
    implicitHeight: implicitContentHeight + bottomPadding
    bottomPadding: expanded ? foldableView.height + spacing : 0
    Behavior on bottomPadding {
        NumberAnimation {
            duration: 180
            easing.type: Easing.OutQuad
        }
    }

    contentItem: YT.Button {
        text: root.text
        iconSource: root.iconSource
        checkable: true
        checked: root.expanded
        onCheckedChanged: root.expanded = checked
    }
    background: null

    property Item foldableView: null
    onFoldableViewChanged: {
        if (foldableView && foldableView.parent == null) {
            foldableView.parent = root;
            foldableView.y = Qt.binding(function () {
                return root.contentItem.height + root.spacing;
            });
        }
    }
}
