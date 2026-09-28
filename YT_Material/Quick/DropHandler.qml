import QtQuick
import YT_Material as YT

Item {
    id: root

    property Item targetItem: parent
    property Item targetView: targetItem.parent
    property alias dropHandler: dropHandler
    property alias dragHandler: dragHandler
    property alias dragObject: dragObject

    property bool active: false
    property bool focusing: dragObject.visible
    property int orientation: Qt.Vertical
    property var dropFunc: null

    states: [
        State {
            when: root.focusing
            PropertyChanges {
                root.targetItem.opacity: 0.0
            }
        }
    ]

    DragHandler {
        id: dragHandler
        enabled: root.active
        target: dragObject
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        snapMode: DragHandler.NoSnap

        onGrabChanged: function (transition, point) {
            if (transition == PointerDevice.GrabPassive) {
                dragObject.x = root.targetItem.x;
                dragObject.y = root.targetItem.y;
                dragObject.Drag.hotSpot.x = point.position.x;
                dragObject.Drag.hotSpot.y = point.position.y;
            } else if (transition == PointerDevice.GrabExclusive) {
                dragObject.Drag.active = true;
            } else {
                dragObject.Drag.drop();
                dragObject.Drag.active = false;
            }
        }
    }

    DropArea {
        id: dropHandler
        anchors.fill: parent

        property int offsetType: 0
        function runDropFunc(evt, func = null) {
            let drop_position = 0;
            const pos = Qt.point(evt.x, evt.y);
            const center_pos = Qt.point(width / 2, height / 2);
            if (pos.y < center_pos.y)
                drop_position = drop_position | Qt.TopEdge;
            if (pos.x < center_pos.x)
                drop_position = drop_position | Qt.LeftEdge;
            if (pos.x >= center_pos.x)
                drop_position = drop_position | Qt.RightEdge;
            if (pos.y >= center_pos.y)
                drop_position = drop_position | Qt.BottomEdge;

            let offset_type = 0;
            let source_index = evt.source.index;
            let target_index = root.targetItem.index;
            const is_ver = root.orientation == Qt.Vertical;
            const is_hor = root.orientation == Qt.Horizontal;
            if (source_index > target_index) {
                if (is_ver && drop_position & Qt.BottomEdge) {
                    target_index = target_index + true;
                    offset_type = source_index == target_index ? 0 : Qt.TopEdge;
                } else if (is_hor && drop_position & Qt.RightEdge) {
                    target_index = target_index + true;
                    offset_type = source_index == target_index ? 0 : Qt.LeftEdge;
                } else {
                    offset_type = is_ver ? Qt.BottomEdge : Qt.RightEdge;
                }
            }
            if (source_index < target_index) {
                if (is_ver && drop_position & Qt.TopEdge) {
                    target_index = target_index - true;
                    offset_type = source_index == target_index ? 0 : Qt.BottomEdge;
                } else if (is_hor && drop_position & Qt.LeftEdge) {
                    target_index = target_index - true;
                    offset_type = source_index == target_index ? 0 : Qt.RightEdge;
                } else {
                    offset_type = is_ver ? Qt.TopEdge : Qt.LeftEdge;
                }
            }

            if (func != null && source_index != target_index) {
                func(source_index, target_index);
            } else {
                offsetType = offset_type;
            }
        }

        property real dropOffset: 0
        onDropped: function (drop) {
            runDropFunc(drop, root.dropFunc);
            dropOffset = root.orientation == Qt.Vertical ? (drop.source as Item).y : (drop.source as Item).x
            dragObject.state = 'droped';
        }
        onEntered: function (drag) {
            runDropFunc(drag);
        }
        onPositionChanged: function (drag) {
            runDropFunc(drag);
        }
        onExited: function () {
            offsetType = 0;
        }
    }

    ShaderEffectSource {
        id: dragObject
        parent: root.targetView
        x: sourceItem.x
        y: sourceItem.y
        z: sourceItem.z + (root.active * 2) + (dropHandler.containsDrag * 1) + 1
        width: sourceItem.width
        height: sourceItem.height

        visible: dragHandler.active || dropHandler.containsDrag || dragObject_Transition.running
        sourceItem: root.targetItem

        Drag.source: root.targetItem
        YT.ElevationEffect {
            anchors.fill: parent
            radius: root.targetItem.radius
            elevation: YT.Theme.Elevation5
        }

        states: [
            State {
                name: 'droped'
                PropertyChanges {
                    dragObject.x: root.orientation == Qt.Horizontal ? dropHandler.dropOffset : root.targetItem.x
                    dragObject.y: root.orientation == Qt.Vertical ? dropHandler.dropOffset : root.targetItem.y
                }
            },
            State {
                name: 'draged'
                // dragObject.x: root.targetItem.x
                // dragObject.y: root.targetItem.y
                PropertyChanges {}
            },
            State {
                when: dropHandler.offsetType == Qt.TopEdge
                PropertyChanges {
                    dragObject.y: root.targetItem.y - root.targetItem.height * 0.5
                }
            },
            State {
                when: dropHandler.offsetType == Qt.LeftEdge
                PropertyChanges {
                    dragObject.x: root.targetItem.x - root.targetItem.width * 0.5
                }
            },
            State {
                when: dropHandler.offsetType == Qt.RightEdge
                PropertyChanges {
                    dragObject.x: root.targetItem.x + root.targetItem.width * 0.5
                }
            },
            State {
                when: dropHandler.offsetType == Qt.BottomEdge
                PropertyChanges {
                    dragObject.y: root.targetItem.y + root.targetItem.height * 0.5
                }
            }
        ]

        transitions: Transition {
            id: dragObject_Transition
            NumberAnimation {
                properties: "x,y"
                easing.type: Easing.InOutQuad
            }
        }
    }
    // Rectangle {
    //     z: +true
    //     parent: root.targetItem
    //     visible: root.drop_position !== 0
    //     radius: YT_Info.Radius
    //     color: YT_Info.ItemFocusColor

    //     property int size: +true
    //     property real margins: 0
    //     states: [
    //         State {
    //             name: "TopEdge"
    //             when: root.orientation === Qt.Vertical && root.drop_position & Qt.TopEdge
    //             PropertyChanges {
    //                 target: drop_item
    //                 x: margins
    //                 y: 0
    //                 width: parent.width - margins * 2
    //                 height: size
    //                 margins: parent.width * 0.15
    //             }
    //         },
    //         State {
    //             name: "LeftEdge"
    //             when: root.orientation === Qt.Horizontal && root.drop_position & Qt.LeftEdge
    //             PropertyChanges {
    //                 target: drop_item
    //                 x: 0
    //                 y: margins
    //                 width: size
    //                 height: parent.height - margins * 2
    //                 margins: parent.height * 0.15
    //             }
    //         },
    //         State {
    //             name: "RightEdge"
    //             when: root.orientation === Qt.Horizontal && root.drop_position & Qt.RightEdge
    //             PropertyChanges {
    //                 target: drop_item
    //                 x: parent.width - size
    //                 y: margins
    //                 width: size
    //                 height: parent.height - margins * 2
    //                 margins: parent.height * 0.15
    //             }
    //         },
    //         State {
    //             name: "BottomEdge"
    //             when: root.orientation === Qt.Vertical && root.drop_position & Qt.BottomEdge
    //             PropertyChanges {
    //                 target: drop_item
    //                 x: margins
    //                 y: parent.height - size
    //                 width: parent.width - margins * 2
    //                 height: size
    //                 margins: parent.width * 0.15
    //             }
    //         }
    //     ]
    // }
}
