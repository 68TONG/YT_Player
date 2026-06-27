import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls
import YT_Player
import HuskarUI.Basic

MusicWidget {
    id: root

    readonly property list<Item> navigationWidget_Items: [listView, controlView, updateTagView]
    Component.onCompleted: {
        YT.musicWidget = this;
        YT.musicFileModel = fileModel;
        YT.musicListModel = listModel;
    }

    GridLayout {
        id: rootContent
        anchors.fill: parent
        rows: 3
        rowSpacing: 0
        columns: 1
        columnSpacing: 0
    }

    readonly property YT_NavigationWidget.YT_Delegate listView: YT_NavigationWidget.YT_Delegate {
        id: listView
        currentIndex: listModel.index(curListInfoID)
        followBackground.followItem: listView.currentItem

        property bool draging: false
        states: [
            State {
                name: ""
                PropertyChanges {
                    target: listView.followBackground
                    followItem: listView.currentItem
                }
            },
            State {
                name: "MenuOpening || EditOpening"
                when: listView_Menu.visible || editDelegate.visible
                PropertyChanges {
                    target: listView.followBackground
                    followItem: listView_Menu.parent
                }
                PropertyChanges {
                    target: YT.navigationWidget
                    setExpanded: true
                }
            },
            State {
                name: "Draging"
                when: listView.draging
                PropertyChanges {
                    target: listView.followBackground
                    followItem: null
                }
            },
            State {
                name: "Hovered"
                when: listView.hovered
                PropertyChanges {
                    target: listView.followBackground
                    followItem: listView.hoverItem
                }
            }
        ]

        model: listModel
        delegate: listDelegate
    }

    readonly property Component listDelegate: YT_NavigationWidget.YT_DelegateItem {
        id: listDelegate
        required property string infoId
        textData: infoId

        YT_DelegateDropMove {
            anchors.margins: -listView.spacing / 2
            DragHandler {
                target: null
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onActiveChanged: {
                    YT.navigationWidget.setExpanded = active;
                    listView.draging = active;

                    if (active == true)
                        parent.startDrag();
                }
            }
            moveFunc: function (source_index, target_index) {
                listModel.move(source_index, target_index);
            }
        }
        TapHandler {
            acceptedButtons: Qt.LeftButton
            onTapped: function (eventPoint, button) {
                if (listView.currentIndex == index) {
                    listView.closeVertical = !listView.closeVertical;
                    return;
                }
                curListInfoID = infoId;
            }
        }
        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: function (eventPoint, button) {
                listView_Menu.popup(parent);
            }
        }
    }

    readonly property YT_ListView fileView: YT_ListView {
        id: fileView
        parent: rootContent
        Layout.row: 1
        Layout.column: 0
        Layout.fillWidth: true
        Layout.fillHeight: true

        spacing: YT_Info.SpacingSmall
        model: fileModel
        header: fileHeader
        footer: fileFooter
        delegate: fileDelegate
        background: null
        view.currentIndex: selectionModel.currentIndex.row
        followBackground.followItem: view.currentItem
        followBackground.color: Qt.lighter(YT_Info.BackgroundColor, 1.4)
        followBackground.shadowColor: YT_Info.FontColor
        followBackground.widthPadding: YT_Info.MarginSmall
        followBackground.heightPadding: YT_Info.MarginSmall
        followBackground.enabledShadow: true

        Connections {
            target: fileModel
            function onViewPositionChanged(index) {
                fileView.view.positionViewAtIndex(index, ListView.Beginning);
            }
        }

        Connections {
            target: fileView.view
            function onMovementEnded() {
                const view = fileView.view;
                const pos = view.mapToItem(view.contentItem, view.topMargin, view.leftMargin);
                var index = view.indexAt(pos.x, pos.y);
                if (index < 0)
                    index = view.indexAt(pos.x, pos.y + view.spacing);
                if (index < 0)
                    return;

                const data = fileModel.tagData(index, fileModel.sortTag);
                if (data == null || data.length == 0)
                    return;

                fileModel.setFooterPosition(data[0][0]);
            }
        }
    }

    readonly property YT_SplitView fileHeader: YT_SplitView {
        id: fileHeader
        parent: fileView
        height: 30
        anchors.leftMargin: viewPadding
        anchors.rightMargin: viewPadding
        readonly property int viewPadding: YT_Info.Margin
        property int viewSize: fileHeader.width - fileHeader_ID.width - fileHeader.spacing * (fileHeader.count - true) - viewPadding * 2
        Connections {
            target: fileModel
            function onItemStretchFactorChanged() {
                fileHeader.viewSize = fileHeader.width - fileHeader_ID.width - fileHeader.spacing * (fileHeader.count - true) - viewPadding * 2;
            }
        }

        YT_Button {
            id: fileHeader_ID
            padding: 0
            modelData: "Id"
            contentItem: YT_Button.LoadText {
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            SplitView.maximumWidth: fileHeader.height
            SplitView.minimumWidth: fileHeader.height
            SplitView.preferredHeight: fileHeader.height
            onClicked: fileModel.sortInfoIds(modelData)
        }
        YT_Button {
            id: fileHeader_Frist
            modelData: fileModel.titleTag + " & " + fileModel.selectTag[0]
            contentItem: YT_Button.LoadText {}

            padding: 0
            horizontalPadding: YT_Info.Margin
            SplitView.preferredWidth: fileModel.itemStretchFactor[0] * fileHeader.viewSize
            SplitView.minimumWidth: implicitWidth
            SplitView.fillHeight: true

            signal_YT: true
            onClicked_YT: function (eventPoint, button) {
                if (button === Qt.LeftButton) {
                    fileModel.sortInfoIds(modelData);
                } else if (button === Qt.RightButton) {
                    fileHeader_Menu.popup(this, Qt.point(0, height + YT_Info.SpacingSmall));
                }
            }

            states: [
                State {
                    name: ""
                    PropertyChanges {
                        target: fileHeader_Frist
                        modelData: fileModel.titleTag + " & " + fileModel.selectTag[0]
                    }
                },
                State {
                    name: "HoverLeft"
                    when: fileHeader_Frist.hovered && fileHeader_Frist.hoverPosition === Qt.LeftEdge
                    PropertyChanges {
                        target: fileHeader_Frist
                        modelData: fileModel.titleTag
                    }
                },
                State {
                    name: "HoverRight"
                    when: fileHeader_Frist.hovered && fileHeader_Frist.hoverPosition === Qt.RightEdge
                    PropertyChanges {
                        target: fileHeader_Frist
                        modelData: fileModel.selectTag[0]
                    }
                }
            ]

            property int hoverPosition
            HoverHandler {
                onPointChanged: function () {
                    const pos = point.position;
                    if (pos.x < parent.width / 2) {
                        parent.hoverPosition = Qt.LeftEdge;
                    } else {
                        parent.hoverPosition = Qt.RightEdge;
                    }
                }
            }
        }
        Repeater {
            id: fileHeader_List
            model: fileModel.selectTag.slice(1)
            delegate: YT_Button {
                required property int index
                required property var model
                modelData: model.modelData
                contentItem: YT_Button.LoadText {}

                padding: 0
                horizontalPadding: YT_Info.Margin
                SplitView.preferredWidth: fileModel.itemStretchFactor[index + true] * fileHeader.viewSize
                SplitView.minimumWidth: implicitWidth
                SplitView.fillHeight: true

                signal_YT: true
                onClicked_YT: function (eventPoint, button) {
                    if (button === Qt.LeftButton) {
                        fileModel.sortInfoIds(modelData);
                    } else if (button === Qt.RightButton) {
                        fileHeader_Menu.popup(this, Qt.point(0, height + YT_Info.SpacingSmall));
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
                        var list = fileModel.selectTag.slice();
                        var data = list.splice(source_index, 1)[0];
                        list.splice(target_index, 0, data);
                        fileModel.selectTag = list;
                    }
                }
            }
        }
    }

    readonly property YT_ListSelectView fileFooter: YT_ListSelectView {
        id: fileFooter
        parent: fileView
        radius: YT_Info.Radius
        opacity: (hovered || focus) ? 1.0 : 0.7
        x: parent.width / 2 - width / 2
        width: (hovered || focus) ? fileView.width - anchors.bottomMargin * 2 : height
        height: 24

        hoverEnabled: true
        Behavior on width {
            NumberAnimation {
                duration: 500
                easing.type: Easing.OutQuad
            }
        }
        Behavior on opacity {
            NumberAnimation {
                duration: 500
                easing.type: Easing.OutQuad
            }
        }

        Connections {
            target: fileModel
            function onFooterPositionChanged(index) {
                fileFooter.view.currentIndex = index;
            }
        }

        view.orientation: Qt.Horizontal
        followBackground.widthPadding: YT_Info.MarginSmall
        followBackground.heightPadding: YT_Info.MarginSmall
        followBackground.followItem: (hovered || focus) ? view.currentItem : null
        onCurrentIndexChanged_YT: {
            fileFooter.focus = true;
            fileModel.setViewPosition(view.currentItem.modelData);
        }

        model: fileModel.footerModel
        delegate: ItemDelegate {
            required property int index
            required property string modelData

            padding: 0
            width: fileFooter.height
            height: fileFooter.height

            background: null
            contentItem: Text {
                text: modelData
                color: YT_Info.FontColor
                font.pointSize: Qt.application.font.pointSize - 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                ListView.view.currentIndex = index;
                fileFooter.currentIndexChanged_YT();
            }
        }
    }

    readonly property Component fileDelegate: ItemDelegate {
        id: fileDelegate
        property bool selected: fileView.selectionModel.isSelected(fileModel.index(index, 0))

        required property int index
        required property int infoId
        required property string titleTag

        implicitWidth: ListView.view.width
        implicitHeight: 50

        topPadding: YT_Info.MarginSmall * 2
        bottomPadding: YT_Info.MarginSmall * 2
        horizontalPadding: fileHeader.viewPadding

        indicator: null
        background: null
        contentItem: GridLayout {
            rowSpacing: 0
            columnSpacing: fileHeader.spacing
        }

        Item {
            parent: fileDelegate.contentItem
            Layout.column: 0
            Layout.preferredWidth: fileHeader_ID.width
            Layout.preferredHeight: fileHeader_ID.height

            YT_Text {
                anchors.fill: parent
                text: fileDelegate.index + true
                color: infoId == root.playInfoId ? YT_Info.FontFocusColor : YT_Info.FontColor
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                opacity: fileDelegate.selected == false
                Behavior on opacity {
                    NumberAnimation {
                        duration: 300
                        easing.type: Easing.InOutCubic
                    }
                }
            }

            Image {
                anchors.fill: parent
                anchors.margins: YT_Info.MarginSmall * 2
                source: "qrc:/Ui_Image/selecting.png"
                fillMode: Image.PreserveAspectFit
                mipmap: true

                opacity: fileDelegate.selected == true ? 0.618 : 0.0
                Behavior on opacity {
                    NumberAnimation {
                        duration: 300
                        easing.type: Easing.InOutCubic
                    }
                }
            }
        }

        GridLayout {
            parent: fileDelegate.contentItem
            rowSpacing: YT_Info.SpacingSmall
            columnSpacing: fileHeader_Frist.horizontalPadding

            Layout.column: 1
            Layout.preferredWidth: fileHeader_Frist.width
            Layout.fillHeight: true

            YT_RoundImage {
                asynchronous: true
                radius: YT_Info.Radius
                padding: 2
                opacity: 0.7
                source: root.getMusicInfo_CoverPath(fileDelegate.infoId)
                imageSource.sourceSize: Qt.size(50, 50)
                imageSource.mipmap: true

                Layout.row: 0
                Layout.rowSpan: 2
                Layout.column: 0
                Layout.preferredWidth: height
                Layout.fillHeight: true
            }

            ItemDelegate_TagItem {
                modelData: fileDelegate.titleTag
                headerData: "Title"

                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.verticalStretchFactor: 6
            }

            ItemDelegate_TagContainer {
                model: fileModel.tagData(fileDelegate.index, fileModel.selectTag[0])
                delegate: ItemDelegate_TagItem {
                    width: Math.min(implicitWidth, parent.width / parent.model.length)
                    font.pointSize: Qt.application.font.pointSize - 2
                    color: hovered ? YT_Info.FontFocusColor : Qt.darker(YT_Info.FontColor, 1.3)
                    headerData: fileModel.selectTag[0]
                }

                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.verticalStretchFactor: 4
            }
        }

        Repeater {
            parent: fileDelegate.contentItem
            model: fileHeader_List.model
            delegate: ItemDelegate_TagContainer {
                required property int index
                required property string modelData
                readonly property Item modelItem: fileHeader_List.count ? fileHeader_List.itemAt(index) : null

                model: fileModel.tagData(fileDelegate.index, modelData)
                delegate: ItemDelegate_TagItem {
                    width: Math.min(implicitWidth, parent.width / parent.model.length)
                    headerData: parent.modelData
                }

                Layout.column: 2 + index
                Layout.leftMargin: fileHeader_Frist.horizontalPadding
                Layout.preferredWidth: modelItem ? modelItem.width - fileHeader_Frist.horizontalPadding : 0
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: function (mouse) {
                fileView.view.focus = true;
                var selectionModel = fileView.selectionModel;
                if (mouse.button === Qt.LeftButton) {
                    selectionModel.selectItem(index, mouse.modifiers);
                } else if (mouse.button === Qt.RightButton) {
                    if (selectionModel.selectedIndexes.length <= +true)
                        selectionModel.selectItem(index, mouse.modifiers);
                    fileView_Menu.popup(fileDelegate);
                } else {
                    return;
                }
            }
            onDoubleClicked: function (mouse) {
                if (mouse.button !== Qt.LeftButton)
                    return;
                playMusic(fileDelegate.index);
            }
        }
    }

    readonly property YT_Menu listView_Menu: YT_Menu {
        id: listView_Menu
        onClosed: parent = null
        Action {
            text: "添加列表"
            onTriggered: {
                const target = listView_Menu.parent;
                if (target === null)
                    return console.log("listView_Menu 添加列表 target === null");
                editDelegate.text = "";
                editDelegate.placeholderText = "请输入名称";
                editDelegate.open_YT(target, acceptedFunction, null);
            }

            function acceptedFunction(data, user_data) {
                listModel.addInfo(data);
            }
        }
        Action {
            text: "删除列表"
            onTriggered: {
                const target = listView_Menu.parent;
                if (target === null)
                    return console.log("listView_Menu 删除列表 target === null");
                listModel.eraseInfo(target.infoId);
            }
        }
        Action {
            text: "重命名列表"
            onTriggered: {
                const target = listView_Menu.parent;
                if (target === null)
                    return console.log("listView_Menu 重命名列表 target === null");
                editDelegate.text = target.infoId;
                editDelegate.userData = target.infoId;
                editDelegate.open_YT(target, acceptedFunction, null);
            }
            function acceptedFunction(data, user_data) {
                listModel.setInfoName(user_data, data);
            }
        }
    }

    readonly property YT_Menu fileView_Menu: YT_Menu {
        id: fileView_Menu
        onClosed: parent = null
        YT_Menu.YT_MenuItem {
            text: "播放音乐"
            onTriggered: {
                const index = fileView.selectionModel.currentIndex.row;
                playMusic(index);
            }
        }
        YT_Menu.YT_MenuItem {
            text: "稍后播放"
            onTriggered: {
                let operationDatas = [];
                const selectedIndexes = fileView.selectionModel.selectedIndexes;
                if (selectedIndexes.length === 0) {
                    return console.log("稍后播放 未选择文件");
                }

                for (let i = selectedIndexes.length - 1; i >= 0; --i) {
                    const model_index = selectedIndexes[i];
                    operationDatas.push(fileModel.data(model_index, MusicFileModel.InfoId_Role));
                }
                addPlayMusic(operationDatas);
            }
        }
        YT_Menu {
            title: "添加到"
            model: listModel
            delegate: YT_Menu.YT_MenuItem {
                required property string infoId
                text: infoId
                onTriggered: {
                    let operationPaths = [];
                    const selectedIndexes = fileView.selectionModel.selectedIndexes;
                    if (selectedIndexes.length === 0) {
                        return console.log("添加到 ", infoId, " 未选择文件");
                    }

                    for (const model_index of selectedIndexes) {
                        operationPaths.push(fileModel.data(model_index, MusicFileModel.MusicPath_Role));
                    }
                    listModel.addFileInfo(infoId, operationPaths);
                }
            }
        }
        YT_Menu.YT_MenuItem {
            text: "评分"
            implicitWidth: implicitContentWidth + indicator.implicitWidth + leftPadding + rightPadding + spacing
            implicitHeight: implicitContentHeight + topPadding + bottomPadding
            spacing: YT_Info.Spacing

            contentItem: YT_Text {
                text: parent.text
            }
            indicator: HusRate {
                anchors.right: parent.right
                anchors.rightMargin: parent.rightPadding
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: +true

                padding: 0
                background: null
                iconSize: fontMetrics.boundingRect("A").height
                readonly property FontMetrics fontMetrics: FontMetrics {
                    font: Qt.application.font
                }
                function updateInitValue() {
                    const selectedIndexes = fileView.selectionModel.selectedIndexes;
                    if (selectedIndexes.length != 1) {
                        initValue = 0;
                        return;
                    }
                    const model_index = selectedIndexes[0];
                    initValue = fileModel.data(model_index, MusicFileModel.StarLevel_Role);
                }
                Component.onCompleted: {
                    fileView_Menu.onAboutToShow.connect(updateInitValue);
                }
                onDone: function (data) {
                    let operationDatas = [];
                    const selectedIndexes = fileView.selectionModel.selectedIndexes;
                    if (selectedIndexes.length === 0) {
                        return console.log("评分 未选择文件");
                    }

                    for (let i = selectedIndexes.length - 1; i >= 0; --i) {
                        const model_index = selectedIndexes[i];
                        operationDatas.push(fileModel.data(model_index, MusicFileModel.InfoId_Role));
                    }

                    root.updateMusicInfo_StarLevel(data, operationDatas);
                    fileView_Menu.close();
                }
            }
            MouseArea {
                z: -true
                anchors.fill: parent
            }
        }
    }

    readonly property YT_Menu fileHeader_Menu: YT_Menu {
        id: fileHeader_Menu

        width: parent ? parent.width : 0
        model: fileModel.headerModel
        delegate: YT_Menu.YT_MenuSelectItem {
            required property int index
            required property string modelData
            selected: fileModel.selectTag.includes(modelData) || modelData == fileModel.titleTag
            text: modelData
            onTriggered: {
                if (modelData == fileModel.titleTag || modelData == fileModel.sortTag) {
                    console.log("modelData == fileModel.titleTag || modelData == fileModel.sortTag");
                    return;
                }

                if (fileHeader_Menu.parent == fileHeader_Frist && fileModel.sortTag != fileModel.selectTag[0]) {
                    var list = fileModel.selectTag.filter(v => modelData != v);
                    if (fileModel.selectTag[0] != modelData)
                        list.splice(0, 0, modelData);
                    fileModel.selectTag = list;
                } else if (selected == false) {
                    fileModel.selectTag.push(modelData);
                } else {
                    fileModel.selectTag = fileModel.selectTag.filter(function (it) {
                        return it != modelData;
                    });
                }
                fileHeader_Menu.close();
            }
        }

        YT_Menu.YT_MenuItem {
            text: "调整列大小"
        }
        YT_Menu.YT_MenuItem {
            text: "调整列所有大小"
        }
        YT_Menu.YT_MenuSeparator {}
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
    //                 source: "image://MusicImage/" + infoId
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

    readonly property Item controlView: GridLayout {
        id: controlView
        rows: 3
        columns: 2
        rowSpacing: YT_Info.SpacingSmall
        columnSpacing: YT_Info.SpacingSmall
        width: parent ? parent.width : 0
        height: implicitHeight

        property int closeSize: 30
        property bool closeVertical: false
        property bool closeHorizontal: YT.navigationWidget.expanded == false

        YT_RoundImage {
            id: controlView_Cover
            property int imageSize: Math.min(70, parent ? parent.width : 0)
            Layout.row: 0
            Layout.column: 0
            Layout.preferredWidth: imageSize
            Layout.preferredHeight: imageSize

            radius: YT_Info.Radius
            padding: YT_Info.MarginSmall
            opacity: controlView.closeHorizontal ? 0.45 : 0.85
            Behavior on opacity {
                NumberAnimation {
                    duration: 500
                    easing.type: Easing.OutQuad
                }
            }

            // readonly property string infoId: itemView.view.currentItem ? itemView.view.currentItem.infoId : ""
            source: root.getMusicInfo_CoverPath(root.playInfoId)
            imageSource.mipmap: true
            asynchronous: true
        }
        Column {
            id: controlView_Info
            visible: controlView.closeHorizontal == false
            spacing: YT_Info.SpacingSmall

            Layout.row: 0
            Layout.column: 1
            ItemDelegate_TagItem {
                font.pixelSize: 18
                font.bold: true
                modelData: fileModel.dataTag_InfoId(root.playInfoId, "Title", 0) || ""
                headerData: "Title"
            }
            ItemDelegate_TagItem {
                x: YT_Info.MarginSmall
                modelData: fileModel.dataTag_InfoId(root.playInfoId, "Album", 0) || ""
                headerData: "Album"
            }
            ItemDelegate_TagContainer {
                x: YT_Info.MarginSmall
                width: parent.width
                model: fileModel.dataTag_InfoId(root.playInfoId, "Artist")
                delegate: ItemDelegate_TagItem {
                    headerData: "Artist"
                    width: Math.min(implicitWidth, parent.width / parent.model.length)
                }
            }
        }
        RowLayout {
            id: controlView_OperateContainer
            Layout.row: 1
            Layout.column: 0
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignCenter
            spacing: YT_Info.SpacingSmall
            visible: controlView.closeHorizontal == false

            property int operateItemSize: 24
            YT_Button {
                Layout.preferredWidth: controlView_OperateContainer.operateItemSize
                Layout.preferredHeight: controlView_OperateContainer.operateItemSize
                readonly property list<string> modelDataList: ["qrc:/Ui_Image/shuffle.png", "qrc:/Ui_Image/list_loop.png", "qrc:/Ui_Image/single_loop.png"]
                modelData: root.playMode
                contentItem: YT_Button.LoadImage {}
                onClicked: {
                    let index = modelDataList.indexOf(root.playMode);
                    index = index < 0 || index + 1 >= modelDataList.length ? 0 : index + 1;
                    root.playMode = modelDataList[index];
                }
            }
            YT_Button {
                Layout.preferredWidth: controlView_OperateContainer.operateItemSize
                Layout.preferredHeight: controlView_OperateContainer.operateItemSize
                modelData: "qrc:/Ui_Image/previous.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: root.playPreviousMusic()
            }
            YT_Button {
                Layout.preferredWidth: controlView_OperateContainer.operateItemSize + 6
                Layout.preferredHeight: controlView_OperateContainer.operateItemSize + 6
                modelData: YT_AudioOutput.playing ? "qrc:/Ui_Image/playing.png" : "qrc:/Ui_Image/stoping.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: YT_AudioOutput.playing ? YT_AudioOutput.pause() : YT_AudioOutput.play()
            }
            YT_Button {
                Layout.preferredWidth: controlView_OperateContainer.operateItemSize
                Layout.preferredHeight: controlView_OperateContainer.operateItemSize
                modelData: "qrc:/Ui_Image/next.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: root.playNextMusic()
            }
            YT_Button {
                Layout.preferredWidth: controlView_OperateContainer.operateItemSize
                Layout.preferredHeight: controlView_OperateContainer.operateItemSize
                modelData: "qrc:/Ui_Image/volume.png"
                contentItem: YT_Button.LoadImage {}
            }
        }
        RowLayout {
            id: controlView_ProgressBar
            Layout.row: 2
            Layout.column: 0
            Layout.columnSpan: 2
            Layout.topMargin: -YT_Info.SpacingSmall
            spacing: YT_Info.Spacing
            visible: controlView.closeHorizontal == false

            YT_Text {
                text: YT.formatTime(YT_AudioOutput.position)
                font.pointSize: Qt.application.font.pointSize - 2
            }
            YT_Slider {
                Layout.fillWidth: true
                Layout.fillHeight: true
                background.anchors.centerIn: this
                backgroundColor: YT_Info.BackgroundColor

                from: 0
                value: YT_AudioOutput.position
                to: YT_AudioOutput.duration

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
            YT_Text {
                text: YT.formatTime(YT_AudioOutput.duration)
                font.pointSize: Qt.application.font.pointSize - 2
            }
        }
    }

    readonly property YT_NavigationWidget.YT_Delegate updateTagView: YT_NavigationWidget.YT_Delegate {
        id: updateTagView
        followBackground.followItem: hoverItem

        visible: opacity != 0.0
        opacity: closeHorizontal == false
        Behavior on opacity {
            NumberAnimation {
                duration: 180
                easing.type: Easing.OutQuad
            }
        }

        model: updateTagModel
        delegate: YT_NavigationWidget.YT_DelegateItem {
            id: updateTagView_Delegate
            required property int modelData
            readonly property int infoId: modelData

            // textData: fileModel.dataTag_InfoId(infoId, "Title", 0)
            imageData: root.getMusicInfo_CoverPath(infoId)
            implicitWidth: implicitContentWidth + indicator.width + spacing
            implicitHeight: 50

            imageDisplay: YT_RoundImage {
                radius: YT_Info.Radius
                source: updateTagView_Delegate.imageData
                imageSource.sourceSize: Qt.size(50, 50)
                imageSource.mipmap: true
                asynchronous: true
            }
            contentItem: GridLayout {
                rowSpacing: YT_Info.SpacingSmall
                columnSpacing: YT_Info.Spacing

                ItemDelegate_TagItem {
                    Layout.row: 0
                    Layout.column: 0
                    Layout.columnSpan: 2
                    Layout.fillHeight: true

                    enabled: false
                    hoverEnabled: false
                    modelData: fileModel.dataTag_InfoId(updateTagView_Delegate.infoId, "Title", 0) || ""
                    headerData: "Title"
                }
                ItemDelegate_TagItem {
                    Layout.row: 1
                    Layout.column: 0
                    Layout.preferredWidth: implicitWidth
                    Layout.fillHeight: true

                    enabled: false
                    hoverEnabled: false
                    modelData: fileModel.dataTag_InfoId(updateTagView_Delegate.infoId, "Album", 0) || ""
                    headerData: "Album"
                }
                ItemDelegate_TagContainer {
                    Layout.row: 1
                    Layout.column: 1
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    model: fileModel.dataTag_InfoId(updateTagView_Delegate.infoId, "Artist")
                    delegate: ItemDelegate_TagItem {
                        enabled: false
                        hoverEnabled: false
                        headerData: "Artist"
                        width: Math.min(implicitWidth, parent.width / parent.model.length)
                    }
                }
            }

            onDoubleClicked: {
                console.log(infoId);
                root.updateMusicInfo_Local(infoId);
            }
        }
    }

    readonly property YT_MusicControl playInfo: YT_MusicControl {
        id: playInfo
        parent: rootContent
        Layout.row: 2
        Layout.column: 0
        Layout.fillWidth: true
        Layout.minimumHeight: 44
        Layout.maximumHeight: 44
    }

    readonly property YT_EditDelegate editDelegate: YT_EditDelegate {
        id: editDelegate
    }

    component ItemDelegate_TagItem: YT_TextLink {
        required property string modelData
        required property string headerData
        text: modelData
        elide: Text.ElideRight
        mouseArea.onClicked: function (mouse) {
            if (mouse.modifiers == Qt.NoModifier) {
                root.pathModel.addSrcData(modelData);
            } else if (mouse.modifiers & Qt.ControlModifier) {
                root.pathModel.addSrcData('(' + headerData + ')' + modelData);
            }
        }
    }
    component ItemDelegate_TagContainer: Row {
        property var model
        property Component delegate
        spacing: YT_Info.Spacing
        Repeater {
            model: parent.model
            delegate: parent.delegate
        }
    }
}
