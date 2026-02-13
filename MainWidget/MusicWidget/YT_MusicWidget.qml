import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls
import YT_Player

MusicWidget {
    id: root
    Component.onCompleted: {
        YT.musicWidget = this;
        YT.musicItemModel = itemModel;
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

        closeVertical: false
        closeHorizontal: YT.navigationWidget.expanded == false
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
        required property string infoID
        textData: infoID

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
                curListInfoID = infoID;
            }
        }
        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: function (eventPoint, button) {
                listView_Menu.popup(parent);
            }
        }
    }

    readonly property YT_ListView itemView: YT_ListView {
        id: itemView
        parent: rootContent
        Layout.row: 1
        Layout.column: 0
        Layout.fillWidth: true
        Layout.fillHeight: true

        spacing: YT_Info.SpacingSmall
        model: itemModel
        header: itemHeader
        footer: itemFooter
        delegate: itemDelegate
        background: null
        view.currentIndex: selectionModel.currentIndex.row

        Connections {
            target: itemModel
            function onViewPositionChanged(index) {
                itemView.view.positionViewAtIndex(index, ListView.Beginning);
            }
        }

        Connections {
            target: itemView.view
            function onMovementEnded() {
                const view = itemView.view;
                const pos = view.mapToItem(view.contentItem, view.topMargin, view.leftMargin);
                var index = view.indexAt(pos.x, pos.y);
                if (index < 0)
                    index = view.indexAt(pos.x, pos.y + view.spacing);
                if (index < 0)
                    return;

                const data = itemModel.tagData(index, itemModel.sortTag);
                if (data == null || data.length == 0)
                    return;

                itemModel.setFooterPosition(data[0][0]);
            }
        }
    }

    readonly property YT_SplitView itemHeader: YT_SplitView {
        id: itemHeader
        parent: itemView
        height: 30

        YT_Button {
            id: itemHeader_ID
            padding: 0
            modelData: "ID"
            contentItem: YT_Button.LoadText {
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            SplitView.maximumWidth: itemHeader.height
            SplitView.minimumWidth: itemHeader.height
            SplitView.preferredHeight: itemHeader.height
            onClicked: itemModel.sortInfoIds(modelData)
        }
        YT_Button {
            id: itemHeader_Frist
            modelData: itemModel.titleTag + " & " + itemModel.selectTag[0]
            contentItem: YT_Button.LoadText {}

            padding: 0
            horizontalPadding: YT_Info.Margin
            SplitView.preferredWidth: 100
            SplitView.fillHeight: true

            signal_YT: true
            onClicked_YT: function (eventPoint, button) {
                if (button === Qt.LeftButton) {
                    itemModel.sortInfoIds(modelData);
                } else if (button === Qt.RightButton) {
                    itemHeader_Menu.open_YT(this);
                }
            }

            states: [
                State {
                    name: ""
                    PropertyChanges {
                        target: itemHeader_Frist
                        modelData: itemModel.titleTag + " & " + itemModel.selectTag[0]
                    }
                },
                State {
                    name: "HoverLeft"
                    when: itemHeader_Frist.hovered && itemHeader_Frist.hoverPosition === Qt.LeftEdge
                    PropertyChanges {
                        target: itemHeader_Frist
                        modelData: itemModel.titleTag
                    }
                },
                State {
                    name: "HoverRight"
                    when: itemHeader_Frist.hovered && itemHeader_Frist.hoverPosition === Qt.RightEdge
                    PropertyChanges {
                        target: itemHeader_Frist
                        modelData: itemModel.selectTag[0]
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
            id: itemHeader_List
            model: itemModel.selectTag.slice(1)
            delegate: YT_Button {
                required property int index
                required property var model
                modelData: model.modelData
                contentItem: YT_Button.LoadText {}

                padding: 0
                horizontalPadding: YT_Info.Margin
                SplitView.fillHeight: true

                signal_YT: true
                onClicked_YT: function (eventPoint, button) {
                    if (button === Qt.LeftButton) {
                        itemModel.sortInfoIds(modelData);
                    } else if (button === Qt.RightButton) {
                        itemHeader_Menu.open_YT(this);
                    }
                }

                YT_DelegateDropMove {
                    orientation: itemHeader.orientation
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
                        var list = itemModel.selectTag.slice();
                        var data = list.splice(source_index, 1)[0];
                        list.splice(target_index, 0, data);
                        itemModel.selectTag = list;
                    }
                }
            }
        }
    }

    readonly property YT_ListSelectView itemFooter: YT_ListSelectView {
        id: itemFooter
        parent: itemView
        radius: YT_Info.Radius
        opacity: (hovered || focus) ? 1.0 : 0.7
        x: parent.width / 2 - width / 2
        width: (hovered || focus) ? itemView.width - anchors.bottomMargin * 2 : height
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
            target: itemModel
            function onFooterPositionChanged(index) {
                itemFooter.view.currentIndex = index;
            }
        }

        view.orientation: Qt.Horizontal
        followBackground.widthPadding: YT_Info.MarginSmall
        followBackground.heightPadding: YT_Info.MarginSmall
        followBackground.followItem: (hovered || focus) ? view.currentItem : null
        onCurrentIndexChanged_YT: {
            itemFooter.focus = true;
            itemModel.setViewPosition(view.currentItem.modelData);
        }

        model: itemModel.footerModel
        delegate: ItemDelegate {
            required property int index
            required property string modelData

            padding: 0
            width: itemFooter.height
            height: itemFooter.height

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
                itemFooter.currentIndexChanged_YT();
            }
        }
    }

    readonly property Component itemDelegate: ItemDelegate {
        id: itemDelegate
        property bool selected: itemView.selectionModel.isSelected(itemModel.index(index, 0))

        required property int index
        required property var model
        required property int infoID
        required property string titleTag

        implicitWidth: ListView.view.width
        implicitHeight: 50

        padding: 0
        topPadding: bottomInset
        bottomPadding: bottomInset * 2
        bottomInset: YT_Info.MarginSmall

        indicator: null
        background: YT_Rectangle {
            animationDuration: 180
            opacity: selected
            color: YT_Info.ItemFocusColor
        }
        contentItem: GridLayout {
            rows: 2
            rowSpacing: YT_Info.SpacingSmall
            // columns: 3
            columnSpacing: itemHeader.spacing
        }

        YT_Text {
            parent: itemDelegate.contentItem
            text: itemDelegate.index + true
            color: index == curPlayIndex ? YT_Info.FontFocusColor : YT_Info.FontColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            Layout.row: 0
            Layout.rowSpan: 2
            Layout.column: 0
            Layout.preferredWidth: itemHeader_ID.width
        }
        ItemDelegate_TagItem {
            parent: itemDelegate.contentItem
            modelData: itemDelegate.titleTag

            Layout.row: 0
            Layout.column: 1
            Layout.leftMargin: itemHeader_Frist.horizontalPadding
            Layout.rightMargin: itemHeader_Frist.horizontalPadding
            Layout.preferredWidth: itemHeader_Frist.availableWidth
            Layout.verticalStretchFactor: 7
            Layout.fillHeight: true
        }
        ItemDelegate_TagContainer {
            parent: itemDelegate.contentItem
            model: itemModel.tagData(itemDelegate.index, itemModel.selectTag[0])
            delegate: ItemDelegate_TagItem {
                width: Math.min(implicitWidth, parent.width / parent.model.length)
                font.pointSize: Qt.application.font.pointSize - 2
                color: hovered ? YT_Info.FontFocusColor : Qt.darker(YT_Info.FontColor, 1.3)
            }

            Layout.row: 1
            Layout.column: 1
            Layout.leftMargin: itemHeader_Frist.horizontalPadding
            Layout.rightMargin: itemHeader_Frist.horizontalPadding
            Layout.preferredWidth: itemHeader_Frist.availableWidth
            Layout.verticalStretchFactor: 3
            Layout.fillHeight: true
        }
        Repeater {
            parent: itemDelegate.contentItem
            model: itemHeader_List.model
            delegate: ItemDelegate_TagContainer {
                required property int index
                required property string modelData
                readonly property Item modelItem: itemHeader_List.count ? itemHeader_List.itemAt(index) : null

                model: itemModel.tagData(itemDelegate.index, modelData)
                delegate: ItemDelegate_TagItem {
                    width: Math.min(implicitWidth, parent.width / parent.model.length)
                }

                Layout.row: 0
                Layout.rowSpan: 2
                Layout.column: 2 + index
                Layout.leftMargin: itemHeader_Frist.horizontalPadding
                Layout.rightMargin: itemHeader_Frist.horizontalPadding
                Layout.preferredWidth: modelItem ? modelItem.availableWidth : 0
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: function (mouse) {
                itemView.view.focus = true;
                var selectionModel = itemView.selectionModel;
                if (mouse.button === Qt.LeftButton) {
                    selectionModel.selectItem(index, mouse.modifiers);
                } else if (mouse.button === Qt.RightButton) {
                    if (selectionModel.selectedIndexes.length <= +true)
                        selectionModel.selectItem(index, mouse.modifiers);
                    itemView_Menu.popup(itemDelegate);
                } else {
                    return;
                }
            }
            onDoubleClicked: function (mouse) {
                if (mouse.button !== Qt.LeftButton)
                    return;
                playMusic(itemDelegate.index);
            }
        }
    }

    readonly property YT_Menu listView_Menu: YT_Menu {
        id: listView_Menu
        onClosed: parent = null
        Action {
            text: "添加列表"
            onTriggered: {
                const target = listView.itemAtIndex(listView.count - true);
                if (target === null)
                    return console.log("listView_Menu 添加列表 target === null");
                editDelegate.text = "";
                editDelegate.placeholderText = "请输入名称";
                editDelegate.y = target.height + 3;
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
                listModel.eraseInfo(target.infoID);
            }
        }
        Action {
            text: "重命名列表"
            onTriggered: {
                const target = listView_Menu.parent;
                if (target === null)
                    return console.log("listView_Menu 重命名列表 target === null");
                editDelegate.text = target.infoID;
                editDelegate.userData = target.infoID;
                editDelegate.open_YT(target, acceptedFunction, null);
            }
            function acceptedFunction(data, user_data) {
                listModel.setInfoName(user_data, data);
            }
        }
    }

    readonly property YT_Menu itemView_Menu: YT_Menu {
        id: itemView_Menu
        onClosed: parent = null
        Action {
            text: "播放音乐"
            onTriggered: {
                const index = itemView.selectionModel.currentIndex.row;
                playMusic(index);
            }
        }
        Action {
            text: "稍后播放"
            onTriggered: {
                var operationPaths = [];
                const selectedIndexes = itemView.selectionModel.selectedIndexes;
                if (selectedIndexes.length === 0) {
                    return console.log("稍后播放 未选择文件");
                }

                for (let i = selectedIndexes.length - 1; i >= 0; --i) {
                    const model_index = selectedIndexes[i];
                    operationPaths.push(itemModel.data(model_index, MusicItemModel.InfoRole_Path));
                }
                addPlayMusic(operationPaths);
            }
        }
        YT_Menu {
            title: "添加到"
            model: listModel
            delegate: YT_Menu.YT_MenuItem {
                required property string infoID
                text: infoID
                onTriggered: {
                    var operationPaths = [];
                    const selectedIndexes = itemView.selectionModel.selectedIndexes;
                    if (selectedIndexes.length === 0) {
                        return console.log("添加到 ", infoID, " 未选择文件");
                    }

                    for (const model_index of selectedIndexes) {
                        operationPaths.push(itemModel.data(model_index, MusicItemModel.InfoRole_Path));
                    }
                    listModel.addFileInfo(infoID, operationPaths);
                }
            }
        }
    }

    readonly property YT_PopupList itemHeader_Menu: YT_PopupList {
        id: itemHeader_Menu

        height: implicitHeight
        model: itemModel.headerModel
        delegate: YT_Button {
            required property int index
            required property var model
            readonly property bool selected: itemModel.selectTag.includes(modelData) || modelData == itemModel.titleTag

            indicator: YT_Button.LoadSelectIndicator {}
            background: null
            contentItem: YT_Button.LoadText {}

            modelData: model.modelData
            implicitWidth: parent ? parent.width : 0
            onHoveredChanged: {
                if (hovered)
                    ListView.view.currentIndex = index;
            }
            onClicked: {
                if (modelData == itemModel.titleTag || modelData == itemModel.sortTag) {
                    console.log("modelData == itemModel.titleTag || modelData == itemModel.sortTag");
                    return;
                }

                if (itemHeader_Menu.parent == itemHeader_Frist && itemModel.sortTag != itemModel.selectTag[0]) {
                    var list = itemModel.selectTag.filter(v => modelData != v);
                    if (itemModel.selectTag[0] != modelData)
                        list.splice(0, 0, modelData);
                    itemModel.selectTag = list;
                } else if (selected == false) {
                    itemModel.selectTag.push(modelData);
                } else {
                    itemModel.selectTag = itemModel.selectTag.filter(function (it) {
                        return it != modelData;
                    });
                }
                itemHeader_Menu.close();
            }
        }
    }

    readonly property Item itemInfo: Row {
        id: itemInfo
        visible: false
        parent: rootContent
        spacing: YT_Info.Spacing
        move: Transition {
            NumberAnimation {
                property: "x"
                duration: 300
                easing.type: Easing.OutCubic
            }
        }

        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true
        Layout.preferredHeight: implicitHeight
        Layout.bottomMargin: YT_Info.Margin

        Control {
            padding: YT_Info.Margin
            background: YT_Rectangle {
                color: YT_Info.ItemColor
            }
            contentItem: Row {
                spacing: YT_Info.Margin
                Item {
                    width: parent.height
                    height: parent.height
                    Image {
                        id: itemInfoImage
                        source: "image://MusicImage/" + infoID
                        anchors.fill: parent
                        visible: false // *** 隐藏原始图片，只显示 MultiEffect 的结果 ***
                        mipmap: true
                        asynchronous: true
                        readonly property string infoID: itemView.view.currentItem ? itemView.view.currentItem.infoID : ""
                    }
                    Rectangle {
                        id: itemInfoMask
                        anchors.fill: itemInfoImage
                        radius: YT_Info.Radius
                        visible: false // *** 隐藏遮罩源，因为它只是一个“模板” ***
                        layer.enabled: true // *** 关键：确保遮罩源被正确渲染到纹理中 ***
                        layer.smooth: true
                        layer.mipmap: true
                    }
                    MultiEffect {
                        anchors.fill: itemInfoImage
                        maskEnabled: true
                        source: itemInfoImage
                        maskSource: itemInfoMask
                    }
                }
                Column {
                    spacing: YT_Info.SpacingSmall
                    YT_TextLink {
                        readonly property var modelData: itemModel.tagData(itemView.view.currentIndex, "Title")[0]
                        text: modelData != null ? modelData : ""
                        font.pixelSize: 20
                        font.bold: true
                        onLinkActivated: function (link) {
                            root.addCurPath(text);
                        }
                    }
                    YT_TextLink {
                        readonly property var modelData: itemModel.tagData(itemView.view.currentIndex, "Album")[0]
                        text: modelData != null ? modelData : ""
                        onLinkActivated: function (link) {
                            root.addCurPath(text);
                        }
                    }
                    Row {
                        spacing: YT_Info.Spacing
                        Repeater {
                            model: itemModel.tagData(itemView.view.currentIndex, "Artist")
                            delegate: YT_TextLink {
                                required property string modelData
                                text: modelData
                                onLinkActivated: function (link) {
                                    root.addCurPath(text);
                                }
                            }
                        }
                    }
                }
            }
        }

        Repeater {
            model: itemModel.headerModel.filter(v => ["Title", "Album", "Artist"].includes(v) == false)
            delegate: Control {
                id: tagInfo
                width: delegateSize + padding * 2
                height: parent ? parent.height : 0
                padding: YT_Info.Margin
                required property string modelData
                property int delegateSize: 0
                property var model: null
                Connections {
                    target: itemView.view
                    function onCurrentIndexChanged() {
                        tagInfo.model = null;
                        tagInfo.delegateSize = tagInfo.contentItem.headerItem.implicitWidth;
                        tagInfo.model = itemModel.tagData(itemView.view.currentIndex, tagInfo.modelData);
                        tagInfo.visible = tagInfo.model.length > 0;
                    }
                }

                background: YT_Rectangle {
                    color: YT_Info.ItemColor
                }
                contentItem: ListView {
                    model: parent.model
                    orientation: Qt.Vertical
                    header: YT_Text {
                        width: parent ? parent.width : 0
                        text: modelData
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    delegate: YT_TextLink {
                        required property string modelData
                        width: parent ? parent.width : 0
                        text: modelData
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        onImplicitWidthChanged: tagInfo.delegateSize = Math.max(tagInfo.delegateSize, implicitWidth)
                        onLinkActivated: function (link) {
                            root.addCurPath(text);
                        }
                    }
                }
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
        text: modelData
        elide: Text.ElideRight
        onLinkActivated: function (link) {
            root.addCurPath(text);
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
