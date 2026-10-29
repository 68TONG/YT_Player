pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import YT_Player
import YT_Material as YT_M

FileWidget {
    id: root

    readonly property list<Item> navigationWidget_Items: []

    Component.onCompleted: {
        YT.fileWidget = this;
        rootContent.addItem(listView);
        rootContent.addItem(itemView);
    }

    YT_M.SplitView {
        id: rootContent
        anchors.fill: parent
        handleSize: 6
        handleScale: 0.95
        spacing: 24
    }

    property alias listView: listView
    YT_TreeListView {
        id: listView
        padding: YT_M.Theme.Space75
        SplitView.preferredWidth: 100
        SplitView.fillHeight: true

        model: root.listModel
        delegate: root.listView_Delegate
        background: Rectangle {
            color: YT_M.Theme.SurfaceContainerHigh
            radius: YT_M.Theme.RadiusSmall
        }
    }

    readonly property Component listView_Delegate: YT_M.TreeViewExItem {
        id: listView_Delegate
        required property int index
        required property variant model
        required property string listName
        required property string listPath

        text: listName
        implicitWidth: ListView.view.width
        implicitHeight: topPadding + implicitContentHeight + bottomPadding

        TapHandler {
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onTapped: function (eventPoint, button) {
                if (button == Qt.LeftButton) {
                    WidgetModel.setPath(listView_Delegate.listPath);
                } else if (button == Qt.RightButton) {
                    listView_Menu.open_YT(listView_Delegate);
                } else {
                    return;
                }
            }
            onDoubleTapped: function (eventPoint, button) {
                listView.toggleExpanded(listView_Delegate.index);
            }
        }
        onIndicatorClicked: {
            listView.toggleExpanded(listView_Delegate.index);
        }
    }

    YT_M.Menu {
        id: listView_Menu
        m_isGetImplicitContentSize: true
        property variant modelHash
        onClosed: parent = null
        Component.onCompleted: {
            modelHash = {
                [FileListModel.LocalFileType]: {
                    "default": [listView_Menu_Add, listView_Menu_Remove, listView_Menu_CopyPath, listView_Menu_Rename, listView_Menu_FileOperation]
                },
                [FileListModel.CreatorInfoType]: {
                    "default": [listView_Menu_CopyPath]
                },
                [FileListModel.FileListInfoType]: {
                    "default": [listView_Menu_CopyPath],
                    "1": [listView_Menu_Add, listView_Menu_CopyPath],
                    "2": [listView_Menu_Remove, listView_Menu_CopyPath, listView_Menu_Rename]
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

        readonly property Item listView_Menu_Add: YT_M.MenuItem {
            id: listView_Menu_Add
            text: "添加"
            iconSource: YT_M.IconProvider.AddCircle
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
        readonly property Item listView_Menu_Remove: YT_M.MenuItem {
            id: listView_Menu_Remove
            text: "删除"
            iconSource: YT_M.IconProvider.Delete
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
        readonly property Item listView_Menu_Rename: YT_M.MenuItem {
            id: listView_Menu_Rename
            text: "重命名"
            iconSource: YT_M.IconProvider.Edit
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
        readonly property Item listView_Menu_CopyPath: YT_M.MenuItem {
            id: listView_Menu_CopyPath
            text: "复制路径"
            iconSource: YT_M.IconProvider.SubdirectoryArrowRight
            onTriggered: {
                console.log("listView_Menu_CopyPath");
            }
        }
        readonly property Menu listView_Menu_FileOperation: YT_M.Menu {
            id: listView_Menu_FileOperation
            title: "文件操作"
            // icon.source: YT_M.IconProvider.Link2
            YT_M.MenuItem {
                text: "复制"
                iconSource: YT_M.IconProvider.ContentCopy
                onTriggered: {}
            }
            YT_M.MenuItem {
                text: "粘贴"
                iconSource: YT_M.IconProvider.ContentPaste
                onTriggered: {}
            }
            YT_M.MenuItem {
                text: "剪切"
                iconSource: YT_M.IconProvider.ContentCut
                onTriggered: {}
            }
        }
    }

    property alias itemView: itemView
    Loader {
        id: itemView
        sourceComponent: fileCardDelegate_Com
        SplitView.fillWidth: true
        SplitView.fillHeight: true
    }

    YT_M.ButtonContainer {
        id: itemOperates
        readonly property bool m_focus: hovered || itemOperates_PropertyMenu.visible || itemOperates_SortMenu.visible
        parent: (itemView.item as Item)
        anchors.top: parent ? parent.top : undefined
        anchors.right: parent ? parent.right : undefined
        anchors.margins: YT_M.Theme.Space150

        implicitHeight: 32 + topPadding + bottomPadding
        verticalPadding: YT_M.Theme.Space75
        horizontalPadding: YT_M.Theme.Space100
        spacing: YT_M.Theme.Space50
        background: Rectangle {
            color: YT_M.Theme.SurfaceContainerHigh
            radius: YT_M.Theme.RadiusFull
            YT_M.ElevationEffect {
                anchors.fill: parent
                radius: parent.radius
                elevation: YT_M.Theme.Elevation2
            }
        }
        scale: m_focus ? 1.0 : 0.85
        Behavior on scale {
            NumberAnimation {
                easing: YT_M.Easing.SpringFastEx
                duration: YT_M.Easing.Medium3
            }
        }
        opacity: m_focus ? 0.92 : 0.80
        Behavior on opacity {
            NumberAnimation {
                easing: YT_M.Easing.SpringFastEx
                duration: YT_M.Easing.Medium3
            }
        }

        YT_M.Button {
            text: "属性"
            iconSource: YT_M.IconProvider.List
            styleType: YT_M.Button.Text
            onClicked: {
                itemOperates_PropertyMenu.openModel();
            }
        }
        YT_M.Button {
            text: "排序"
            iconSource: YT_M.IconProvider.Sort
            styleType: YT_M.Button.Text
            onClicked: {
                itemOperates_SortMenu.openModel();
            }
        }
        YT_M.Button {
            text: "视图"
            iconSource: YT_M.IconProvider.Dashboard
            styleType: YT_M.Button.Text
        }

        YT_M.Menu {
            id: itemOperates_PropertyMenu
            m_isGetImplicitContentSize: true
            implicitWidth: parent.width
            x: 0
            y: parent ? parent.height + YT_M.Theme.Space50 : 0
            model: FileModel.properties
            function syncPropertyOrder() {
                let propertyOrder = [];
                for (const property of FileModel.selectProperty) {
                    if (FileModel.properties.includes(property) && !propertyOrder.includes(property))
                        propertyOrder.push(property);
                }
                for (const property of FileModel.properties) {
                    if (!propertyOrder.includes(property))
                        propertyOrder.push(property);
                }

                let currentOrder = [];
                for (let index = 0; index < count; index++)
                    currentOrder.push((itemAt(index) as YT_M.Button).text);

                const currentCount = currentOrder.length;
                const targetCount = propertyOrder.length;
                // Keep a longest common subsequence in place to minimize single-item moves.
                let lcs = [];
                for (let i = 0; i <= currentCount; i++) {
                    lcs.push([]);
                    for (let j = 0; j <= targetCount; j++)
                        lcs[i].push(0);
                }
                for (let i = currentCount - 1; i >= 0; i--) {
                    for (let j = targetCount - 1; j >= 0; j--)
                        lcs[i][j] = currentOrder[i] == propertyOrder[j] ? 1 + lcs[i + 1][j + 1] : Math.max(lcs[i + 1][j], lcs[i][j + 1]);
                }

                let preservedProperties = [];
                let currentIndex = 0;
                let targetIndex = 0;
                while (currentIndex < currentCount && targetIndex < targetCount) {
                    if (currentOrder[currentIndex] == propertyOrder[targetIndex]) {
                        preservedProperties.push(propertyOrder[targetIndex]);
                        currentIndex++;
                        targetIndex++;
                    } else if (lcs[currentIndex + 1][targetIndex] >= lcs[currentIndex][targetIndex + 1]) {
                        currentIndex++;
                    } else {
                        targetIndex++;
                    }
                }

                for (let target = targetCount - 1; target >= 0; target--) {
                    const property = propertyOrder[target];
                    if (preservedProperties.includes(property))
                        continue;

                    let from = 0;
                    while (from < count && (itemAt(from) as YT_M.Button).text != property)
                        from++;
                    if (from == count)
                        continue;

                    let to = count - 1;
                    if (target + 1 < targetCount) {
                        let nextIndex = 0;
                        ;
                        while (nextIndex < count && (itemAt(nextIndex) as YT_M.Button).text != propertyOrder[target + 1])
                            nextIndex++;
                        if (nextIndex == count)
                            continue;
                        to = nextIndex - (from < nextIndex ? 1 : 0);
                    }
                    if (from != to) {
                        moveItem(from, to);
                    }
                }
            }

            Connections {
                target: itemOperates_PropertyMenu
                function onAboutToShow() {
                    itemOperates_PropertyMenu.syncPropertyOrder();
                }
            }
            Connections {
                target: FileModel
                function onSelectPropertyChanged() {
                    itemOperates_PropertyMenu.syncPropertyOrder();
                }
            }

            delegate: YT_M.Button {
                required property string modelData
                text: modelData
                iconSource: checked ? YT_M.IconProvider.CheckBox : YT_M.IconProvider.CheckBoxOutlineBlank
                width: parent ? parent.width : 0
                height: implicitHeight

                checked: FileModel.selectProperty.includes(modelData)
                spacing: YT_M.Theme.Space100
                verticalPadding: YT_M.Theme.Space100
                horizontalPadding: YT_M.Theme.Space150
                iconColor: YT_M.Theme.OnSurfaceVariant
                labelColor: YT_M.Theme.OnSurface
                containerColor: YT_M.Theme.Transparent
                radius: checked ? YT_M.Theme.RadiusMedium : YT_M.Theme.RadiusExtraSmall
                topLeftRadius: ObjectModel.index == 0 ? YT_M.Theme.RadiusMedium : radius
                topRightRadius: ObjectModel.index == 0 ? YT_M.Theme.RadiusMedium : radius
                bottomLeftRadius: ObjectModel.index == itemOperates_PropertyMenu.count - 1 ? YT_M.Theme.RadiusMedium : radius
                bottomRightRadius: ObjectModel.index == itemOperates_PropertyMenu.count - 1 ? YT_M.Theme.RadiusMedium : radius

                onClicked: {
                    if (checked == false) {
                        FileModel.selectProperty = [modelData].concat(FileModel.selectProperty);
                    } else {
                        FileModel.selectProperty = FileModel.selectProperty.filter(function (it) {
                            return it != modelData;
                        });
                    }
                }
            }
        }

        // YT_M.Menu {
        //     id: itemOperates_PropertyMenu
        //     m_isGetImplicitContentSize: true
        //     implicitWidth: parent.width
        //     x: 0
        //     y: parent ? parent.height + YT_M.Theme.Space50 : 0
        //     closePolicy: Popup.CloseOnPressOutside
        //     delegate: YT_M.ListViewItemDrag {
        //         required property string modelData
        //         width: parent ? parent.width : 0
        //         height: implicitHeight

        //         dragHandler.xAxis.enabled: false
        //         dragHandler.yAxis.enabled: true
        //         dropHandler.onEntered: function(evt) {
        //             itemOperates_PropertyMenu.moveItem(evt.source.ObjectModel.index, ObjectModel.index);
        //         }
        //         dropHandler.onDropped: function (evt) {
        //             let orderedProperties = [];
        //             for (let index = 0; index < itemOperates_PropertyMenu.count; index++) {
        //                 const property = itemOperates_PropertyMenu.itemAt(index).modelData;
        //                 if (FileModel.selectProperty.includes(property))
        //                     orderedProperties.push(property);
        //             }
        //             FileModel.selectProperty = orderedProperties;
        //             console.log(FileModel.selectProperty)
        //         }

        //         m.text: modelData
        //         m.iconSource: m.checked ? YT_M.IconProvider.CheckBox : YT_M.IconProvider.CheckBoxOutlineBlank
        //         m.checked: FileModel.selectProperty.includes(modelData)
        //         m.spacing: YT_M.Theme.Space100
        //         m.verticalPadding: YT_M.Theme.Space100
        //         m.horizontalPadding: YT_M.Theme.Space150
        //         m.iconColor: YT_M.Theme.OnSurfaceVariant
        //         m.labelColor: YT_M.Theme.OnSurface
        //         m.containerColor: YT_M.Theme.SurfaceContainerLow
        //         m.radius: m.checked || m.draging ? YT_M.Theme.RadiusMedium : YT_M.Theme.RadiusExtraSmall
        //         m.topLeftRadius: ObjectModel.index == 0 ? YT_M.Theme.RadiusMedium : m.radius
        //         m.topRightRadius: ObjectModel.index == 0 ? YT_M.Theme.RadiusMedium : m.radius
        //         m.bottomLeftRadius: ObjectModel.index == itemOperates_PropertyMenu.count - 1 ? YT_M.Theme.RadiusMedium : m.radius
        //         m.bottomRightRadius: ObjectModel.index == itemOperates_PropertyMenu.count - 1 ? YT_M.Theme.RadiusMedium : m.radius
        //         m.onClicked: {
        //             if (m.checked == false) {
        //                 FileModel.selectProperty = [modelData].concat(FileModel.selectProperty);
        //             } else {
        //                 FileModel.selectProperty = FileModel.selectProperty.filter(function (it) {
        //                     return it != modelData;
        //                 });
        //             }
        //         }
        //     }
        // }
        YT_M.Menu {
            id: itemOperates_SortMenu
            m_isGetImplicitContentSize: true
            implicitWidth: parent.width
            x: 0
            y: parent ? parent.height + YT_M.Theme.Space50 : 0
            model: FileModel.properties
            delegate: YT_M.MenuItem {
                required property string modelData
                text: modelData
                checked: modelData == FileModel.sortProperty
                iconSource: checked ? YT_M.IconProvider.Priority : null
                onClicked: {
                    FileModel.sortProperty = modelData;
                }
            }
        }
    }

    Component {
        id: fileCardDelegate_Com
        FileCardDelegate {
            menu: itemView_Menu
            model: FileModel
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

    YT_M.Menu {
        id: itemView_Menu
        m_isGetImplicitContentSize: true
        // function addOperationPaths() {
        //     root.operationPaths = [];
        //     const selectedIndexes = itemView.selectionModel.selectedIndexes;
        //     if (selectedIndexes.length === 0) {
        //         return false;
        //     }

        //     for (const model_index of selectedIndexes) {
        //         const index = model_index.row;
        //         root.operationPaths.push(itemModel.data(index, FileItemModel.FileInfoRole_Path));
        //     }
        //     return true;
        // }

        YT_M.Menu {
            m_isGetImplicitContentSize: true
            title: "添加到"
            YT_M.Menu {
                m_isGetImplicitContentSize: true
                title: "MusicWidget"
                model: YT.musicListModel
                delegate: YT_M.MenuItem {
                    required property string infoId
                    text: infoId
                    // onTriggered: {
                    //     // if (itemView_Menu.addOperationPaths() === false) {
                    //     //     return messageDialog.open_YT("未选择文件");
                    //     // }
                    //     root.operationType = "MusicWidget";
                    //     let data = itemModel.dataIndex(itemView_Menu.parent.index, FileItemModel.FilePath_Role);
                    //     YT.musicListModel.addFileInfo(infoId, [data]);
                    // }
                }
            }

            YT_M.Menu {
                title: "WallpaperWidget"
                enabled: false
            }
        }

        Action {
            text: "重命名"
            // onTriggered: {
            //     const index = itemView.selectionModel.currentIndex.row;
            //     if (index < 0)
            //         return console.log("itemView_Menu 重命名 index < 0");

            //     const target = itemView.view.itemAtIndex(index);
            //     root.operationPaths = [itemModel.data(index, FileItemModel.FileInfoRole_Path)];
            //     root.operationType = text;

            //     editDelegate.text = itemModel.data(index, FileItemModel.FileInfoRole_Name);
            //     editDelegate.open_YT(target, acceptedFunction, null);
            // }
            // function acceptedFunction(data, user_data) {
            //     root.rename(itemModel.currentDir + "/" + data);
            // }
        }

        Action {
            text: "粘贴"
            // enabled: root.operationType === "剪切" || root.operationType === "复制"
            // onTriggered: {
            //     if (root.operationType === "剪切") {
            //         root.move(itemModel.currentDir);
            //         root.operationPaths = null;
            //         root.operationType = null;
            //     } else if (root.operationType === "复制") {
            //         root.copy(itemModel.currentDir);
            //     }
            // }
        }

        Action {
            text: "剪切"
            // onTriggered: {
            //     if (itemView_Menu.addOperationPaths() === false) {
            //         return messageDialog.open_YT("未选择文件");
            //     }
            //     root.operationType = text;
            // }
        }

        Action {
            text: "复制"
            // onTriggered: {
            //     if (itemView_Menu.addOperationPaths() === false) {
            //         return messageDialog.open_YT("未选择文件");
            //     }
            //     root.operationType = text;
            // }
        }

        Action {
            text: "删除"
            // onTriggered: {
            //     if (itemView_Menu.addOperationPaths() === false) {
            //         return messageDialog.open_YT("未选择文件");
            //     }
            //     root.operationType = text;
            //     messageDialog.open_YT("确定删除", function () {
            //         root.remove();
            //     });
            // }
        }

        YT_M.Menu {
            title: "新建"
            Action {
                text: "文件夹"
                // onClicked: {
                //     root.createFolder(itemModel.currentDir);
                // }
            }
            Action {
                text: "文件"
                // onClicked: {
                //     root.create(itemModel.currentDir);
                // }
            }
        }
    }

    readonly property YT_EditDelegate editDelegate: YT_EditDelegate {
        id: editDelegate
    }

    readonly property YT_MessageDialog messageDialog: YT_MessageDialog {
        id: messageDialog
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
