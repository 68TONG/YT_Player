import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

import YT_Player

MusicWidget {
    id: root
    Component.onCompleted: {
        YT.musicWidget = this
        YT.musicItemModel = itemModel
        YT.musicListModel = listModel
    }

    YT_SplitView {
        id: rootContent
        anchors.fill: parent
    }

    readonly property YT_ListView listView: YT_ListView {
        id: listView
        parent: rootContent.contentItem

        padding: 7
        view.acceptedButtons: Qt.NoButton
        view.currentIndex: listModel.index(curListInfoID)
        followBackground.followItem: listView.view.currentItem
        followBackground.heightPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)
        SplitView.preferredWidth: 100
        SplitView.fillHeight: true

        model: listModel
        delegate: listView_Delegate
        followBackground.states: [
            State {
                name: ""
                PropertyChanges {
                    target: listView.followBackground
                    followItem: listView.view.currentItem
                }
            },
            State {
                name: "MenuOpening || EditOpening"
                when: listView_Menu.visible || editDelegate.visible
                PropertyChanges {
                    target: listView.followBackground
                    followItem: listView_Menu.parent
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

        property bool draging: false
        property bool hovered: false
        property Item hoverItem: null

        HoverHandler {
            parent: listView.view
            onHoveredChanged: {
                listView.hovered = hovered
                if (hovered === false) listView.hoverItem = null
            }
        }

        TapHandler {
            parent: listView.view
            acceptedButtons: Qt.RightButton
            onTapped: function (eventPoint, button) {
                listView_Menu.popup()
            }
        }
    }

    readonly property YT_ListView itemView: YT_ListView {
        id: itemView
        parent: rootContent.contentItem

        padding: 7
        spacing: 3
        SplitView.fillWidth: true
        SplitView.fillHeight: true

        model: itemModel
        header: itemHeader
        delegate: itemView_Delegate
        Connections {
            target: itemModel
            function onViewPositionChanged(index) {
                itemView.view.positionViewAtIndex(index, ListView.Beginning)
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

        readonly property YT_Button itemID: YT_Button {
            parent: itemHeader.contentItem
            padding: 0
            modelData: "ID"
            contentItem: YT_Button.LoadText {
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            SplitView.maximumWidth: itemHeader.height
            SplitView.minimumWidth: itemHeader.height
            SplitView.preferredHeight: itemHeader.height

            onClicked: itemModel.sortInfoID_List(modelData)
        }
        readonly property YT_Button itemOne: YT_Button {
            parent: itemHeader.contentItem
            modelData: metaData_Title + " & " + metaData_One
            contentItem: YT_Button.LoadText { }

            padding: 0
            horizontalPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

            SplitView.fillWidth: true
            SplitView.fillHeight: true

            signal_YT: true
            onClicked_YT: function (eventPoint, button) {
                if (button === Qt.LeftButton) {
                    itemModel.sortInfoID_List(modelData)
                } else if (button === Qt.RightButton) {
                    itemHeader_Menu.open_YT(itemHeader.itemOne)
                }
            }

            states: [
                State {
                    name: ""
                    PropertyChanges {
                        target: itemHeader.itemOne
                        modelData: metaData_Title + " & " + metaData_One
                    }
                },
                State {
                    name: "HoverLeft"
                    when: itemHeader.itemOne.hovered && itemHeader.itemOne.hoverPosition === Qt.LeftEdge
                    PropertyChanges {
                        target: itemHeader.itemOne
                        modelData: metaData_Title
                    }
                },
                State {
                    name: "HoverRight"
                    when: itemHeader.itemOne.hovered && itemHeader.itemOne.hoverPosition === Qt.RightEdge
                    PropertyChanges {
                        target: itemHeader.itemOne
                        modelData: metaData_One
                    }
                }
            ]

            property int hoverPosition
            HoverHandler {
                onPointChanged: function () {
                    const pos = point.position
                    if (pos.x < parent.width / 2) {
                        parent.hoverPosition = Qt.LeftEdge
                    } else {
                        parent.hoverPosition = Qt.RightEdge
                    }
                }
            }
        }
        readonly property YT_Button itemTwo: YT_Button {
            parent: itemHeader.contentItem
            modelData: metaData_Two
            contentItem: YT_Button.LoadText {}

            padding: 0
            horizontalPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

            SplitView.fillWidth: true
            SplitView.fillHeight: true

            signal_YT: true
            onClicked_YT: function (eventPoint, button) {
                if (button === Qt.LeftButton) {
                    itemModel.sortInfoID_List(modelData)
                } else if (button === Qt.RightButton) {
                    itemHeader_Menu.open_YT(itemHeader.itemTwo)
                }
            }
        }
    }

    readonly property YT_EditDelegate editDelegate: YT_EditDelegate {
        id: editDelegate
    }

    readonly property YT_Menu listView_Menu: YT_Menu {
        id: listView_Menu
        onClosed: parent = null
        Action {
            text: "添加列表"
            onTriggered: {
                const target = listView.view.itemAtIndex(listView.view.count - true)
                if (target === null) return console.log("listView_Menu 添加列表 target === null")
                editDelegate.text = ""
                editDelegate.placeholderText = "请输入名称"
                editDelegate.y = target.height + 3
                editDelegate.open_YT(target, acceptedFunction, null)
            }

            function acceptedFunction(data) {
                listModel.addInfo(data["acceptData"])
            }
        }
        Action {
            text: "删除列表"
            onTriggered: {
                const target = listView_Menu.parent
                if (target === null) return console.log("listView_Menu 删除列表 target === null")
                listModel.eraseInfo(target.infoID)
            }
        }
        Action {
            text: "重命名列表"
            onTriggered: {
                const target = listView_Menu.parent
                if (target === null) return console.log("listView_Menu 重命名列表 target === null")
                editDelegate.text = target.name
                editDelegate.modelData["infoID"] = target.infoID
                editDelegate.open_YT(target, acceptedFunction, null)
            }
            function acceptedFunction(data) {
                listModel.setInfoName(data["infoID"], data["acceptData"])
            }
        }
    }

    readonly property YT_Menu itemView_Menu: YT_Menu {
        id: itemView_Menu
        onClosed: parent = null
        Action {
            text: "播放音乐"
            onTriggered: {
                const index = itemView.selectionModel.currentIndex.row
                playMusic(index)
            }
        }
        Action {
            text: "稍后播放"
            onTriggered: {
                const selectedIndexes = itemView.selectionModel.selectedIndexes
                for (const model_index of selectedIndexes) {
                    const index = model_index.row
                    const target = itemView.view.itemAtIndex(index)
                    addPlayMusic(target.model.path)
                }
            }
        }
        YT_Menu {
            title: "添加到"

            model: listModel
            delegate: YT_Menu.YT_MenuItem {
                required property int infoID
                required property string name

                text: name
                onTriggered: {
                    var operationPaths = []
                    const selectedIndexes = itemView.selectionModel.selectedIndexes
                    if (selectedIndexes.length === 0) {
                        return console.log("未选择文件")
                    }

                    for (const model_index of selectedIndexes) {
                        operationPaths.push(itemModel.data(model_index, MusicItemModel.InfoRole_Path))
                    }
                    listModel.addItemInfo(infoID, operationPaths)
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

        model: itemHeaderModel
        delegate: YT_PopupList.YT_PopupList_Delegate {
            readonly property bool selected: [metaData_Title, metaData_One, metaData_Two].includes(modelData)
            modelData: model.modelData
            implicitWidth: parent ? parent.width : 0
            indicator: YT_PopupList.YT_SelectedIndicator {}
            onClicked: {
                itemHeader_Menu.close()
            }
        }
    }

    readonly property Component listView_Delegate: YT_ItemDelegate {
        id: listView_Delegate
        info_YT: listView.delegateInfo
        required property int infoID
        required property string name
        onDragingChanged: listView.draging = draging

        ItemDelegate {
            implicitWidth: info_YT.view.contentItem.width
            implicitHeight: 30 + verticalPadding * 2

            padding: 0
            horizontalPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
            verticalPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)

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
            contentItem: Text {
                clip: true
                text: listView_Delegate.name
                color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }
            onHoveredChanged: {
                if (hovered) listView.hoverItem = listView_Delegate
            }

            YT_ItemDelegate.YT_DragDelegate {
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                item_delegate: listView_Delegate
                item_container: listView_Delegate.item_container

                onClicked: function (mouse) {
                    listView.view.focus = true
                    if (mouse.button === Qt.LeftButton) {
                        curListInfoID = listView_Delegate.infoID
                    } else if (mouse.button === Qt.RightButton) {
                        listView_Menu.popup(listView_Delegate)
                    } else {
                        return
                    }
                }
            }
        }
    }

    readonly property Component itemView_Delegate: ItemDelegate {
        id: itemView_Delegate
        property bool selected: itemView.selectionModel.isSelected(itemModel.index(index, 0))

        required property int index
        required property var model
        required property string title
        required property string metaData_One
        required property string metaData_Two

        implicitWidth: ListView.view.width
        implicitHeight: 50

        padding: 0
        topPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)
        bottomPadding: bottomInset + YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)
        bottomInset: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)

        indicator: null
        background: Rectangle {
            visible: opacity !== 0
            opacity: selected
            radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
            color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)

            Behavior on opacity {
                NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
            }
        }
        contentItem: GridLayout {
            rows: 2
            rowSpacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)
            columns: 3
            columnSpacing: itemHeader.spacing
        }

        Text {
            parent: itemView_Delegate.contentItem

            text: itemView_Delegate.index + true
            color: index == curPlayIndex ? "#FFD700" : YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            padding: 0
            Layout.row: 0
            Layout.rowSpan: 2
            Layout.column: 0
            Layout.maximumWidth: itemHeader.itemID.width
            Layout.minimumWidth: itemHeader.itemID.width
            Layout.preferredWidth: itemHeader.itemID.width
        }
        YT_Button.LoadText {
            parent: itemView_Delegate.contentItem

            link_YT: true
            elide: Text.ElideRight
            text: itemView_Delegate.title
            onLinkActivated: function(link) {
                root.addPath_FilterInfo(itemView_Delegate.title, metaData_Title)
            }

            padding: 0
            leftPadding: itemHeader.itemOne.horizontalPadding

            Layout.row: 0
            Layout.column: 1
            Layout.preferredWidth: itemHeader.itemOne.width
            Layout.verticalStretchFactor: 7
            Layout.fillHeight: true
        }
        YT_Button.LoadText {
            parent: itemView_Delegate.contentItem

            link_YT: true
            elide: Text.ElideRight
            font.pointSize: itemHeader.itemOne.font.pointSize - 2
            color: link_hovered ? "#FFD700" : Qt.darker(YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor), 1.3)
            text: itemView_Delegate.metaData_One
            onLinkActivated: function(link) {
                root.addPath_FilterInfo(itemView_Delegate.metaData_One, root.metaData_One)
            }

            padding: 0
            leftPadding: itemHeader.itemOne.horizontalPadding

            Layout.row: 1
            Layout.column: 1
            Layout.preferredWidth: itemHeader.itemOne.width
            Layout.verticalStretchFactor: 3
            Layout.fillHeight: true
        }
        YT_Button.LoadText {
            parent: itemView_Delegate.contentItem

            link_YT: true
            elide: Text.ElideRight
            text: itemView_Delegate.metaData_Two
            onLinkActivated: function(link) {
                root.addPath_FilterInfo(itemView_Delegate.metaData_Two, root.metaData_Two)
            }

            padding: 0
            leftPadding: itemHeader.itemTwo.horizontalPadding

            Layout.row:0
            Layout.rowSpan: 2
            Layout.column: 2
            Layout.preferredWidth: itemHeader.itemTwo.width
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: function (mouse) {
                itemView.view.focus = true
                var selectionModel = itemView.selectionModel
                if (mouse.button === Qt.LeftButton) {
                    selectionModel.selectItem(index, mouse.modifiers)
                    // itemView.view.positionViewAtIndex(itemView_Delegate.index, ListView.Center)
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
                playMusic(itemView_Delegate.index)
            }
        }
    }
}
