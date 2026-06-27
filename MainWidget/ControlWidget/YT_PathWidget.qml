import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import YT_Player

Item {
    id: root

    property int currentIndex: YT.widgetIndex
    onCurrentIndexChanged: YT.widgetIndex = currentIndex
    readonly property int rootPadding: 4
    readonly property int rootItemSize: height
    readonly property YT.YT_WidgetInfo currentInfo: YT.widgetInfos.get(currentIndex)

    QtObject {
        id: historicalPath
        property bool isSeting: false
        property int currentIndex: -1
        property list<string> pathDatas: []
        readonly property int pathDataCount: 10

        function addPath(data) {
            if (isSeting)
                return;

            const start = Math.max(0, currentIndex + 1 - pathDataCount);
            pathDatas = pathDatas.slice(start, currentIndex + 1);
            pathDatas.push(data);
            currentIndex = pathDatas.length - 1;
        }
        function setPath(data) {
            isSeting = true;
            let data_list = data.split('/');
            const data_first = data_list[0];
            for (let i = 0; i < YT.widgetInfos.count; i++) {
                if (YT.widgetInfos.get(i).widgetName == data_first) {
                    YT.widgetIndex = i;
                    data_list.shift();
                    break;
                }
            }
            currentInfo.pathModel.setSrcData(data_list);
            isSeting = false;
        }
        function setNextPath() {
            if (currentIndex < -1 || currentIndex > pathDatas.length - 2)
                return;
            currentIndex = currentIndex + 1;
            setPath(pathDatas[currentIndex]);
        }
        function setPreviousPath() {
            if (currentIndex < +1 || currentIndex > pathDatas.length)
                return;
            currentIndex = currentIndex - 1;
            setPath(pathDatas[currentIndex]);
        }
    }
    onCurrentInfoChanged: {
        historicalPath.addPath(currentInfo.pathModel.getData_WidgetPath());
    }
    Connections {
        target: currentInfo.pathModel
        function onSrcDataChanged() {
            historicalPath.addPath(currentInfo.pathModel.getData_WidgetPath());
        }
    }

    YT_Rectangle {
        anchors.fill: parent
        color: YT_Info.ItemColor
    }

    GridLayout {
        id: rootContent
        anchors.fill: parent
        rows: 1
        rowSpacing: 0
        columns: 3
        columnSpacing: 0

        // YT_PathBar {
        //     id: pathBar
        //     color: Qt.rgba(0, 0, 0, 0)
        //     border.color: Qt.rgba(0, 0, 0, 0)

        //     x: curWidget.width
        //     implicitWidth: parent.width - x
        //     implicitHeight: parent.height

        //     view.spacing: rootPadding
        //     view.cacheBuffer: (Math.pow(2, 31) - 1)

        //     model: YT.widgetInfos.get(currentIndex).widget_curPath
        //     delegate: pathBar_Delegate
        //     onModelChanged: {
        //         view.positionViewAtEnd();
        //     }
        // }

        // TapHandler {
        //     parent: pathBar.view
        //     onTapped: {
        //         editDelegate.text = editDelegate.getPathData();
        //         editDelegate.open_YT(root, editDelegate.acceptedFunction, null);
        //     }
        // }

        // YT_EditDelegate {
        //     id: editDelegate

        //     text: getPathData()
        //     padding: rootPadding
        //     topInset: padding
        //     leftInset: padding
        //     rightInset: padding
        //     bottomInset: padding
        //     editItem.font.pointSize: Qt.application.font.pointSize - 2

        //     function getPathData() {
        //         var data = "";
        //         for (var it of pathBar.model)
        //             data += it + "/";
        //         return data;
        //     }
        //     function acceptedFunction(data, user_data) {
        //         YT.widgetInfos.get(currentIndex).widget.setCurPath(data);
        //     }
        // }
    }

    readonly property Row navigationView: Row {
        id: navigationView
        parent: rootContent
        Layout.row: 0
        Layout.column: 0
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight
        Layout.rightMargin: YT_Info.Margin

        spacing: 0
        YT_PathWidget_Item {
            modelData: "qrc:/Ui_Image/previous_page.png"
            onClicked: {
                historicalPath.setPreviousPath();
            }
        }
        YT_PathWidget_Item {
            modelData: "qrc:/Ui_Image/next_page.png"
            onClicked: {
                historicalPath.setNextPath();
            }
        }
        YT_PathWidget_Item {
            modelData: "qrc:/Ui_Image/search_logo.png"
            onClicked: {
                searchWidget.visible ? searchWidget.close() : searchWidget.open();
            }

            Rectangle {
                width: +true
                radius: YT_Info.Radius
                color: YT_Info.ItemFocusColor

                anchors.top: parent.top
                anchors.left: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: rootPadding
                anchors.leftMargin: navigationView.Layout.rightMargin / 2
            }
        }
    }

    readonly property YT_SearchWidget searchWidget: YT_SearchWidget {
        id: searchWidget
        parent: root
        height: 500
    }

    readonly property Row pathView: Row {
        id: pathView
        parent: rootContent

        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true
        Layout.preferredHeight: rootItemSize

        spacing: YT_Info.SpacingSmall
        onWidthChanged: updateVisibleItem()

        property int collapseDatas: 0
        YT_PathWidget_Item {
            id: pathView_WidgetItem
            modelData: currentInfo.logoPath
            onClicked: {
                console.log(modelData);
            }
        }
        YT_Button {
            id: pathView_CollapseItem
            modelData: "..."
            verticalPadding: YT_Info.MarginSmall
            horizontalPadding: YT_Info.Margin
            implicitWidth: contentItem.implicitWidth + horizontalPadding * 2
            implicitHeight: pathView.height

            topInset: rootPadding
            bottomInset: rootPadding
            background.opacity: (hovered || down || pathView_Menu.parent == this)
            contentItem: YT_Button.LoadText {
                font.pointSize: Qt.application.font.pointSize - 2
            }
            onClicked: pathView_CollapseMenu.open()
        }
        YT_Menu {
            id: pathView_CollapseMenu
            parent: pathView_CollapseItem
            y: parent.height + YT_Info.SpacingSmall
            model: pathView.collapseDatas
            delegate: YT_Menu.YT_MenuItem {
                required property int index
                readonly property Item target: pathView_Instantiator.objectAt(index)
                text: target ? target.modelData : ""
                onTriggered: {
                    pathView_CollapseMenu.close();
                    currentInfo.pathModel.setSrcData(target.index);
                }
            }
        }
        function updateVisibleItem() {
            pathView.collapseDatas = 0;
            let view_size = pathView_WidgetItem.implicitWidth + pathView_CollapseItem.implicitWidth + pathView.spacing;
            for (let i = pathView_Instantiator.count - 1; i >= 0; i--) {
                const it = pathView_Instantiator.objectAt(i);
                view_size = view_size + it.implicitWidth + pathView.spacing;
                it.visible = view_size < pathView.width;
                pathView.collapseDatas = pathView.collapseDatas + (it.visible == false);
            }
            pathView_CollapseItem.visible = view_size >= pathView.width;
        }
    }

    readonly property Instantiator pathView_Instantiator: Instantiator {
        id: pathView_Instantiator
        model: currentInfo.pathModel.pathData
        delegate: pathView_Delegate

        onObjectAdded: function (index, object) {
            object.parent = pathView;
            if (index + 1 == pathView_Instantiator.model.length) {
                pathView.updateVisibleItem();
            }
        }
        onObjectRemoved: function (index, object) {
            object.parent = null;
        }
    }

    readonly property Component pathView_Delegate: YT_Button {
        id: pathView_Delegate
        required property int index
        required property var model
        readonly property int indicator_implicit: pathView.height * 0.4

        modelData: model.modelData
        verticalPadding: YT_Info.MarginSmall
        horizontalPadding: YT_Info.Margin
        implicitWidth: contentItem.implicitWidth + indicator_implicit + horizontalPadding * 2
        implicitHeight: pathView.height

        topInset: rootPadding
        bottomInset: rootPadding
        background.opacity: (hovered || down || pathView_Menu.parent == this)

        contentItem: YT_Button.LoadText {
            font.pointSize: Qt.application.font.pointSize - 2
        }
        indicator: YT_Button {
            enabled: false
            x: pathView_Delegate.contentItem.x + pathView_Delegate.contentItem.implicitWidth
            anchors.verticalCenter: pathView_Delegate.verticalCenter
            implicitWidth: pathView_Delegate.indicator_implicit
            implicitHeight: pathView_Delegate.indicator_implicit

            padding: 0
            modelData: "qrc:/Ui_Image/next_page.png"
            contentItem: YT_Button.LoadImage {}
            background: null
        }

        signal_YT: true
        onClicked_YT: function (eventPoint, button) {
            if (button === Qt.LeftButton) {
                currentInfo.pathModel.setSrcData(index);
            } else if (button === Qt.RightButton) {
                pathView_Menu.model = currentInfo.pathModel.getSubPath(index);
                pathView_Menu.popup(this, Qt.point(0, height + YT_Info.SpacingSmall));
            }
        }
    }

    readonly property YT_Menu pathView_Menu: YT_Menu {
        id: pathView_Menu
        width: Math.max(delegateWidth + padding * 2, parent ? parent.width : 0)
        height: delegateHeight + padding * 2
        delegate: YT_Menu.YT_MenuItem {
            required property int index
            required property string modelData
            text: modelData
            onTriggered: {
                const index = pathView_Menu.parent.index;
                currentInfo.pathModel.addSrcData(index, modelData);
            }
        }
        onClosed: {
            parent = null;
            pathView_MenuEditItem.contentItem.text = "";
        }

        YT_Menu.YT_MenuInputItem {
            id: pathView_MenuEditItem
            placeholderText: "EditPathData"
            onTriggered: {
                const index = pathView_Menu.parent.index;
                currentInfo.pathModel.addSrcData(index, '*' + text + '*');
            }
        }
        YT_Menu.YT_MenuSeparator {}
    }

    readonly property Row toolView: Row {
        id: toolView
        parent: rootContent
        Layout.row: 0
        Layout.column: 2
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight

        spacing: 0
        YT_PathWidget_Item {
            modelData: "qrc:/Ui_Image/minimize.png"
            onClicked: {
                Window.window.showMinimized();
            }
        }
        YT_PathWidget_Item {
            readonly property bool isMaximized: Window.window.visibility == Window.Maximized
            modelData: isMaximized ? "qrc:/Ui_Image/full_screen_close.png" : "qrc:/Ui_Image/full_screen.png"
            onClicked: {
                if (isMaximized) {
                    Window.window.showNormal();
                } else {
                    Window.window.showMaximized();
                }
            }
        }
        YT_PathWidget_Item {
            modelData: "qrc:/Ui_Image/close.png"
            onClicked: {
                Qt.quit();
            }
        }
    }

    component YT_PathWidget_Item: YT_Button {
        inset: rootPadding
        padding: YT_Info.Margin
        implicitWidth: rootItemSize
        implicitHeight: rootItemSize
        contentItem: YT_Button.LoadImage {}
    }
}
