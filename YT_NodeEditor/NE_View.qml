import QtQuick
import YT_Player

Item {
    id: root

    property alias contentItem: contentItem
    default property alias contentItem_children: contentItem.children
    Item {
        id: contentItem
        width: parent.width
        height: parent.height
    }

    QtObject {
        id: selectionModel
        property list<Item> model: []
        function select(cur_model, modifiers) {
            const pre_model = model.slice();
            const addedItems = cur_model.filter(it => (pre_model.includes(it) === false));
            const removedItems = cur_model.filter(it => (pre_model.includes(it) === true));
            const allItems = pre_model.concat(addedItems);

            if (modifiers === Qt.NoModifier) {
                model = cur_model.slice();
            } else if (modifiers === Qt.ShiftModifier) {
                model = allItems;
            } else if (modifiers === Qt.ControlModifier) {
                model = allItems.filter(it => !removedItems.includes(it));
            }

            for (const it of allItems) {
                if ("selected" in it)
                    it.selected = model.includes(it);
            }
        }
    }

    TapHandler {
        acceptedButtons: Qt.LeftButton
        grabPermissions: PointerHandler.TakeOverForbidden
        onTapped: function (eventPoint, button) {
            const selectionPos = YT.mapFromGlobal(contentItem, root, eventPoint.position);
            const child = contentItem.childAt(selectionPos.x, selectionPos.y);
            if (child && child.parent === contentItem) {
                selectionModel.select([child], point.modifiers);
            } else {
                selectionModel.select([], Qt.NoModifier);
            }
        }
    }

    TapHandler {
        acceptedButtons: Qt.RightButton
        grabPermissions: PointerHandler.TakeOverForbidden
        onTapped: function (eventPoint, button) {
            viewMenu.popup();
        }
    }

    DragHandler {
        id: selectionHandler
        target: null
        dragThreshold: 0
        acceptedButtons: Qt.LeftButton
        grabPermissions: PointerHandler.TakeOverForbidden
        property int modifiers: Qt.NoModifier
        property point pre_pos: Qt.point(0, 0)
        property bool is_drag_selection: false
        onActiveChanged: {
            if (active === true) {
                pre_pos = root.mapToItem(contentItem, centroid.pressPosition);
                const at = contentItem.childAt(pre_pos.x, pre_pos.y);
                is_drag_selection = (at === null);
                modifiers = centroid.modifiers;

                if (at && selectionModel.model.includes(at) === false) {
                    selectionModel.select([at], Qt.NoModifier);
                }
            } else if (active === false && is_drag_selection === true) {
                var ret = [];
                const children = contentItem.children;
                const rect = root.mapToItem(contentItem, Qt.rect(selectionRect.x, selectionRect.y, selectionRect.width, selectionRect.height));
                for (var i = 0; i < children.length; i++) {
                    const child = children[i];
                    var childRect = Qt.rect(child.x, child.y, child.width, child.height);
                    if (YT_Func.intersects(childRect, rect) === false)
                        continue;
                    ret.push(child);
                }
                selectionModel.select(ret, modifiers);
            }
        }

        onCentroidChanged: {
            if (is_drag_selection === true || active === false)
                return;
            const cur_pos = root.mapToItem(contentItem, centroid.position);
            const pos = Qt.point(cur_pos.x - pre_pos.x, cur_pos.y - pre_pos.y);
            pre_pos = cur_pos;

            for (var it of selectionModel.model) {
                it.x = pos.x + it.x;
                it.y = pos.y + it.y;
            }
        }

        readonly property Item selectionRect: Rectangle {
            parent: root
            color: Qt.rgba(0, 0, 0, 0)
            border.color: YT_Info.FontColor
            visible: selectionHandler.active && selectionHandler.is_drag_selection

            x: Math.min(pressPos.x, movePos.x)
            y: Math.min(pressPos.y, movePos.y)
            width: Math.abs(movePos.x - pressPos.x)
            height: Math.abs(movePos.y - pressPos.y)

            readonly property point pressPos: selectionHandler.centroid.pressPosition
            readonly property point movePos: selectionHandler.centroid.position
        }
    }

    DragHandler {
        target: contentItem
        dragThreshold: 0
        snapMode: DragHandler.NoSnap
        acceptedButtons: Qt.MiddleButton
        grabPermissions: PointerHandler.TakeOverForbidden
    }

    WheelHandler {
        target: contentItem
        property: "scale"
        acceptedModifiers: Qt.NoModifier
        grabPermissions: PointerHandler.TakeOverForbidden
    }

    // WheelHandler {
    //     target: contentItem
    //     property: "scale"
    //     acceptedModifiers: Qt.ControlModifier
    //     targetTransformAroundCursor: false
    // }

    readonly property alias nodeLine: nodeLine
    NE_NodeLine {
        id: nodeLine
        visible: false
        enabled: false
        anchors.fill: parent
    }

    readonly property alias dragInfo: dragInfo
    Item {
        id: dragInfo
        Drag.source: this
        Drag.active: nodeLine.visible

        property bool typeIO_: false
        property NE_Data dataNE_: null
        property NE_Data dataNE_Reset: null
        property NE_Data dataNE_Connect: null
    }

    YT_Menu {
        id: viewMenu
        model: dataModel.count
        delegate: YT_Menu.YT_MenuItem {
            required property int index
            readonly property QtObject model: viewMenu.dataModel.get(index)

            text: model.name
            onTriggered: {
                model._component.createObject(root.contentItem);
            }
        }
        readonly property ObjectModel dataModel: ObjectModel {
            QtObject {
                property string name: "输入文件夹"
                property Component _component: Component {
                    NE_FolderInput {}
                }
            }
            QtObject {
                property string name: "输入媒体"
                property Component _component: Component {
                    NE_MediaInput {}
                }
            }
            QtObject {
                property string name: "输出媒体"
                property Component _component: Component {
                    NE_MediaOutput {}
                }
            }
            QtObject {
                property string name: "编辑文本"
                property Component _component: Component {
                    NE_Group {
                        NE_TextInputArea {
                            inputNode: NE_Node {}
                            outputNode: NE_Node {}
                        }
                    }
                }
            }
            QtObject {
                property string name: "音频流"
                property Component _component: Component {
                    NE_AudioStream {}
                }
            }
        }
    }
}
