import QtQuick
import YT_Material as YT_M

YT_M.DragContainer {
    id: root

    property alias m: listViewItem
    containerItem: YT_M.ListViewItem {
        id: listViewItem
        isCurrentItem: root.ListView.isCurrentItem
        width: root.width
        height: root.height
        draging: root.dragHandler.active
        droping: root.dropHandler.containsDrag
    }

    dragHandler.xAxis.enabled: ListView.view.orientation == Qt.Horizontal
    dragHandler.yAxis.enabled: ListView.view.orientation == Qt.Vertical
}
