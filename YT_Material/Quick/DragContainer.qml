import QtQuick
import YT_Material as YT_M

Item {
    id: root

    property bool dragEnabled: true
    property bool dropEnabled: true
    readonly property bool draging: dragHandler.active || dragRetAnimation.running
    readonly property bool droping: dropHandler.containsDrag
    z: draging
    implicitWidth: containerItem ? containerItem.implicitWidth : 0
    implicitHeight: containerItem ? containerItem.implicitHeight : 0

    property alias dragHandler: dragHandler
    DragHandler {
        id: dragHandler
        enabled: root.dragEnabled
        target: root.containerItem
        acceptedButtons: Qt.LeftButton
        snapMode: DragHandler.NoSnap
        dragThreshold: 5

        onActiveChanged: {
            if (active) {
                let pos = target.parent.mapToItem(root.targetView, target.x, target.y);
                target.parent = root.targetView;
                target.x = pos.x;
                target.y = pos.y;
                target.z = root.z + 1;

                target.Drag.active = true;
                target.Drag.hotSpot.x = centroid.position.x;
                target.Drag.hotSpot.y = centroid.position.y;
            } else {
                target.Drag.drop();
                target.Drag.active = false;

                let pos = root.mapFromItem(target.parent, target.x, target.y);
                target.parent = root;
                target.x = pos.x;
                target.y = pos.y;
                target.z = 0;
                dragRetAnimation.start();
            }
        }
    }

    property alias dropHandler: dropHandler
    DropArea {
        id: dropHandler
        anchors.fill: parent
        enabled: dragHandler.active == false && root.dropEnabled
    }

    NumberAnimation {
        id: dragRetAnimation
        target: root.containerItem
        properties: "x,y"
        to: 0
        duration: YT_M.Easing.Medium3
        easing: YT_M.Easing.SpringFastEx
    }

    property Item targetView: parent
    property Item containerItem
    onContainerItemChanged: {
        if (containerItem && containerItem.parent == null) {
            containerItem.parent = root;
            containerItem.Drag.source = root;
        }
    }
}
