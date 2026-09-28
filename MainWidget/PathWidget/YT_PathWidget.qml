pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import YT_Material as YT_M
import YT_Player

Control {
    id: root

    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: 32 + topPadding + bottomPadding

    verticalPadding: YT_M.Theme.Space75
    horizontalPadding: YT_M.Theme.Space100

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
        target: root.currentInfo.pathModel
        function onSrcDataChanged() {
            historicalPath.addPath(root.currentInfo.pathModel.getData_WidgetPath());
        }
    }

    contentItem: GridLayout {
        id: rootContent
        rows: 1
        rowSpacing: 0
        columns: 3
        columnSpacing: YT_M.Theme.Space75 + true
    }

    Row {
        id: navigationView
        parent: rootContent
        Layout.row: 0
        Layout.column: 0
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight

        spacing: 0
        M_Item {
            iconSource: YT_M.IconProvider.KeyboardArrowLeft
            onClicked: historicalPath.setPreviousPath()
        }
        M_Item {
            iconSource: YT_M.IconProvider.KeyboardArrowRight
            onClicked: historicalPath.setNextPath()
        }
        M_Item {
            iconSource: YT_M.IconProvider.Search
            // onClicked: searchWidget.visible ? searchWidget.close() : searchWidget.open();
            // Rectangle {
            //     width: +true
            //     radius: +true
            //     color: YT_M.Theme.OutlineVariant

            //     anchors.top: parent.top
            //     anchors.left: parent.right
            //     anchors.bottom: parent.bottom
            //     anchors.margins: rootContent.columnSpacing
            //     anchors.leftMargin: rootContent.columnSpacing / 2
            // }
        }
    }

    Item {
        parent: rootContent

        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true
        Layout.preferredHeight: pathView.implicitHeight
        onWidthChanged: pathView.updateVisibleItem()

        Row {
            id: pathView
            anchors.fill: parent

            M_Item {
                id: pathView_WidgetItem
                implicitWidth: implicitHeight
                padding: YT_M.Theme.Space100
                iconSize: implicitHeight - topPadding - bottomPadding
                iconSource: root.currentInfo.logoPath
                background: null
                onClicked: console.log(iconSource)
            }

            property int collapseDatas: 0
            M_Item {
                id: pathView_CollapseItem
                topInset: YT_M.Theme.Space50
                bottomInset: YT_M.Theme.Space50
                iconSource: YT_M.IconProvider.MoreHoriz
                onClicked: pathView_CollapseMenu.open()
            }

            YT_M.Menu {
                id: pathView_CollapseMenu
                m_isGetImplicitContentSize: true
                y: parent.height + YT_M.Theme.Space50
                parent: pathView_CollapseItem
                model: pathView.collapseDatas
                delegate: YT_M.MenuItem {
                    required property int index
                    readonly property variant target: pathView_Instantiator.objectAt(index)
                    text: target ? target.modelData : ""
                    verticalPadding: YT_M.Theme.Space75
                    horizontalPadding: YT_M.Theme.Space125
                    onTriggered: {
                        menu.close();
                        root.currentInfo.pathModel.setSrcData(target.index);
                    }
                }
            }

            function updateVisibleItem() {
                pathView.collapseDatas = 0;
                let view_size = pathView_WidgetItem.implicitWidth + pathView_CollapseItem.implicitWidth + pathView.spacing;
                for (let i = pathView_Instantiator.count - 1; i >= 0; i--) {
                    const it = pathView_Instantiator.objectAt(i) as Item;
                    view_size = view_size + it.implicitWidth + pathView.spacing;
                    it.visible = view_size < pathView.width;
                    pathView.collapseDatas = pathView.collapseDatas + (it.visible == false);
                }
                pathView_CollapseItem.visible = view_size >= pathView.width;
            }
        }

        Rectangle {
            z: -true
            anchors.fill: parent
            color: YT_M.Theme.SurfaceContainerLowest
            radius: YT_M.Theme.RadiusFull
        }
    }

    Instantiator {
        id: pathView_Instantiator
        model: root.currentInfo.pathModel.pathData
        delegate: root.pathView_Delegate

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

    readonly property Component pathView_Delegate: YT_M.Button {
        id: pathView_Delegate
        required property int index
        required property string modelData

        sizeType: YT_M.Button.Xsmall
        text: modelData
        font.pointSize: 8
        spacing: YT_M.Theme.Space0
        topInset: YT_M.Theme.Space50
        bottomInset: YT_M.Theme.Space50
        leftPadding: YT_M.Theme.Space100
        rightPadding: YT_M.Theme.Space25
        labelColor: YT_M.Theme.Secondary
        containerColor: YT_M.Theme.Transparent
        containerLayerColor: YT_M.Theme.OnSecondaryContainer
        iconSource: YT_M.IconProvider.KeyboardArrowRight

        indicator: YT_M.Icon {
            anchors.right: pathView_Delegate.right
            anchors.rightMargin: pathView_Delegate.rightPadding
            anchors.verticalCenter: pathView_Delegate.verticalCenter

            iconSize: pathView_Delegate.iconSize
            iconColor: pathView_Delegate.iconColor
            iconSource: pathView_Delegate.iconSource
        }

        contentItem: Text {
            text: pathView_Delegate.text
            font: pathView_Delegate.font
            color: pathView_Delegate.labelColor
            rightPadding: pathView_Delegate.indicator.width + pathView_Delegate.spacing
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onTapped: function (eventPoint, button) {
                if (button == Qt.LeftButton) {
                    root.currentInfo.pathModel.setSrcData(pathView_Delegate.index);
                } else if (button == Qt.RightButton) {
                    pathView_Menu.model = root.currentInfo.pathModel.getSubPath(pathView_Delegate.index);
                    pathView_Menu.popup(pathView_Delegate);
                }
            }
        }
    }

    YT_M.Menu {
        id: pathView_Menu
        m_isGetImplicitContentSize: true
        y: parent ? parent.height + YT_M.Theme.Space50 : 0
        implicitWidth: Math.max(m_implicitContentWidth + leftPadding + rightPadding, parent ? parent.width : 0)
        implicitHeight: m_implicitContentHeight + topPadding + +bottomPadding
        delegate: YT_M.MenuItem {
            required property string modelData
            text: modelData
            verticalPadding: YT_M.Theme.Space75
            horizontalPadding: YT_M.Theme.Space125
            onTriggered: {
                const index = pathView_Menu.mTarget.index;
                root.currentInfo.pathModel.addSrcData(index, modelData);
            }
        }
        onClosed: {
            parent = null;
            pathView_MenuEditItemText.text = "";
        }

        YT_M.MenuItem {
            id: pathView_MenuEditItem
            implicitHeight: implicitContentHeight + topPadding + bottomPadding
            verticalPadding: YT_M.Theme.Space75
            horizontalPadding: YT_M.Theme.Space125

            contentItem: YT_M.TextField {
                id: pathView_MenuEditItemText
                placeholderText: "EditPathData"
                implicitHeight: inputText.implicitHeight + 11 + topPadding + bottomPadding
                verticalPadding: 0
                horizontalPadding: 0
                background: null

                inputText.font: YT_M.Theme.FontBodySmall
                labelText.font.pixelSize: inputText.activeFocus ? 8 : inputText.font.pixelSize
            }
            onTriggered: {
                const index = pathView_Menu.mTarget.index;
                root.currentInfo.pathModel.addSrcData(index, '*' + pathView_MenuEditItemText.text + '*');
            }
        }

        YT_M.MenuSeparator {}
    }

    Row {
        id: toolView
        parent: rootContent
        Layout.row: 0
        Layout.column: 2
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight

        M_Item {
            iconSource: YT_M.IconProvider.Remove
            onClicked: Window.window.showMinimized()
        }
        M_Item {
            readonly property bool isMaximized: Window.window && Window.window.visibility == Window.Maximized
            iconSource: isMaximized ? YT_M.IconProvider.FullscreenExit : YT_M.IconProvider.Fullscreen
            onClicked: isMaximized ? Window.window.showNormal() : Window.window.showMaximized()
        }
        M_Item {
            iconSource: YT_M.IconProvider.Close
            onClicked: Qt.quit()
        }
    }

    component M_Item: YT_M.IconButton {
        sizeType: YT_M.IconButton.Xsmall
        widthType: YT_M.IconButton.Narrow
        labelColor: YT_M.Theme.Secondary
        containerColor: YT_M.Theme.Transparent
        containerLayerColor: YT_M.Theme.OnSecondaryContainer
    }
}
