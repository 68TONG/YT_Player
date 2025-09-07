import QtQuick
import QtQuick.Controls.Basic

import YT_Player

Item {
    id: itemDelegate
    ListView.delayRemove: draging

    required property int index
    required property YT_DelegateInfo info_YT

    property bool draging: false
    property bool selected: false
    property int drop_position: 0

    implicitWidth: contentItem.width
    implicitHeight: contentItem.height

    readonly property Item item_container: Item {
        parent: itemDelegate
        implicitWidth: contentItem.width
        implicitHeight: contentItem.height
    }

    default property Item contentItem: null
    onContentItemChanged: {
        if(contentItem === null) return
        contentItem.parent = item_container
    }

    component YT_DelegateInfo: QtObject {
        required property Item view
        required property var model
    }

    component YT_DropDelegate: DropArea {
        anchors.fill: parent
        required property Item item_delegate
        readonly property Rectangle dropItem: Rectangle {
            z: +true
            parent: item_delegate

            visible: item_delegate.drop_position !== 0
            radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
            color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)

            property int item_width: 2
            property real padding: 0
            states: [
                State {
                    name: "TopEdge"
                    when: info_YT.view.orientation === Qt.Vertical && item_delegate.drop_position & Qt.TopEdge
                    PropertyChanges {
                        target: dropItem
                        x: padding
                        y: 0
                        width: parent.width - padding * 2
                        height: item_width
                        padding: parent.width * 0.15
                    }
                },
                State {
                    name: "LeftEdge"
                    when: info_YT.view.orientation === Qt.Horizontal && item_delegate.drop_position & Qt.LeftEdge
                    PropertyChanges {
                        target: dropItem
                        x: 0
                        y: padding
                        width: item_width
                        height: parent.height - padding * 2
                        padding: parent.height * 0.15
                    }
                },
                State {
                    name: "RightEdge"
                    when: info_YT.view.orientation === Qt.Horizontal && item_delegate.drop_position & Qt.RightEdge
                    PropertyChanges {
                        target: dropItem
                        x: parent.width
                        y: padding
                        width: item_width
                        height: parent.height - padding * 2
                        padding: parent.height * 0.15
                    }
                },
                State {
                    name: "BottomEdge"
                    when: info_YT.view.orientation === Qt.Vertical && item_delegate.drop_position & Qt.BottomEdge
                    PropertyChanges {
                        target: dropItem
                        x: padding
                        y: parent.height
                        width: parent.width - padding * 2
                        height: item_width
                        padding: parent.width * 0.15
                    }
                }
            ]
            // transitions: Transition {
            //     NumberAnimation { properties: "x,y"; easing.type: Easing.InOutQuad }
            // }
        }
        Component.onCompleted: {
            item_delegate.Drag.source = item_delegate
            item_delegate.draging = Qt.binding(function () {return item_delegate.Drag.active})
        }

        function startDrag () {
            item_delegate.Drag.active = true
            item_delegate.Drag.startDrag()
        }
        function updatePosition (pos) {
            var drop_position = 0
            var this_center = Qt.point(this.width / 2, this.height / 2)
            if(pos.y < this_center.y) drop_position = drop_position | Qt.TopEdge
            if(pos.x < this_center.x) drop_position = drop_position | Qt.LeftEdge
            if(pos.x >= this_center.x) drop_position = drop_position | Qt.RightEdge
            if(pos.y >= this_center.y) drop_position = drop_position | Qt.BottomEdge
            item_delegate.drop_position = drop_position
        }

        onEntered: function (drag) {
            updatePosition(Qt.point(drag.x, drag.y))
        }
        onPositionChanged: function (drag) {
            updatePosition(Qt.point(drag.x, drag.y))
        }
        onDropped: function (drop) {
            var orientation = info_YT.view.orientation
            var source_index = drop.source.index
            var target_index = index

            if(source_index > target_index) {
                if(orientation === Qt.Vertical && item_delegate.drop_position & Qt.BottomEdge) {
                    target_index = target_index + true
                }
                if(orientation === Qt.Horizontal && item_delegate.drop_position & Qt.RightEdge) {
                    target_index = target_index + true
                }
            }
            if(source_index < target_index) {
                if(orientation === Qt.Vertical && item_delegate.drop_position & Qt.TopEdge) {
                    target_index = target_index - true
                }
                if(orientation === Qt.Horizontal && item_delegate.drop_position & Qt.LeftEdge) {
                    target_index = target_index - true
                }
            }
            if(target_index >= 0 && target_index < info_YT.model.count && target_index !== source_index)
                info_YT.model.move(source_index, target_index, +true)
            item_delegate.drop_position = 0
        }
        onExited: function () {
            item_delegate.drop_position = 0
        }
    }

    component YT_DragDelegate: MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        required property Item item_delegate
        required property Item item_container
        readonly property NumberAnimation item_container_animation: NumberAnimation {
            target: item_container
            properties: "x,y"
            duration: 200
            to: 0
        }
        Component.onCompleted: {
            item_delegate.z = Qt.binding(function () {return item_container_animation.running ? +true : 0})
            item_delegate.draging = Qt.binding(function () {return drag.active})
        }

        drag.axis: info_YT.view.orientation === Qt.Vertical ? Drag.YAxis : Drag.XAxis
        drag.target: item_container
        onPressed: {
            var pos = info_YT.view.mapFromItem(item_container, 0, 0)
            item_container.parent = info_YT.view
            item_container.x = pos.x
            item_container.y = pos.y
        }
        onReleased: {
            var pos = info_YT.view.mapToItem(item_delegate, item_container.x, item_container.y)
            item_container.parent = item_delegate
            item_container.x = pos.x
            item_container.y = pos.y
            item_container_animation.start()
        }
        onPositionChanged: {
            var pos = info_YT.view.mapFromItem(item_container, 0, 0)
            if(info_YT.view.contains(pos) === false) return
            updatePosition(pos)
        }
        function updatePosition (pos) {
            var at_index = info_YT.view.indexAt(pos.x + info_YT.view.contentX, pos.y + info_YT.view.contentY)
            // console.log(index, at_index)

            if(at_index < 0 || at_index >= info_YT.model.count || index === at_index) return
            info_YT.model.move(index, at_index, +true)
        }
    }
}
