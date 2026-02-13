import QtQuick
import YT_Player

Item {
    id: root

    property int rootPadding: 4
    property int currentIndex: YT.widgetIndex
    onCurrentIndexChanged: YT.widgetIndex = currentIndex

    YT_Button {
        id: curWidget
        implicitWidth: parent.height
        implicitHeight: parent.height

        inset: root.rootPadding
        padding: YT_Info.Margin

        modelData: YT.widgetModel.get(currentIndex).logo_path
        contentItem: YT_Button.LoadImage {}
    }

    YT_PathBar {
        id: pathBar
        color: Qt.rgba(0, 0, 0, 0)
        border.color: Qt.rgba(0, 0, 0, 0)

        x: curWidget.width
        implicitWidth: parent.width - x
        implicitHeight: parent.height

        view.spacing: rootPadding
        view.cacheBuffer: (Math.pow(2, 31) - 1)

        model: YT.widgetModel.get(currentIndex).widget_curPath
        delegate: pathBar_Delegate
        onModelChanged: {
            view.positionViewAtEnd();
        }
    }

    readonly property Component pathBar_Delegate: YT_Button {
        id: pathBar_Delegate
        required property int index
        required property var model
        readonly property int indicator_implicit: pathBar.implicitHeight * 0.4

        modelData: model.modelData
        padding: YT_Info.MarginSmall
        implicitWidth: contentItem.implicitWidth + indicator_implicit + horizontalPadding * 2
        implicitHeight: pathBar.implicitHeight

        topInset: rootPadding
        bottomInset: rootPadding
        background.opacity: (hovered || down || pathBar_Menu.parent === this)

        contentItem: YT_Button.LoadText {
            font.pointSize: Qt.application.font.pointSize - 2
            color: YT_Info.FontColor
        }
        indicator: YT_Button {
            enabled: false
            x: parent.contentItem.x + parent.contentItem.implicitWidth
            anchors.verticalCenter: parent.verticalCenter
            implicitWidth: parent.indicator_implicit
            implicitHeight: parent.indicator_implicit

            padding: 0
            modelData: "qrc:/Resource_UI/next_page.png"
            contentItem: YT_Button.LoadImage {}
            background: null
        }

        signal_YT: true
        onClicked_YT: function (eventPoint, button) {
            const widget = YT.widgetModel.get(currentIndex).widget;
            if (button === Qt.LeftButton) {
                widget.setCurPath(pathBar.model.slice(0, index + 1));
            } else if (button === Qt.RightButton) {
                pathBar_Menu.model = null;
                pathBar_Menu.delegateSize = 0;
                pathBar_Menu.model = widget.getSubPath(pathBar.model.slice(0, index + 1));
                pathBar_Menu.open_YT(pathBar_Delegate);
            }
        }
    }

    YT_PopupList {
        id: pathBar_Menu
        property int delegateSize: 0
        width: Math.max(delegateSize + padding * 2, parent ? parent.width : 0)

        // exit: null
        // enter: null
        // popupType: YT_PopupList.Window

        model: null
        onClosed: parent = null

        delegate: YT_Button {
            required property int index
            required property var model

            indicator: null
            background: null
            contentItem: YT_Button.LoadText {}

            modelData: model.modelData
            width: parent ? parent.width : 0
            onImplicitWidthChanged: {
                pathBar_Menu.delegateSize = Math.max(pathBar_Menu.delegateSize, implicitWidth);
            }
            onHoveredChanged: {
                if (hovered)
                    ListView.view.currentIndex = index;
            }
            onClicked: {
                const index = pathBar_Menu.parent.index;
                const widget = YT.widgetModel.get(currentIndex).widget;
                var path = pathBar.model.slice(0, index + 1);
                path.push(modelData);

                widget.setCurPath(path);
                pathBar_Menu.close();
            }
        }
    }

    TapHandler {
        parent: pathBar.view
        onTapped: {
            editDelegate.text = editDelegate.getPathData();
            editDelegate.open_YT(root, editDelegate.acceptedFunction, null);
        }
    }

    YT_EditDelegate {
        id: editDelegate

        text: getPathData()
        padding: rootPadding
        topInset: padding
        leftInset: padding
        rightInset: padding
        bottomInset: padding
        editItem.font.pointSize: Qt.application.font.pointSize - 2

        function getPathData() {
            var data = "";
            for (var it of pathBar.model)
                data += it + "/";
            return data;
        }
        function acceptedFunction(data, user_data) {
            YT.widgetModel.get(currentIndex).widget.setCurPath(data);
        }
    }
}
