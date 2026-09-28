import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.Container {
    id: root

    property int styleType: NavigationBar.ViewV_ItemH
    enum StyleT {
        ViewH_ItemH,
        ViewH_ItemV,
        ViewV_ItemH,
        ViewV_ItemV
    }

    spacing: YT.Theme.Space100
    property real radius: expanded ? YT.Theme.RadiusLarge : YT.Theme.RadiusNone
    implicitWidth: (orientation == Qt.Vertical ? root.containerSize : implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: (orientation == Qt.Horizontal ? root.containerSize : implicitContentHeight + topPadding + bottomPadding)
    topPadding: orientation == Qt.Vertical ? 44 : 0

    property color containerColor: YT.Theme.Surface
    property real containerSize: {
        if (orientation == Qt.Vertical) {
            return expanded ? 360 : 96;
        } else if (orientation == Qt.Horizontal) {
            return 56;
        }
        return 0;
    }

    property alias view: rootView
    property alias orientation: rootView.orientation
    property bool expanded: false

    contentItem: ListView {
        id: rootView
        interactive: false
        orientation: [NavigationBar.ViewH_ItemH, NavigationBar.ViewH_ItemV].includes(root.styleType) ? Qt.Horizontal : Qt.Vertical

        spacing: root.spacing
        implicitWidth: contentItem.width
        implicitHeight: contentItem.height
        model: root.contentModel
    }

    background: Rectangle {
        color: root.containerColor
        radius: root.radius
    }

    property Item fAB: null
    property Item menu: null
}
