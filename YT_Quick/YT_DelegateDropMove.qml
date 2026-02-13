import QtQuick
import YT_Player

DropArea {
    id: root
    anchors.fill: parent
    readonly property Item target_item: parent
    property int drop_position: 0
    property int orientation: Qt.Vertical
    property var moveFunc: null

    property Item drop_item: Rectangle {
        z: +true
        parent: target_item
        visible: root.drop_position !== 0
        radius: YT_Info.Radius
        color: YT_Info.ItemFocusColor

        property int size: +true
        property real margins: 0
        states: [
            State {
                name: "TopEdge"
                when: root.orientation === Qt.Vertical && root.drop_position & Qt.TopEdge
                PropertyChanges {
                    target: drop_item
                    x: margins
                    y: 0
                    width: parent.width - margins * 2
                    height: size
                    margins: parent.width * 0.15
                }
            },
            State {
                name: "LeftEdge"
                when: root.orientation === Qt.Horizontal && root.drop_position & Qt.LeftEdge
                PropertyChanges {
                    target: drop_item
                    x: 0
                    y: margins
                    width: size
                    height: parent.height - margins * 2
                    margins: parent.height * 0.15
                }
            },
            State {
                name: "RightEdge"
                when: root.orientation === Qt.Horizontal && root.drop_position & Qt.RightEdge
                PropertyChanges {
                    target: drop_item
                    x: parent.width - size
                    y: margins
                    width: size
                    height: parent.height - margins * 2
                    margins: parent.height * 0.15
                }
            },
            State {
                name: "BottomEdge"
                when: root.orientation === Qt.Vertical && root.drop_position & Qt.BottomEdge
                PropertyChanges {
                    target: drop_item
                    x: margins
                    y: parent.height - size
                    width: parent.width - margins * 2
                    height: size
                    margins: parent.width * 0.15
                }
            }
        ]
    }

    function startDrag() {
        target_item.Drag.active = true;
        target_item.Drag.startDrag();
    }
    function updatePosition(pos) {
        var drop_position = 0;
        var this_center = Qt.point(this.width / 2, this.height / 2);
        if (pos.y < this_center.y)
            drop_position = drop_position | Qt.TopEdge;
        if (pos.x < this_center.x)
            drop_position = drop_position | Qt.LeftEdge;
        if (pos.x >= this_center.x)
            drop_position = drop_position | Qt.RightEdge;
        if (pos.y >= this_center.y)
            drop_position = drop_position | Qt.BottomEdge;
        root.drop_position = drop_position;
    }

    onEntered: function (drag) {
        updatePosition(Qt.point(drag.x, drag.y));
    }
    onPositionChanged: function (drag) {
        updatePosition(Qt.point(drag.x, drag.y));
    }
    onDropped: function (drop) {
        var orientation = root.orientation;
        var source_index = drop.source.index;
        var target_index = target_item.index;

        if (source_index > target_index) {
            if (orientation === Qt.Vertical && root.drop_position & Qt.BottomEdge) {
                target_index = target_index + true;
            }
            if (orientation === Qt.Horizontal && root.drop_position & Qt.RightEdge) {
                target_index = target_index + true;
            }
        }
        if (source_index < target_index) {
            if (orientation === Qt.Vertical && root.drop_position & Qt.TopEdge) {
                target_index = target_index - true;
            }
            if (orientation === Qt.Horizontal && root.drop_position & Qt.LeftEdge) {
                target_index = target_index - true;
            }
        }
        // if (target_index >= 0 && target_index < info_YT.model.count && target_index !== source_index)
        // info_YT.model.move(source_index, target_index, +true);
        if (source_index != target_index)
            moveFunc(source_index, target_index);
        root.drop_position = 0;
    }
    onExited: function () {
        root.drop_position = 0;
    }
}
