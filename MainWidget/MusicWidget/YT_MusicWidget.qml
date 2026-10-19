pragma ComponentBehavior: Bound
import QtQuick
import QtQml.Models
import QtQuick.Layouts
import QtQuick.Controls.Basic
import YT_Player
import HuskarUI.Basic
import YT_Material as YT_M

MusicWidget {
    id: root

    readonly property list<Item> navigationWidget_Items: [listView, updateTagView]
    Component.onCompleted: {
        YT.musicWidget = this;
        YT.musicFileModel = fileModel;
        YT.musicListModel = listModel;
    }

    GridLayout {
        id: rootContent
        anchors.fill: parent
        rowSpacing: 0
        columnSpacing: 0
    }

    property alias listView: listView
    YT_NavigationWidget.M_View {
        id: listView

        opacity: closeHorizontal == false
        currentIndex: root.listModel.index(root.curListInfoID)
        followBackground.followItem: {
            if (listView_Menu.visible) {
                return listView_Menu.parent;
            } else if (dragingItem) {
                return dragingItem;
            } else if (hoveredItem) {
                return hoveredItem;
            }
            return currentItem;
        }

        model: root.listModel
        delegate: root.listDelegate

        Connections {
            target: listView_Menu
            function onVisibleChanged() {
                YT.navigationWidget.setExpanded = listView_Menu.visible;
            }
        }
    }

    readonly property Component listDelegate: YT_NavigationWidget.M_ViewItem {
        id: listDelegate
        required property string infoId
        m.text: infoId

        dragEnabled: true
        dropEnabled: true
        dropHandler.onEntered: function (evt) {
            root.listModel.move(evt.source.index, index);
        }

        m.onClickedEx: function (eventPoint, button, modifiers) {
            if (button == Qt.LeftButton) {
                if (listView.currentIndex == listDelegate.index) {
                    listView.closeVertical = !listView.closeVertical;
                    return;
                }
                root.curListInfoID = listDelegate.infoId;
            } else if (button == Qt.RightButton) {
                listView_Menu.popup(listDelegate);
            }
        }
    }

    property alias fileView: fileView
    YT_M.ListView {
        id: fileView
        clip: true
        parent: rootContent
        spacing: YT_M.Theme.Space25
        Layout.row: 1
        Layout.column: 0
        Layout.fillWidth: true
        Layout.fillHeight: true

        model: root.fileModel
        // delegate: Item {
        //     width: ListView.view.width
        //     height: 50
        //     YT_M.DebugRectangle {}
        // }
        delegate: root.fileDelegate
        currentIndex: fileSelectionModel.currentIndex.row

        Connections {
            target: root.fileModel
            function onViewPositionChanged(index) {
                fileView.positionViewAtIndex(index, ListView.Beginning);
            }
        }

        Connections {
            target: fileView
            function onMovementEnded() {
                const view = fileView;
                const model = root.fileModel;
                const pos = view.mapToItem(view.contentItem, view.topMargin, view.leftMargin);
                let index = view.indexAt(pos.x, pos.y);
                if (index < 0)
                    index = view.indexAt(pos.x, pos.y + view.spacing);
                if (index < 0)
                    return;

                model.setFooterPosition(index);
            }
        }
    }

    ItemSelectionModel {
        id: fileSelectionModel
        model: fileView.model

        function m_select(index = -1, modifiers = Qt.NoModifier) {
            let model_index = model.index(index, 0);
            if (model_index.valid == false) {
                return clearSelection();
            }

            if (modifiers == Qt.NoModifier) {
                setCurrentIndex(model_index, ItemSelectionModel.ClearAndSelect);
            } else if (modifiers & Qt.ShiftModifier) {
                let cur_index = currentIndex;
                if (cur_index.valid == false)
                    cur_index = model_index;

                let start = Math.min(cur_index.row, model_index.row);
                let end = Math.max(cur_index.row, model_index.row);

                let command = (modifiers & Qt.ControlModifier) ? ItemSelectionModel.Toggle : ItemSelectionModel.Select;
                for (let i = start; i <= end; ++i) {
                    select(model.index(i, 0), command);
                }
                setCurrentIndex(model_index, ItemSelectionModel.Current);
            } else if (modifiers & Qt.ControlModifier) {
                select(model_index, ItemSelectionModel.Toggle);
                setCurrentIndex(model_index, ItemSelectionModel.Current);
            }
        }
    }

    YT_M.SplitView {
        id: fileHeader
        parent: rootContent

        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true
        Layout.preferredHeight: 32

        property real viewSize: width - fileHeader_Id.width - leftPadding - rightPadding - spacing * (count - 1)
        Connections {
            target: root.fileModel
            function onItemStretchFactorChanged() {
                fileHeader.viewSize = fileHeader.width - fileHeader_Id.width - fileHeader.leftPadding - fileHeader.rightPadding - fileHeader.spacing * (fileHeader.count - 1);
            }
        }

        YT_M.Button {
            id: fileHeader_Id
            text: "Id"
            styleType: YT_M.Button.Text
            verticalPadding: 0
            horizontalPadding: 0
            contentItem: Text {
                text: fileHeader_Id.text
                font: fileHeader_Id.font
                color: fileHeader_Id.labelColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            SplitView.maximumWidth: implicitHeight
            SplitView.minimumWidth: implicitHeight
            onClicked: root.fileModel.sortInfoIds(text)
        }

        YT_M.Button {
            id: fileHeader_Frist
            text: {
                if (hoverPosition == Qt.LeftEdge) {
                    return root.fileModel.titleTag;
                } else if (hoverPosition == Qt.RightEdge) {
                    return root.fileModel.selectTag[0];
                }
                return root.fileModel.titleTag + " & " + root.fileModel.selectTag[0];
            }

            styleType: YT_M.Button.Text
            SplitView.preferredWidth: root.fileModel.itemStretchFactor[0] * fileHeader.viewSize
            SplitView.minimumWidth: implicitWidth

            TapHandler {
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onTapped: function (eventPoint, button) {
                    if (button == Qt.LeftButton) {
                        root.fileModel.sortInfoIds(fileHeader_Frist.text);
                    } else if (button == Qt.RightButton) {
                        fileHeader_Menu.m_open(fileHeader_Frist);
                    }
                }
            }

            property int hoverPosition: 0
            HoverHandler {
                onHoveredChanged: {
                    if (hovered == false) {
                        fileHeader_Frist.hoverPosition = 0;
                    }
                }
                onPointChanged: {
                    fileHeader_Frist.hoverPosition = point.position.x < fileHeader_Frist.width / 2 ? Qt.LeftEdge : Qt.RightEdge;
                }
            }
        }

        Repeater {
            id: fileHeader_List
            model: root.fileModel.selectTag.slice(1)
            delegate: YT_M.Button {
                id: fileHeader_ListItem
                required property int index
                required property string modelData
                text: modelData
                styleType: YT_M.Button.Text
                SplitView.preferredWidth: root.fileModel.itemStretchFactor[index + 1] * fileHeader.viewSize
                SplitView.minimumWidth: implicitWidth

                TapHandler {
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onTapped: function (eventPoint, button) {
                        if (button == Qt.LeftButton) {
                            root.fileModel.sortInfoIds(fileHeader_ListItem.text);
                        } else if (button == Qt.RightButton) {
                            fileHeader_Menu.m_open(fileHeader_ListItem);
                        }
                    }
                }

                YT_DelegateDropMove {
                    orientation: fileHeader.orientation
                    DragHandler {
                        target: null
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onActiveChanged: {
                            if (active == true)
                                parent.startDrag();
                        }
                    }
                    moveFunc: function (source_index, target_index) {
                        source_index = source_index + 1;
                        target_index = target_index + 1;
                        let list = root.fileModel.selectTag.slice();
                        let data = list.splice(source_index, 1)[0];
                        list.splice(target_index, 0, data);
                        root.fileModel.selectTag = list;
                    }
                }
            }
        }
    }

    // YT_M.ListView {
    //     id: fileScrollBar
    // }

    YT_M.Button {
        id: fileFooter
        parent: fileView
        anchors.bottom: parent.bottom
        anchors.bottomMargin: YT_M.Theme.Space75
        anchors.horizontalCenter: parent.horizontalCenter
        implicitWidth: 32
        implicitHeight: 32
        font: YT_M.Theme.FontBodySmall
        styleType: YT_M.Button.Tonal
        opacity: 0.618

        contentItem: Text {
            text: fileFooter.text
            font: fileFooter.font
            color: fileFooter.labelColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: {
            fileFooterView.open();
        }

        Connections {
            target: root.fileModel
            function onFooterPositionChanged(index) {
                fileFooter.text = root.fileModel.footerModel[index];
            }
        }
    }

    YT_M.BottomSheet {
        id: fileFooterView
        dragMargin: 0
        implicitWidth: 640
        implicitHeight: 480
        contentItem: ListView {
            clip: true
            model: fileFooterModel
            spacing: YT_M.Theme.Space75
        }
    }

    ObjectModel {
        id: fileFooterModel

        property list<string> deModel: root.fileModel.footerModel.slice(0, 28)
        property list<string> exModel: root.fileModel.footerModel.slice(28)

        M_FileFooterItem {
            text: "Sort Header"
            model: fileFooterView.visible ? root.fileModel.headerModel : null
            exSetCheckSortTag: true
            clickedFunc: function (data) {
                root.fileModel.sortInfoIds(data);
            }
        }
        M_FileFooterItem {
            text: "A-Z Index"
            model: fileFooterView.visible ? fileFooterModel.deModel : null
        }
        M_FileFooterItem {
            text: "Ex Index"
            model: fileFooterView.visible ? fileFooterModel.exModel : null
        }
        M_FileFooterItem {
            text: "Second Index " + fileFooter.text
            model: fileFooterView.visible ? root.fileModel.footerModelEx : null
            clickedFunc: function (data) {
                root.fileModel.setViewPositionEx(fileFooter.text, data);
            }
        }
    }

    component M_FileFooterItem: YT_M.FoldableView {
        id: m_FileFooterItem
        required property list<string> model
        property variant clickedFunc: function (data) {
            root.fileModel.setViewPosition(data);
        }

        property bool exSetCheckSortTag: false

        width: parent ? parent.width : 0
        foldableView: Flow {
            width: (32 + spacing) * 14 - spacing
            anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
            spacing: YT_M.Theme.Space100
            Repeater {
                model: m_FileFooterItem.model
                delegate: YT_M.Button {
                    id: m_FileFooterItem_IndexBut
                    required property string modelData
                    implicitWidth: modelData.length == 1 ? 32 : implicitContentWidth + leftPadding + rightPadding
                    implicitHeight: 32
                    text: modelData
                    font: YT_M.Theme.FontBodySmall
                    styleType: YT_M.Button.Outlined

                    contentItem: Text {
                        text: m_FileFooterItem_IndexBut.text
                        font: m_FileFooterItem_IndexBut.font
                        color: m_FileFooterItem_IndexBut.labelColor
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        (m_FileFooterItem.clickedFunc as Function)(modelData);
                    }
                    Component.onCompleted: {
                        if (m_FileFooterItem.exSetCheckSortTag) {
                            checked = Qt.binding(function () {
                                return modelData == root.fileModel.sortTag;
                            });
                        }
                    }
                }
            }
        }
    }

    readonly property Component fileDelegate: YT_M.ListViewItem {
        id: fileDelegate
        required property int index
        required property int infoId
        required property string titleTag
        required property string musicPath

        checked: fileSelectionModel.hasSelection ? fileSelectionModel.isSelected(root.fileModel.index(index, 0)) : false
        implicitWidth: ListView.view.width
        implicitHeight: 50
        verticalPadding: 0
        horizontalPadding: 0

        contentItem: GridLayout {
            rowSpacing: 0
            columnSpacing: fileHeader.spacing
        }

        Text {
            parent: fileDelegate.contentItem
            Layout.column: 0
            Layout.preferredWidth: fileHeader_Id.width
            Layout.preferredHeight: fileHeader_Id.height

            font: fileHeader_Id.font
            text: fileDelegate.index + true
            color: fileDelegate.infoId == root.playInfoId ? YT_M.Theme.Primary : fileDelegate.labelColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        GridLayout {
            parent: fileDelegate.contentItem
            rowSpacing: YT_M.Theme.Space0
            columnSpacing: YT_M.Theme.Space50

            Layout.column: 1
            Layout.preferredWidth: fileHeader_Frist.width
            Layout.fillHeight: true

            YT_RoundImage {
                asynchronous: true
                radius: YT_M.Theme.RadiusSmall
                padding: YT_M.Theme.Space50
                // opacity: 0.7
                source: YT_Func.getFileCoverPath(fileDelegate.musicPath)
                imageSource.sourceSize: Qt.size(50, 50)

                Layout.row: 0
                Layout.rowSpan: 2
                Layout.column: 0
                Layout.preferredWidth: height
                Layout.fillHeight: true
            }

            YT_PathWidget_TagItem {
                pathModel: root.pathModel
                pathData: fileDelegate.titleTag
                pathType: "Title"

                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.verticalStretchFactor: 6
            }

            YT_PathWidget_TagContainer {
                readonly property string selectTagOne: root.fileModel.selectTag[0]
                opacity: 0.7
                model: root.fileModel.dataTag(fileDelegate.index, selectTagOne)
                delegate: YT_PathWidget_TagItem {
                    required property string modelData
                    width: Math.min(implicitWidth, parent.width / parent.model.length) // qmllint disable missing-property
                    font: YT_M.Theme.FontBodySmall

                    pathModel: root.pathModel
                    pathData: modelData
                    pathType: root.fileModel.selectTag[0]
                }

                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.verticalStretchFactor: 4
            }
        }

        Instantiator {
            asynchronous: true
            model: fileHeader_List.model
            delegate: YT_PathWidget_TagContainer {
                id: fileDelegate_TagContainer
                required property int index
                required property string modelData
                readonly property Item modelItem: fileHeader_List.count ? fileHeader_List.itemAt(index) : null

                model: root.fileModel.dataTag(fileDelegate.index, modelData)
                delegate: YT_PathWidget_TagItem {
                    required property string modelData
                    width: Math.min(implicitWidth, parent.width / fileDelegate_TagContainer.model.length)
                    pathData: modelData
                    pathType: fileDelegate_TagContainer.modelData
                    pathModel: root.pathModel
                }

                leftPadding: fileHeader_Frist.leftPadding
                Layout.column: 2 + index
                Layout.preferredWidth: modelItem ? modelItem.width : 0
            }
            onObjectAdded: function (index, object) {
                object.parent = fileDelegate.contentItem;
            }
            onObjectRemoved: function (index, object) {
                object.parent = null;
            }
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton
            acceptedModifiers: Qt.NoModifier
            onTapped: {
                fileSelectionModel.m_select(fileDelegate.index, acceptedModifiers);
            }
            onDoubleTapped: root.playMusic(fileDelegate.index)
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton
            acceptedModifiers: Qt.ControlModifier
            onTapped: {
                fileSelectionModel.m_select(fileDelegate.index, acceptedModifiers);
            }
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton
            acceptedModifiers: Qt.ShiftModifier
            onTapped: {
                fileSelectionModel.m_select(fileDelegate.index, acceptedModifiers);
            }
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton
            acceptedModifiers: Qt.ControlModifier & Qt.ShiftModifier
            onTapped: {
                fileSelectionModel.m_select(fileDelegate.index, acceptedModifiers);
            }
        }

        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: {
                if (fileSelectionModel.selectedIndexes.length <= 1)
                    fileSelectionModel.m_select(fileDelegate.index);
                fileView_Menu.popup(fileDelegate);
            }
        }
    }

    YT_M.Menu {
        id: listView_Menu
        m_isGetImplicitContentSize: true

        Action {
            text: "添加列表"
            onTriggered: {
                const target = listView_Menu.mTarget;
                if (target === null)
                    return console.log("listView_Menu 添加列表 target === null");
                editDelegate.text = "";
                editDelegate.placeholderText = "请输入名称";
                editDelegate.open_YT(target, acceptedFunction, null);
            }

            function acceptedFunction(data, user_data) {
                root.listModel.addInfo(data);
            }
        }
        Action {
            text: "删除列表"
            onTriggered: {
                const target = listView_Menu.mTarget;
                if (target === null)
                    return console.log("listView_Menu 删除列表 target === null");
                root.listModel.eraseInfo(target.infoId);
            }
        }
        Action {
            text: "重命名列表"
            onTriggered: {
                const target = listView_Menu.mTarget;
                if (target === null)
                    return console.log("listView_Menu 重命名列表 target === null");
                editDelegate.text = target.infoId;
                editDelegate.userData = target.infoId;
                editDelegate.open_YT(target, acceptedFunction, null);
            }
            function acceptedFunction(data, user_data) {
                root.listModel.setInfoName(user_data, data);
            }
        }
        onClosed: parent = null
    }

    YT_M.Menu {
        id: fileView_Menu
        m_isGetImplicitContentSize: true

        onClosed: parent = null
        Action {
            text: "播放音乐"
            onTriggered: {
                const index = fileSelectionModel.currentIndex.row;
                root.playMusic(index);
            }
        }
        Action {
            text: "稍后播放"
            onTriggered: {
                let operationDatas = [];
                const selectedIndexes = fileSelectionModel.selectedIndexes;
                if (selectedIndexes.length === 0) {
                    return console.log("稍后播放 未选择文件");
                }

                for (let i = selectedIndexes.length - 1; i >= 0; --i) {
                    const model_index = selectedIndexes[i];
                    operationDatas.push(root.fileModel.data(model_index, MusicFileModel.InfoId_Role));
                }
                root.addPlayMusic(operationDatas);
            }
        }
        YT_M.Menu {
            m_isGetImplicitContentSize: true
            title: "添加到"
            model: root.listModel
            delegate: YT_M.MenuItem {
                required property string infoId
                text: infoId
                verticalPadding: YT_M.Theme.Space75
                horizontalPadding: YT_M.Theme.Space125
                onTriggered: {
                    let operationPaths = [];
                    const selectedIndexes = fileSelectionModel.selectedIndexes;
                    if (selectedIndexes.length === 0) {
                        return console.log("添加到 ", infoId, " 未选择文件");
                    }

                    for (const model_index of selectedIndexes) {
                        operationPaths.push(root.fileModel.data(model_index, MusicFileModel.MusicPath_Role));
                    }
                    root.listModel.addFileInfo(infoId, operationPaths);
                }
            }
        }
        YT_M.MenuItem {
            id: fileView_Menu_StarLevelItem
            text: "评分"
            spacing: YT_M.Theme.Space75
            verticalPadding: YT_M.Theme.Space75
            horizontalPadding: YT_M.Theme.Space125

            indicator: HusRate {
                anchors.right: fileView_Menu_StarLevelItem.right
                anchors.rightMargin: fileView_Menu_StarLevelItem.rightPadding
                anchors.verticalCenter: fileView_Menu_StarLevelItem.verticalCenter
                anchors.verticalCenterOffset: +true

                padding: 0
                background: null
                iconSize: fontMetrics.boundingRect("A").height
                readonly property FontMetrics fontMetrics: FontMetrics {
                    font: fileView_Menu_StarLevelItem.font
                }
                function updateInitValue() {
                    const selectedIndexes = fileSelectionModel.selectedIndexes;
                    if (selectedIndexes.length != 1) {
                        initValue = 0;
                        return;
                    }
                    const model_index = selectedIndexes[0];
                    initValue = root.fileModel.data(model_index, MusicFileModel.StarLevel_Role);
                }
                Component.onCompleted: {
                    fileView_Menu.onAboutToShow.connect(updateInitValue);
                }
                onDone: function (data) {
                    let operationDatas = [];
                    const selectedIndexes = fileSelectionModel.selectedIndexes;
                    if (selectedIndexes.length === 0) {
                        return console.log("评分 未选择文件");
                    }

                    for (let i = selectedIndexes.length - 1; i >= 0; --i) {
                        const model_index = selectedIndexes[i];
                        operationDatas.push(root.fileModel.data(model_index, MusicFileModel.InfoId_Role));
                    }

                    root.updateMusicInfo_StarLevel(data, operationDatas);
                    fileView_Menu.close();
                }
            }

            contentItem: Text {
                text: fileView_Menu_StarLevelItem.text
                font: fileView_Menu_StarLevelItem.font
                color: fileView_Menu_StarLevelItem.labelColor
                rightPadding: fileView_Menu_StarLevelItem.indicator.width + fileView_Menu_StarLevelItem.spacing
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
                z: -true
                anchors.fill: parent
            }
        }
    }

    YT_M.Menu {
        id: fileHeader_Menu
        m_isGetImplicitContentSize: true
        x: 0
        y: parent ? parent.height + YT_M.Theme.Space50 : 0
        implicitWidth: parent ? parent.width : 0
        model: root.fileModel.headerModel
        delegate: YT_M.MenuItem {
            id: fileHeader_Menu_Delegate
            required property int index
            required property string modelData
            readonly property bool m_checked: root.fileModel.selectTag.includes(modelData) || modelData == root.fileModel.titleTag
            text: modelData
            enabled: modelData != root.fileModel.sortTag
            spacing: YT_M.Theme.Space75
            verticalPadding: YT_M.Theme.Space75
            horizontalPadding: YT_M.Theme.Space125

            iconSize: 18
            iconSource: m_checked ? YT_M.IconProvider.Check : ""
            onTriggered: {
                const model = root.fileModel;
                if (modelData == model.titleTag || modelData == model.sortTag) {
                    console.log("modelData == fileModel.titleTag || modelData == fileModel.sortTag");
                    return;
                }

                if (fileHeader_Menu.parent == fileHeader_Frist && model.sortTag != model.selectTag[0]) {
                    let list = model.selectTag.filter(v => modelData != v);
                    if (model.selectTag[0] != modelData)
                        list.splice(0, 0, modelData);
                    model.selectTag = list;
                } else if (m_checked == false) {
                    model.selectTag.push(modelData);
                } else {
                    model.selectTag = model.selectTag.filter(function (it) {
                        return it != modelData;
                    });
                }
                fileHeader_Menu.close();
            }
        }

        YT_M.MenuItem {
            text: "调整列大小"
            verticalPadding: YT_M.Theme.Space75
            horizontalPadding: YT_M.Theme.Space125
        }

        YT_M.MenuItem {
            text: "调整列所有大小"
            verticalPadding: YT_M.Theme.Space75
            horizontalPadding: YT_M.Theme.Space125
        }

        YT_M.MenuSeparatorGroup {}
    }

    property alias updateTagView: updateTagView
    YT_NavigationWidget.M_View {
        id: updateTagView
        opacity: closeHorizontal == false
        followBackground.followItem: hoveredItem

        model: root.updateTagModel
        // delegate: YT_NavigationWidget.M_ViewItem {
        //     id: updateTagView_Delegate
        //     required property int modelData
        //     readonly property int infoId: modelData

        //     // textData: fileModel.dataTag_InfoId(infoId, "Title", 0)
        //     text: root.getMusicInfo_CoverPath(infoId)
        //     implicitWidth: implicitContentWidth + indicator.width + spacing
        //     implicitHeight: 50

        //     imageDisplay: YT_RoundImage {
        //         radius: YT_Info.Radius
        //         source: updateTagView_Delegate.imageData
        //         imageSource.sourceSize: Qt.size(50, 50)
        //         imageSource.mipmap: true
        //         asynchronous: true
        //     }
        //     contentItem: GridLayout {
        //         rowSpacing: YT_Info.SpacingSmall
        //         columnSpacing: YT_Info.Spacing

        //         YT_PathWidget_TagItem {
        //             Layout.row: 0
        //             Layout.column: 0
        //             Layout.columnSpan: 2
        //             Layout.fillHeight: true

        //             enabled: false
        //             hoverEnabled: false
        //             modelData: fileModel.dataTag_InfoId(updateTagView_Delegate.infoId, "Title", 0) || ""
        //             headerData: "Title"
        //             pathModel: root.pathModel
        //         }
        //         YT_PathWidget_TagItem {
        //             Layout.row: 1
        //             Layout.column: 0
        //             Layout.preferredWidth: implicitWidth
        //             Layout.fillHeight: true

        //             enabled: false
        //             hoverEnabled: false
        //             modelData: fileModel.dataTag_InfoId(updateTagView_Delegate.infoId, "Album", 0) || ""
        //             headerData: "Album"
        //             pathModel: root.pathModel
        //         }
        //         YT_PathWidget_TagContainer {
        //             Layout.row: 1
        //             Layout.column: 1
        //             Layout.fillWidth: true
        //             Layout.fillHeight: true

        //             model: fileModel.dataTag_InfoId(updateTagView_Delegate.infoId, "Artist")
        //             delegate: YT_PathWidget_TagItem {
        //                 width: Math.min(implicitWidth, parent.width / parent.model.length)
        //                 enabled: false
        //                 hoverEnabled: false
        //                 headerData: "Artist"
        //                 pathModel: root.pathModel
        //             }
        //         }
        //     }

        //     onDoubleClicked: {
        //         console.log(infoId);
        //         root.updateMusicInfo_Local(infoId);
        //     }
        // }
    }

    Item {
        id: controlView
        parent: rootContent
        Layout.row: 2
        Layout.column: 0
        Layout.fillWidth: true
        Layout.minimumHeight: 56
        Layout.maximumHeight: 56

        // rows: 3
        // columns: 2
        // rowSpacing: YT_Info.SpacingSmall
        // columnSpacing: YT_Info.SpacingSmall
        Rectangle {
            width: parent.width
            height: 1
            color: YT_M.Theme.OutlineVariant
        }

        GridLayout {
            id: controlView_Info
            anchors.left: parent.left
            anchors.right: controlView_Control.left
            anchors.rightMargin: YT_M.Theme.Space200
            height: parent.height
            rowSpacing: YT_M.Theme.Space0
            columnSpacing: YT_M.Theme.Space50

            YT_RoundImage {
                asynchronous: true
                radius: YT_M.Theme.RadiusSmall
                padding: YT_M.Theme.Space50
                source: YT_Func.getFileCoverPath_InfoId(root.playInfoId)
                imageSource.sourceSize: Qt.size(50, 50)

                Layout.row: 0
                Layout.rowSpan: 2
                Layout.column: 0
                Layout.preferredWidth: height
                Layout.fillHeight: true
            }

            YT_PathWidget_TagItem {
                pathModel: root.pathModel
                pathData: root.fileModel.dataTag_InfoId(root.playInfoId, "Title", 0) || ""
                pathType: "Title"

                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: YT_M.Theme.Space100
            }

            YT_PathWidget_TagItem {
                opacity: 0.7
                font: YT_M.Theme.FontBodySmall
                pathModel: root.pathModel
                pathData: root.fileModel.dataTag_InfoId(root.playInfoId, "Album", 0) || ""
                pathType: "Album"

                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.bottomMargin: YT_M.Theme.Space75
            }
        }

        Column {
            id: controlView_Control
            width: Math.max(controlView.width - (controlView_Info.implicitWidth + YT_M.Theme.Space200) * 2, 280)
            anchors.centerIn: parent

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: YT_M.Theme.Space75

                YT_M.IconButton {
                    readonly property list<variant> iconSourceList: [YT_M.IconProvider.Shuffle, YT_M.IconProvider.Repeat, YT_M.IconProvider.RepeatOne]
                    iconSource: YT_M.IconProvider.Shuffle
                    onClicked: {
                        return console.log(iconSourceList);
                        let index = iconSourceList.indexOf(root.playMode);
                        index = index < 0 || index + 1 >= modelDataList.length ? 0 : index + 1;
                        root.playMode = modelDataList[index];
                    }
                }
                YT_M.IconButton {
                    iconSource: YT_M.IconProvider.FastRewind
                    onClicked: root.playPreviousMusic()
                }
                YT_M.IconButton {
                    iconSource: YT_AudioOutput.playing ? YT_M.IconProvider.Pause : YT_M.IconProvider.Resume
                    onClicked: YT_AudioOutput.playing ? YT_AudioOutput.pause() : YT_AudioOutput.play()
                }
                YT_M.IconButton {
                    iconSource: YT_M.IconProvider.FastForward
                    onClicked: root.playNextMusic()
                }
                YT_M.IconButton {
                    iconSource: YT_M.IconProvider.VolumeUp
                }
            }

            RowLayout {
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    font: YT_M.Theme.FontBodySmall
                    text: YT.formatTime(YT_AudioOutput.position)
                    color: YT_M.Theme.OnSurface
                }
                YT_M.Slider {
                    from: 0.0
                    value: YT_AudioOutput.position
                    to: YT_AudioOutput.duration

                    Layout.fillWidth: true
                    Layout.preferredHeight: 8
                    onPressedChanged: {
                        if (pressed)
                            return value = value;

                        YT_AudioOutput.position = value;
                        value = Qt.binding(function () {
                            return YT_AudioOutput.position;
                        });
                        if (YT_AudioOutput.playing == false)
                            YT_AudioOutput.play();
                    }
                }
                Text {
                    font: YT_M.Theme.FontBodySmall
                    text: YT.formatTime(YT_AudioOutput.duration)
                    color: YT_M.Theme.OnSurface
                }
            }
        }
    }

    // YT_MusicControl {
    //     id: controlView
    //     parent: rootContent
    //     Layout.row: 2
    //     Layout.column: 0
    //     Layout.fillWidth: true
    //     Layout.minimumHeight: 50
    //     Layout.maximumHeight: 50
    // }

    readonly property YT_EditDelegate editDelegate: YT_EditDelegate {
        id: editDelegate
    }

    // readonly property Item itemInfo: Row {
    //     id: itemInfo
    //     visible: false
    //     parent: rootContent
    //     spacing: YT_Info.Spacing
    //     move: Transition {
    //         NumberAnimation {
    //             property: "x"
    //             duration: 300
    //             easing.type: Easing.OutCubic
    //         }
    //     }

    //     Layout.row: 0
    //     Layout.column: 0
    //     Layout.fillWidth: true
    //     Layout.preferredHeight: implicitHeight
    //     Layout.bottomMargin: YT_Info.Margin

    //     Control {
    //         padding: YT_Info.Margin
    //         background: YT_Rectangle {
    //             color: YT_Info.ItemColor
    //         }
    //         contentItem: Row {
    //             spacing: YT_Info.Margin
    //             YT_Image {
    //                 width: parent.height
    //                 height: parent.height
    //                 readonly property string infoId: itemView.view.currentItem ? itemView.view.currentItem.infoId : ""
    //                 asynchronous: true
    //             }
    //             Column {
    //                 spacing: YT_Info.SpacingSmall
    //                 ItemDelegate_TagItem {
    //                     modelData: fileModel.tagData(itemView.view.currentIndex, "Title")[0] || ""
    //                     headerData: "Title"
    //                     font.pixelSize: 18
    //                     font.bold: true
    //                 }
    //                 ItemDelegate_TagItem {
    //                     modelData: fileModel.tagData(itemView.view.currentIndex, "Album")[0] || ""
    //                     headerData: "Album"
    //                 }
    //                 ItemDelegate_TagContainer {
    //                     model: fileModel.tagData(itemView.view.currentIndex, "Artist")
    //                     delegate: ItemDelegate_TagItem {
    //                         headerData: "Artist"
    //                     }
    //                 }
    //             }
    //         }
    //     }

    //     Repeater {
    //         model: fileModel.headerModel.filter(v => ["Title", "Album", "Artist"].includes(v) == false)
    //         delegate: Control {
    //             id: tagInfo
    //             width: delegateSize + padding * 2
    //             height: parent ? parent.height : 0
    //             padding: YT_Info.Margin
    //             required property string modelData
    //             property int delegateSize: 0
    //             property var model: null
    //             Connections {
    //                 target: itemView.view
    //                 function onCurrentIndexChanged() {
    //                     tagInfo.model = null;
    //                     tagInfo.delegateSize = tagInfo.contentItem.headerItem.implicitWidth;
    //                     tagInfo.model = fileModel.tagData(itemView.view.currentIndex, tagInfo.modelData);
    //                     tagInfo.visible = tagInfo.model.length > 0;
    //                 }
    //             }

    //             background: YT_Rectangle {
    //                 color: YT_Info.ItemColor
    //             }
    //             contentItem: ListView {
    //                 model: parent.model
    //                 orientation: Qt.Vertical
    //                 header: YT_Text {
    //                     width: parent ? parent.width : 0
    //                     text: modelData
    //                     horizontalAlignment: Text.AlignHCenter
    //                     verticalAlignment: Text.AlignVCenter
    //                 }
    //                 delegate: YT_TextLink {
    //                     required property string modelData
    //                     width: parent ? parent.width : 0
    //                     text: modelData
    //                     horizontalAlignment: Text.AlignHCenter
    //                     verticalAlignment: Text.AlignVCenter
    //                     onImplicitWidthChanged: tagInfo.delegateSize = Math.max(tagInfo.delegateSize, implicitWidth)
    //                     onLinkActivated: function (link) {
    //                         root.pathModel.addSrcData(text);
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
}
