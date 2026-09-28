import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import HuskarUI.Basic
import YT_Player

FileWidget {
    id: root

    curListInfoPath: "C:/Users/YI_TONG/Music"
    readonly property list<Item> navigationWidget_Items: [controlView]
    readonly property Item controlView: HusIconButton {
        width: parent ? parent.width : 0
        height: implicitHeight
        type: HusButton.Type_Primary
        iconSource: HusIcon.SearchOutlined
    }

    Component.onCompleted: {
        YT.fileWidget = this;
    }

    YT_SplitView {
        id: rootContent
        anchors.fill: parent
    }

    readonly property YT_TreeListView listView: YT_TreeListView {
        id: listView
        parent: rootContent.contentItem

        padding: YT_Info.Margin
        SplitView.preferredWidth: 100
        SplitView.fillHeight: true

        model: listModel
        delegate: listView_Delegate
        followBackground.followItem: listView.view.currentItem
        states: [
            State {
                name: ""
                PropertyChanges {
                    target: listView.followBackground
                    followItem: listView.view.currentItem
                }
            },
            State {
                name: "MenuOpening || EditOpening"
                when: listView_Menu.visible
                PropertyChanges {
                    target: listView.followBackground
                    followItem: listView_Menu.parent
                }
            }
        ]
    }

    readonly property Component listView_Delegate: YT_TreeViewDelegate {
        id: listView_Delegate
        required property variant model
        required property string listName
        required property string listPath

        contentItem: YT_Text {
            text: listView_Delegate.listName
            elide: Text.ElideRight
        }

        onHoveredChanged: {
            if (hovered) {
                listView.view.currentIndex = listView_Delegate.index;
            } else if (listView.view.currentIndex == listView_Delegate.index) {
                listView.view.currentIndex = -true;
            }
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: function (mouse) {
                listView.view.focus = true;
                if (mouse.button == Qt.LeftButton) {
                    root.curListInfoPath = listView_Delegate.listPath;
                } else if (mouse.button == Qt.RightButton) {
                    listView_Menu.open_YT(listView_Delegate);
                } else {
                    return;
                }
            }
            onDoubleClicked: function (mouse) {
                listView.toggleExpanded(index);
            }
        }
    }

    readonly property YT_Menu listView_Menu: YT_Menu {
        id: listView_Menu
        property variant modelHash
        Component.onCompleted: {
            modelHash = {
                [FileListModel.LocalFileType]: {
                    "default": [listView_Menu_Add, listView_Menu_Remove, listView_Menu_Rename, listView_Menu_CopyPath, listView_Menu_FileOperation]
                },
                [FileListModel.CreatorInfoType]: {
                    "default": [listView_Menu_CopyPath]
                },
                [FileListModel.FileListInfoType]: {
                    "default": [listView_Menu_CopyPath],
                    "1": [listView_Menu_Add, listView_Menu_CopyPath],
                    "2": [listView_Menu_Rename, listView_Menu_Remove, listView_Menu_CopyPath]
                }
            };
        }
        function open_YT(target) {
            const type = target.model.listType;
            const depth = target.depth;

            const model_list = modelHash[type];
            const model_ = (model_list && model_list[depth] != undefined) ? model_list[depth] : model_list["default"];
            // console.log("type:", type, "depth:", depth, "model_:", model_);

            while (listView_Menu.count > 0) {
                if (listView_Menu.menuAt(0)) {
                    listView_Menu.takeMenu(0);
                } else if (listView_Menu.itemAt(0)) {
                    listView_Menu.takeItem(0);
                } else {
                    console.log("YT_FileWidget listView_Menu open_YT while error");
                    return;
                }
            }
            for (let it of model_) {
                if (it instanceof Item) {
                    listView_Menu.addItem(it);
                } else if (it instanceof Menu) {
                    listView_Menu.addMenu(it);
                } else {
                    console.log("YT_FileWidget listView_Menu open_YT instanceof error", it);
                }
            }
            popup(target);
        }
        function getTargetModelIndex(target) {
            const p_model = listView.proxyModel;
            const p_model_index = p_model.index(target.index, 0);
            const s_model_index = p_model.mapToSource(p_model_index);
            return s_model_index;
        }

        readonly property Item listView_Menu_Add: YT_Menu.YT_MenuItem {
            id: listView_Menu_Add
            text: "添加"
            onTriggered: {
                const target = listView_Menu.parent;
                if (target == null)
                    return console.log("YT_FileWidget listView_Menu 添加 target == null");

                const acceptedFunction = function (data, user_data) {
                    listModel.menuAdd(user_data, data);
                };

                editDelegate.text = "";
                editDelegate.placeholderText = "请输入名称";
                editDelegate.userData = listView_Menu.getTargetModelIndex(target);
                editDelegate.open_YT(target, acceptedFunction, null);
            }
        }
        readonly property Item listView_Menu_Remove: YT_Menu.YT_MenuItem {
            id: listView_Menu_Remove
            text: "删除"
            onTriggered: {
                const target = listView_Menu.parent;
                if (target == null)
                    return console.log("YT_FileWidget listView_Menu 删除 target == null");

                const acceptedFunction = function () {
                    const index = listView_Menu.getTargetModelIndex(target);
                    listModel.menuRemove(index);
                };

                messageDialog.open_YT("确定删除", acceptedFunction);
            }
        }
        readonly property Item listView_Menu_Rename: YT_Menu.YT_MenuItem {
            id: listView_Menu_Rename
            text: "重命名"
            onTriggered: {
                const target = listView_Menu.parent;
                if (target == null)
                    return console.log("YT_FileWidget listView_Menu 重命名 target == null");

                const acceptedFunction = function (data, user_data) {};

                editDelegate.text = "";
                editDelegate.placeholderText = "请输入名称";
                editDelegate.userData = listView_Menu.getTargetModelIndex(target);
                editDelegate.open_YT(target, acceptedFunction, null);
            }
        }
        readonly property Item listView_Menu_CopyPath: YT_Menu.YT_MenuItem {
            id: listView_Menu_CopyPath
            text: "复制路径"
            onTriggered: {
                console.log("listView_Menu_CopyPath");
            }
        }
        readonly property YT_Menu listView_Menu_FileOperation: YT_Menu {
            id: listView_Menu_FileOperation
            title: "文件操作"
            YT_Menu.YT_MenuItem {
                text: "复制"
                onTriggered: {}
            }
            YT_Menu.YT_MenuItem {
                text: "粘贴"
                onTriggered: {}
            }
            YT_Menu.YT_MenuItem {
                text: "剪切"
                onTriggered: {}
            }
        }
        onClosed: parent = null
    }

    readonly property YT_GridView itemView: YT_GridView {
        id: itemView
        parent: rootContent.contentItem
        readonly property int delegateSplitCount: view.width / 200
        readonly property int delegateSize: view.width / delegateSplitCount

        padding: YT_Info.Margin
        view.currentIndex: 0
        view.cellWidth: view.currentItem ? view.currentItem.implicitWidth : 0
        view.cellHeight: view.currentItem ? view.currentItem.implicitHeight : 0

        SplitView.fillWidth: true
        SplitView.fillHeight: true

        model: itemModel
        delegate: itemDelegate

        TapHandler {
            parent: itemView.view
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onTapped: function (eventPoint, button) {
                if (button === Qt.LeftButton) {
                    itemView.selectionModel.clear();
                } else if (button === Qt.RightButton) {
                    itemView_Menu.popup();
                } else {
                    return;
                }
            }
        }
    }

    // readonly property YT_FlowView itemView: YT_FlowView {
    //     id: itemView
    //     parent: rootContent.contentItem
    //     readonly property int delegateSplitCount: view.width / 200
    //     readonly property int delegateSize: (view.width - delegateSplitCount * view.spacing) / delegateSplitCount

    //     padding: YT_Info.Margin
    //     view.spacing: YT_Info.Spacing
    //     SplitView.fillWidth: true
    //     SplitView.fillHeight: true

    //     model: itemModel
    //     delegate: itemDelegate

    //     TapHandler {
    //         parent: itemView.view
    //         acceptedButtons: Qt.LeftButton | Qt.RightButton
    //         onTapped: function (eventPoint, button) {
    //             if (button === Qt.LeftButton) {
    //                 itemView.selectionModel.clear();
    //             } else if (button === Qt.RightButton) {
    //                 itemView_Menu.popup();
    //             } else {
    //                 return;
    //             }
    //         }
    //     }
    // }

    // readonly property YT_ListView itemView: YT_ListView {
    //     id: itemView
    //     parent: rootContent.contentItem
    //     view.headerPositioning: ListView.OverlayHeader

    //     padding: 7
    //     spacing: 3
    //     SplitView.fillWidth: true
    //     SplitView.fillHeight: true

    //     model: itemModel
    //     header: itemHeader
    //     delegate: itemDelegate

    //     TapHandler {
    //         parent: itemView.view
    //         acceptedButtons: Qt.LeftButton | Qt.RightButton
    //         onTapped: function (eventPoint, button) {
    //             if (button === Qt.LeftButton) {
    //                 itemView.selectionModel.clear();
    //             } else if (button === Qt.RightButton) {
    //                 itemView_Menu.popup();
    //             } else {
    //                 return;
    //             }
    //         }
    //     }
    // }

    // readonly property YT_SplitView itemHeader: YT_SplitView {
    //     id: itemHeader
    //     parent: itemView
    //     z: +true * 2
    //     height: 30

    //     Repeater {
    //         id: itemHeader_Repeater
    //         model: itemHeader_Model
    //         delegate: itemHeader_Delegate
    //     }

    //     readonly property YT_ItemDelegate.YT_DelegateInfo info_YT: YT_ItemDelegate.YT_DelegateInfo {
    //         view: itemHeader.contentItem
    //         model: itemHeader_Model
    //     }
    // }

    readonly property Component itemDelegate: AbstractButton {
        id: itemDelegate
        required property int index
        required property string infoTitle

        implicitWidth: itemView.delegateSize
        implicitHeight: implicitContentHeight + topPadding + bottomPadding
        topInset: YT_Info.MarginSmall
        leftInset: YT_Info.MarginSmall
        rightInset: YT_Info.MarginSmall
        bottomInset: YT_Info.MarginSmall

        padding: YT_Info.Margin * 2
        topPadding: indicator.height + padding

        indicator: YT_RoundImage {
            source: root.getFileInfo_CoverPath(itemModel.dataIndex(itemDelegate.index, FileItemModel.FilePath_Role))
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
            x: itemDelegate.leftInset
            y: itemDelegate.topInset
            width: parent.width - itemDelegate.leftInset - itemDelegate.rightInset
            height: parent.width
            padding: +true
            radius: YT_Info.Radius
            // opacity: 0.618
        }
        background: YT_Rectangle {
            border.color: itemDelegate.hovered ? YT_Info.ItemFocusColor : YT_Info.ItemColor
        }
        contentItem: ColumnLayout {
            spacing: YT_Info.Spacing
            YT_Text {
                text: itemDelegate.infoTitle
                font.pixelSize: 16
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
            Repeater {
                model: itemModel.selectTagModel
                delegate: Loader {
                    required property var modelData
                    sourceComponent: modelData != FileItemModel.InfoTag_Role ? textTagView : listTagView
                    Layout.fillWidth: true
                    // Layout.preferredHeight: item.implciitHeight
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: function (mouse) {
                itemView.view.focus = true;
                if (mouse.button === Qt.LeftButton) {} else if (mouse.button === Qt.RightButton) {
                    itemView_Menu.popup(itemDelegate);
                } else {
                    return;
                }
            }
            onDoubleClicked: function (mouse) {
                if (mouse.button !== Qt.LeftButton)
                    return;
                if (itemModel.dataIndex(itemDelegate.index, FileItemModel.FileType_Role) == "Folder") {
                    itemModel.currentDir = itemModel.dataIndex(itemDelegate.index, FileItemModel.FilePath_Role);
                }
            }
        }

        readonly property Component textTagView: Column {
            id: textTagView
            readonly property int infoRole: parent.modelData
            readonly property string infoRoleStr: itemModel.roleNames(infoRole)
            readonly property variant infoRoleData: itemModel.dataIndex(itemDelegate.index, infoRole)

            spacing: YT_Info.SpacingSmall
            YT_Text {
                text: textTagView.infoRoleStr
                // color: Qt.lighter(YT_Info.FontColor, 0.85)
            }
            YT_Text {
                text: textTagView.infoRoleData || "<No Data>"
            }
        }

        readonly property Component listTagView: YT_PathWidget_TagView {
            id: listTagView
            readonly property int infoRole: parent.modelData
            readonly property string infoRoleStr: itemModel.roleNames(infoRole)
            readonly property variant infoRoleData: itemModel.dataIndex(itemDelegate.index, infoRole) || ["<No Data>"]

            spacing: YT_Info.SpacingSmall
            modelData: infoRoleStr
            model: infoRoleData
            delegate: YT_PathWidget_TagItem {
                required property string modelData
                pathModel: root.pathModel
                pathData: modelData
            }
        }
    }

    // readonly property Component itemDelegate: YT_ItemDelegate {
    //     id: itemDelegate
    //     info_YT: itemView.delegateInfo

    //     ItemDelegate {
    //         implicitWidth: info_YT.view.contentItem.width
    //         implicitHeight: 30 + bottomInset
    //         padding: 0
    //         bottomInset: 3
    //         bottomPadding: bottomInset

    //         indicator: null
    //         background: Rectangle {
    //             visible: opacity !== 0
    //             opacity: selected || draging
    //             radius: YT_Info.Radius
    //             color: YT_Info.ItemFocusColor

    //             Behavior on opacity {
    //                 NumberAnimation {
    //                     duration: 200
    //                     easing.type: Easing.InOutQuad
    //                 }
    //             }
    //         }
    //         contentItem: Item {
    //             Repeater {
    //                 model: itemHeader_Repeater.count
    //                 delegate: Text {
    //                     required property int index
    //                     readonly property Item headerItem: itemHeader_Repeater.count ? itemHeader_Repeater.itemAt(index) : null
    //                     x: headerItem ? headerItem.x : x
    //                     width: headerItem ? headerItem.width : width
    //                     anchors.verticalCenter: parent.verticalCenter

    //                     clip: true
    //                     text: itemModel.data(itemDelegate.index, headerItem.headerIndex) ?? null
    //                     color: YT_Info.FontColor

    //                     leftPadding: YT_Info.Margin
    //                     verticalAlignment: Text.AlignVCenter
    //                 }
    //             }
    //         }

    //         MouseArea {
    //             anchors.fill: parent
    //             acceptedButtons: Qt.LeftButton | Qt.RightButton
    //             onClicked: function (mouse) {
    //                 itemView.view.focus = true;
    //                 var selectionModel = itemView.selectionModel;
    //                 if (mouse.button === Qt.LeftButton) {
    //                     selectionModel.selectItem(index, mouse.modifiers);
    //                 } else if (mouse.button === Qt.RightButton) {
    //                     if (selectionModel.selectedIndexes.length <= +true)
    //                         selectionModel.selectItem(index, mouse.modifiers);
    //                     itemView_Menu.popup(itemDelegate);
    //                 } else {
    //                     return;
    //                 }
    //             }
    //             onDoubleClicked: function (mouse) {
    //                 if (mouse.button !== Qt.LeftButton)
    //                     return;
    //                 if (itemModel.data(index, FileItemModel.FileInfoRole_Type) === "Folder") {
    //                     itemModel.currentDir = itemModel.data(index, FileItemModel.FileInfoRole_Path);
    //                 }
    //             }
    //         }
    //     }
    // }

    readonly property YT_EditDelegate editDelegate: YT_EditDelegate {
        id: editDelegate
    }

    readonly property YT_MessageDialog messageDialog: YT_MessageDialog {
        id: messageDialog
    }

    readonly property YT_Menu itemView_Menu: YT_Menu {
        id: itemView_Menu

        function addOperationPaths() {
            root.operationPaths = [];
            const selectedIndexes = itemView.selectionModel.selectedIndexes;
            if (selectedIndexes.length === 0) {
                return false;
            }

            for (const model_index of selectedIndexes) {
                const index = model_index.row;
                root.operationPaths.push(itemModel.data(index, FileItemModel.FileInfoRole_Path));
            }
            return true;
        }

        YT_Menu {
            title: "添加到"
            YT_Menu {
                title: "MusicWidget"
                model: YT.musicListModel
                delegate: YT_Menu.YT_MenuItem {
                    required property string infoId
                    text: infoId
                    onTriggered: {
                        // if (itemView_Menu.addOperationPaths() === false) {
                        //     return messageDialog.open_YT("未选择文件");
                        // }
                        root.operationType = "MusicWidget";
                        let data = itemModel.dataIndex(itemView_Menu.parent.index, FileItemModel.FilePath_Role);
                        YT.musicListModel.addFileInfo(infoId, [data]);
                    }
                }
            }

            YT_Menu {
                title: "WallpaperWidget"
            }
        }

        Action {
            text: "重命名"
            onTriggered: {
                const index = itemView.selectionModel.currentIndex.row;
                if (index < 0)
                    return console.log("itemView_Menu 重命名 index < 0");

                const target = itemView.view.itemAtIndex(index);
                root.operationPaths = [itemModel.data(index, FileItemModel.FileInfoRole_Path)];
                root.operationType = text;

                editDelegate.text = itemModel.data(index, FileItemModel.FileInfoRole_Name);
                editDelegate.open_YT(target, acceptedFunction, null);
            }
            function acceptedFunction(data, user_data) {
                root.rename(itemModel.currentDir + "/" + data);
            }
        }

        Action {
            text: "粘贴"
            enabled: root.operationType === "剪切" || root.operationType === "复制"
            onTriggered: {
                if (root.operationType === "剪切") {
                    root.move(itemModel.currentDir);
                    root.operationPaths = null;
                    root.operationType = null;
                } else if (root.operationType === "复制") {
                    root.copy(itemModel.currentDir);
                }
            }
        }

        Action {
            text: "剪切"
            onTriggered: {
                if (itemView_Menu.addOperationPaths() === false) {
                    return messageDialog.open_YT("未选择文件");
                }
                root.operationType = text;
            }
        }

        Action {
            text: "复制"
            onTriggered: {
                if (itemView_Menu.addOperationPaths() === false) {
                    return messageDialog.open_YT("未选择文件");
                }
                root.operationType = text;
            }
        }

        Action {
            text: "删除"
            onTriggered: {
                if (itemView_Menu.addOperationPaths() === false) {
                    return messageDialog.open_YT("未选择文件");
                }
                root.operationType = text;
                messageDialog.open_YT("确定删除", function () {
                    root.remove();
                });
            }
        }

        YT_Menu {
            title: "新建"
            YT_Menu.YT_MenuItem {
                text: "文件夹"
                onClicked: {
                    root.createFolder(itemModel.currentDir);
                }
            }
            YT_Menu.YT_MenuItem {
                text: "文件"
                onClicked: {
                    root.create(itemModel.currentDir);
                }
            }
        }
    }

    // readonly property YT_PopupList itemHeader_Menu: YT_PopupList {
    //     id: itemHeader_Menu

    //     model: itemModel.headerIndex_List
    //     delegate: YT_Button {
    //         required property int index
    //         required property var model
    //         property bool selected: itemHeader_Model.count && itemHeader_Model.contains(index)

    //         indicator: YT_Button.LoadSelectIndicator {}
    //         background: null
    //         contentItem: YT_Button.LoadText {}

    //         modelData: itemModel.getHeaderName(model.modelData)
    //         implicitWidth: parent.width

    //         onHoveredChanged: {
    //             if (hovered)
    //                 ListView.view.currentIndex = index;
    //         }
    //         onClicked: {
    //             itemHeader_Menu.close();
    //             if (selected) {
    //                 itemHeader_Model.removeHeaderIndex(index);
    //             } else {
    //                 itemHeader_Model.appendHeaderIndex(index);
    //             }
    //         }
    //     }
    // }

    // readonly property Component itemHeader_Delegate: YT_ItemDelegate {
    //     id: itemHeader_Delegate

    //     info_YT: itemHeader.info_YT
    //     required property int headerIndex

    //     SplitView.preferredWidth: 100
    //     SplitView.fillHeight: true

    //     YT_Button {
    //         implicitWidth: itemHeader_Delegate.width
    //         implicitHeight: itemHeader_Delegate.height

    //         modelData: itemModel.getHeaderName(itemHeader_Delegate.headerIndex)
    //         contentItem: YT_Button.LoadText {}

    //         YT_ItemDelegate.YT_DropDelegate {
    //             target_item: itemHeader_Delegate
    //             DragHandler {
    //                 target: null
    //                 acceptedButtons: Qt.LeftButton | Qt.RightButton
    //                 onActiveChanged: if (active === true)
    //                     parent.startDrag()
    //             }
    //         }
    //         TapHandler {
    //             acceptedButtons: Qt.LeftButton
    //             onTapped: function (eventPoint, button) {
    //                 console.log("LeftButton");
    //             }
    //         }
    //         TapHandler {
    //             acceptedButtons: Qt.RightButton
    //             onTapped: function (eventPoint, button) {
    //                 itemHeader_Menu.open_YT(itemHeader_Delegate);
    //             }
    //         }
    //     }
    // }
}
