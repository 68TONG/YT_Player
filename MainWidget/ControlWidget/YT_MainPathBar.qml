import QtQuick
import QtQuick.Controls.Basic

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

        background.anchors.margins: rootPadding
        padding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

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
            view.positionViewAtEnd()
        }
    }

    readonly property Component pathBar_Delegate: YT_Button {
        id: pathBar_Delegate
        required property int index
        required property var model
        readonly property int indicator_implicit: pathBar.implicitHeight * 0.4

        modelData: model.modelData
        padding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)
        implicitWidth: contentItem.implicitWidth + indicator_implicit + horizontalPadding * 2
        implicitHeight: pathBar.implicitHeight

        background.opacity: (hovered || down || pathBar_Menu.parent === this)
        background.anchors.topMargin: rootPadding
        background.anchors.bottomMargin: rootPadding
        contentItem: YT_Button.LoadText {
            font.pointSize: parent.font.pointSize - 2
            color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)
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
        onClicked_YT: function(eventPoint, button) {
            if (button === Qt.LeftButton) {
                const widget = YT.widgetModel.get(currentIndex).widget
                widget.setCurPath(pathBar.model.slice(0, index + 1))
            } else if (button === Qt.RightButton) {
                const widget = YT.widgetModel.get(currentIndex).widget
                pathBar_Menu.model = widget.getSubPath(pathBar.model.slice(0, index + 1))
                pathBar_Menu.open_YT(pathBar_Delegate)
            }
        }
    }

    YT_PopupList {
        id: pathBar_Menu
        property int delegateWidth: 0

        x: 0
        y: parent ? parent.height + true * 2 : 0
        width: Math.max(delegateWidth, parent ? parent.width : 0)
        height: contentItem.implicitHeight * Math.min(5, contentItem.count) / contentItem.count

        model: null
        onModalChanged: delegateWidth = 0
        onClosed: parent = null

        delegate: YT_PopupList.YT_PopupList_Delegate {
            modelData: model.modelData
            width: parent ? parent.width : 0
            onImplicitWidthChanged: {
                pathBar_Menu.delegateWidth = Math.max(pathBar_Menu.delegateWidth, implicitWidth)
            }
            onClicked: {
                const index = pathBar_Menu.parent.index
                const widget = YT.widgetModel.get(currentIndex).widget
                var path = pathBar.model.slice(0, index + 1)
                path.push(modelData)

                widget.setCurPath(path)
                pathBar_Menu.close()
            }
        }
    }

    TapHandler {
        parent: pathBar.view
        onTapped: {
            editDelegate.open_YT(root, editDelegate.acceptedFunction, null)
        }
    }

    YT_EditDelegate {
        id: editDelegate

        text: {
            var path = ""
            for (var it of pathBar.model)
                path += it + "/"
            return path
        }

        padding: rootPadding
        topInset: padding
        leftInset: padding
        rightInset: padding
        bottomInset: padding

        function acceptedFunction(data) {
            YT.widgetModel.get(currentIndex).widget.setCurPath(data["acceptData"])
        }
    }
}
