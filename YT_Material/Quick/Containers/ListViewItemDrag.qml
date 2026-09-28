import QtQuick
import YT_Material as YT

YT.DragContainer {
    id: root

    property alias m: listViewItem
    containerItem: YT.ListViewItem {
        id: listViewItem
        isCurrentItem: root.ListView.isCurrentItem
        draging: root.dragHandler.active
        droping: root.dropHandler.containsDrag 
    }

    dragHandler.xAxis.enabled: ListView.view.orientation == Qt.Horizontal
    dragHandler.yAxis.enabled: ListView.view.orientation == Qt.Vertical
}
