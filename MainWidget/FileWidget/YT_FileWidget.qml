import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

import YT_Player

FileWidget {
    id: root

    curModel: itemModel
    Component.onCompleted: {
        YT.fileWidget = this
    }

    YT_SplitView {
        id: rootContent
        anchors.fill: parent
    }

    FileItemModel {
        id: itemModel
        currentDir: "C:/Users/YI_TONG/Music"
    }
    FileListModel {
        id: listModel
    }
    ListModel {
        id: itemHeader_Model
        ListElement { headerIndex: FileItemModel.FileInfoRole_Name }
        ListElement { headerIndex: FileItemModel.FileInfoRole_Size }
        ListElement { headerIndex: FileItemModel.FileInfoRole_Type }

        function appendHeaderIndex(index) {
            append({"headerIndex": index})
        }
        function removeHeaderIndex(index) {
            if(count === +true) return
            for (var i = 0; i < count; i++) {
                if (get(i).headerIndex === index) {
                    return remove(i, +true)
                }
            }
        }
        function contains(index) {
            for (var i = 0; i < count; i++) {
                if(get(i).headerIndex === index) {
                    return true;
                }
            }
            return false;
        }
    }

    readonly property YT_TreeView listView: YT_TreeView {
        id: listView
        parent: rootContent.contentItem
        view.pointerNavigationEnabled: false

        padding: 7
        SplitView.preferredWidth: 100
        SplitView.fillHeight: true

        model: listModel
        delegate: listView_Delegate
    }

    readonly property YT_ListView itemView: YT_ListView {
        id: itemView
        parent: rootContent.contentItem
        view.headerPositioning: ListView.OverlayHeader

        padding: 7
        spacing: 3
        SplitView.fillWidth: true
        SplitView.fillHeight: true

        model: itemModel
        header: itemHeader
        delegate: itemView_Delegate

        TapHandler {
            parent: itemView.view
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onTapped: function (eventPoint, button) {
                if (button === Qt.LeftButton) {
                    itemView.selectionModel.clear()
                } else if(button === Qt.RightButton) {
                    itemView_Menu.popup()
                } else {
                    return
                }
            }
        }
    }

    readonly property YT_SplitView itemHeader: YT_SplitView {
        id: itemHeader
        parent: itemView
        z: +true * 2; height: 30

        contentItem: ListView {
            orientation: parent.orientation
            Rectangle {
                z: -true
                anchors.fill: parent
                color: YT_ConfigureInfo.getData(YT_ConfigureInfo.BackgroundColor)
            }
        }

        Repeater {
            id: itemHeader_Repeater
            model: itemHeader_Model
            delegate: itemHeader_Delegate
        }

        readonly property YT_ItemDelegate.YT_DelegateInfo info_YT: YT_ItemDelegate.YT_DelegateInfo {
            view: itemHeader.contentItem
            model: itemHeader_Model
        }
    }

    readonly property YT_EditDelegate editDelegate: YT_EditDelegate {
        id: editDelegate
    }

    readonly property YT_MessageDialog messageDialog: YT_MessageDialog {
        id: messageDialog
    }

    readonly property YT_Menu listView_Menu: YT_Menu {
        id: listView_Menu
        Action {
            text: "粘贴"
            onTriggered: {
                if (root.operationType === "剪切") {
                    root.move(itemModel.currentDir)
                    root.operationPaths = null
                    root.operationType = null
                } else if (root.operationType === "复制") {
                    root.copy(itemModel.currentDir)
                }
            }
        }
        Action {
            text: "剪切"
            onTriggered: {
                const target = listView.followBackground.followItem
                root.operationPaths = [target.filePath]
                root.operationType = text
            }
        }
        Action {
            text: "复制"
            onTriggered: {
                const target = listView.followBackground.followItem
                root.operationPaths = [target.filePath]
                root.operationType = text
            }
        }
        Action {
            text: "删除"
            onTriggered: {
                const target = listView.followBackground.followItem
                root.operationPaths = [target.filePath]
                root.operationType = text
                messageDialog.open_YT("确定删除", function () {root.remove()})
            }
        }
        onClosed: listView.followBackground.followItem = null
    }

    readonly property YT_Menu itemView_Menu: YT_Menu {
        id: itemView_Menu

        function addOperationPaths() {
            root.operationPaths = []
            const selectedIndexes = itemView.selectionModel.selectedIndexes
            if (selectedIndexes.length === 0) {
                return false
            }

            for (const model_index of selectedIndexes) {
                const index = model_index.row
                root.operationPaths.push(itemModel.data(index, FileItemModel.FileInfoRole_Path))
            }
            return true
        }

        YT_Menu {
            title: "添加到"
            YT_Menu {
                title: "MusicWidget"
                enabled: count

                model: YT.musicListModel
                delegate: YT_Menu.YT_MenuItem {
                    required property int infoID
                    required property string name

                    text: name
                    onTriggered: {
                        if (itemView_Menu.addOperationPaths() === false) {
                            return messageDialog.open_YT("未选择文件")
                        }
                        root.operationType = "MusicWidget"
                        YT.musicListModel.addItemInfo(infoID, root.operationPaths)
                    }
                }
            }

            YT_Menu {
                title: "WallpaperWidget"
                enabled: count
            }
        }

        Action {
            text: "重命名"
            onTriggered: {
                const index = itemView.selectionModel.currentIndex.row
                if (index < 0) return console.log("itemView_Menu 重命名 index < 0")

                const target = itemView.view.itemAtIndex(index)
                root.operationPaths = [itemModel.data(index, FileItemModel.FileInfoRole_Path)]
                root.operationType = text

                editDelegate.text = itemModel.data(index, FileItemModel.FileInfoRole_Name)
                editDelegate.open_YT(target, acceptedFunction, null)
            }
            function acceptedFunction(data) {
                root.rename(itemModel.currentDir + "/" + data["acceptData"])
            }
        }

        Action {
            text: "粘贴"
            enabled: root.operationType === "剪切" || root.operationType === "复制"
            onTriggered: {
                if (root.operationType === "剪切") {
                    root.move(itemModel.currentDir)
                    root.operationPaths = null
                    root.operationType = null
                } else if (root.operationType === "复制") {
                    root.copy(itemModel.currentDir)
                }
            }
        }

        Action {
            text: "剪切"
            onTriggered: {
                if (itemView_Menu.addOperationPaths() === false) {
                    return messageDialog.open_YT("未选择文件")
                }
                root.operationType = text
            }
        }

        Action {
            text: "复制"
            onTriggered: {
                if (itemView_Menu.addOperationPaths() === false) {
                    return messageDialog.open_YT("未选择文件")
                }
                root.operationType = text
            }
        }

        Action {
            text: "删除"
            onTriggered: {
                if (itemView_Menu.addOperationPaths() === false) {
                    return messageDialog.open_YT("未选择文件")
                }
                root.operationType = text
                messageDialog.open_YT("确定删除", function () {root.remove()})
            }
        }

        YT_Menu {
            title: "新建"
            YT_Menu.YT_MenuItem {
                text: "文件夹"
                onClicked: {
                    root.createFolder(itemModel.currentDir)
                }
            }
            YT_Menu.YT_MenuItem {
                text: "文件"
                onClicked: {
                    root.create(itemModel.currentDir)
                }
            }
        }
    }
    readonly property YT_PopupList itemHeader_Menu: YT_PopupList {
        id: itemHeader_Menu
        x: 0
        y: parent ? parent.height + true * 2 : 0
        implicitWidth: parent ? parent.width : 0
        implicitHeight: contentItem.implicitHeight

        model: itemModel.headerIndex_List
        delegate: YT_PopupList.YT_PopupList_Delegate {
            property bool selected: itemHeader_Model.count && itemHeader_Model.contains(index)
            modelData: itemModel.getHeaderName(model.modelData)
            implicitWidth: parent.width
            indicator: YT_PopupList.YT_SelectedIndicator { }

            onClicked: {
                itemHeader_Menu.close()
                if (selected) {
                    itemHeader_Model.removeHeaderIndex(index)
                } else {
                    itemHeader_Model.appendHeaderIndex(index)
                }
            }
        }
    }

    readonly property Component listView_Delegate: YT_TreeView.YT_TreeViewDelegate {
        id: listView_Delegate
        required property var fileName
        required property var filePath

        contentItem: Text {
            text: listView_Delegate.fileName
            color: hovered ? "#FFD700" : YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: function (mouse) {
                if(mouse.button === Qt.LeftButton) {
                    itemModel.currentDir = listView_Delegate.filePath
                } else if(mouse.button === Qt.RightButton) {
                    listView.followBackground.followItem = listView_Delegate
                    listView_Menu.popup()
                } else {
                    return
                }
            }
            onDoubleClicked: function (mouse) {
                listView.view.toggleExpanded(index)
            }
        }
    }

    readonly property Component itemView_Delegate: YT_ItemDelegate {
        id: itemView_Delegate
        info_YT: itemView.delegateInfo

        ItemDelegate {
            implicitWidth: info_YT.view.contentItem.width; implicitHeight: 30 + bottomInset
            padding: 0
            bottomInset: 3
            bottomPadding: bottomInset

            indicator: null
            background: Rectangle {
                visible: opacity !== 0
                opacity: selected || draging
                radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
                color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)

                Behavior on opacity {
                    NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                }
            }
            contentItem: Item {
                Repeater {
                    model: itemHeader_Repeater.count
                    delegate: Text {
                        required property int index
                        readonly property Item headerItem: itemHeader_Repeater.count ? itemHeader_Repeater.itemAt(index) : null
                        x: headerItem ? headerItem.x : x
                        width: headerItem ? headerItem.width : width
                        anchors.verticalCenter: parent.verticalCenter

                        clip: true
                        text: itemModel.data(itemView_Delegate.index, headerItem.headerIndex) ?? null
                        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)

                        leftPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: function (mouse) {
                    itemView.view.focus = true
                    var selectionModel = itemView.selectionModel
                    if (mouse.button === Qt.LeftButton) {
                        selectionModel.selectItem(index, mouse.modifiers)
                    } else if (mouse.button === Qt.RightButton) {
                        if (selectionModel.selectedIndexes.length <= +true)
                            selectionModel.selectItem(index, mouse.modifiers)
                        itemView_Menu.popup(itemView_Delegate)
                    } else {
                        return
                    }
                }
                onDoubleClicked: function (mouse) {
                    if (mouse.button !== Qt.LeftButton) return
                    if (itemModel.data(index, FileItemModel.FileInfoRole_Type) === "Folder") {
                        itemModel.currentDir = itemModel.data(index, FileItemModel.FileInfoRole_Path)
                    }
                }
            }
        }
    }

    readonly property Component itemHeader_Delegate: YT_ItemDelegate {
        id: itemHeader_Delegate

        info_YT: itemHeader.info_YT
        required property int headerIndex

        SplitView.preferredWidth: 100
        SplitView.fillHeight: true

        YT_Button {
            implicitWidth: itemHeader_Delegate.width
            implicitHeight: itemHeader_Delegate.height

            modelData: itemModel.getHeaderName(itemHeader_Delegate.headerIndex)
            contentItem: YT_Button.LoadText {}

            YT_ItemDelegate.YT_DropDelegate {
                item_delegate: itemHeader_Delegate
                DragHandler {
                    target: null
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onActiveChanged: if(active === true) parent.startDrag()
                }
            }
            TapHandler {
                acceptedButtons: Qt.LeftButton
                onTapped: function (eventPoint, button) {
                    console.log("LeftButton")
                }
            }
            TapHandler {
                acceptedButtons: Qt.RightButton
                onTapped: function (eventPoint, button) {
                    itemHeader_Menu.open_YT(itemHeader_Delegate)
                }
            }
        }
    }
}
