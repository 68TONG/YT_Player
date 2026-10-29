pragma Singleton

import QtQuick
import YT_Player
import HuskarUI.Basic
import YT_Material as YT_M

QtObject {
    objectName: "YT"
    property HusMessage mainMessage: null
    property YT_ScrollBarList mainScrollBar: null
    property YT_NavigationWidget navigationWidget: null

    property Item currentWidget: null
    property MusicWidget musicWidget: null
    property MusicFileModel musicFileModel: null
    property MusicListModel musicListModel: null

    property FileWidget fileWidget: null

    property Item setWidget: null

    property Item testWidget: null

    property int widgetIndex: 0
    readonly property ObjectModel widgetInfos: ObjectModel {
        YT_WidgetInfo {
            title: "音乐"
            widget: YT.musicWidget
            widgetName: "MusicWidget"
            logoPath: "qrc:/Ui_Image/music_logo.png"
            logoIconPath: YT_M.IconProvider.LibraryMusic
        }
        YT_WidgetInfo {
            title: "文件"
            widget: YT.fileWidget
            widgetName: "FileWidget"
            logoPath: "qrc:/Ui_Image/file_logo.png"
            logoIconPath: YT_M.IconProvider.FolderCopy
        }
        YT_WidgetInfo {
            title: "编辑"
            widget: null
            widgetName: "EditWidget"
            logoPath: "qrc:/Ui_Image/edit_logo.png"
            logoIconPath: YT_M.IconProvider.EditSquare
        }
        YT_WidgetInfo {
            title: "设置"
            widget: YT.setWidget
            widgetName: "SetWidget"
            logoPath: "qrc:/Ui_Image/set_logo.png"
            logoIconPath: YT_M.IconProvider.Settings
        }
        YT_WidgetInfo {
            title: "测试"
            widget: YT.testWidget
            widgetName: "TestWidget"
            logoPath: "qrc:/Ui_Image/picture_logo.png"
            logoIconPath: YT_M.IconProvider.FrameBug
        }
    }

    component YT_WidgetInfo: QtObject {
        property string title: ""
        property Item widget: null
        property string widgetName: ""
        property string logoPath: ""
        property variant logoIconPath: ""
        readonly property YT_PathWidgetModel pathModel: widget ? widget.pathModel : null
        readonly property YT_SearchWidgetModel searchModel: widget ? widget.searchModel : null
    }

    readonly property ListModel widgetModel: ListModel {
        ListElement {
            title: "文件"
            logoIconPath: YT_M.IconProvider.FolderCopy
            widgetComponent: "Item.qml"
        }
        ListElement {
            title: "音乐"
            logoIconPath: YT_M.IconProvider.LibraryMusic
            widgetComponent: "Item.qml"
        }
        ListElement {
            title: "视频"
            logoIconPath: null
            widgetComponent: "Item.qml"
        }
        ListElement {
            title: "图片"
            logoIconPath: null
            widgetComponent: "Item.qml"
        }
        ListElement {
            title: "创作者"
            logoIconPath: null
            widgetComponent: "Item.qml"
        }
        ListElement {
            title: "编辑"
            logoIconPath: YT_M.IconProvider.EditSquare
            widgetComponent: "Item.qml"
        }
        ListElement {
            title: "设置"
            logoIconPath: YT_M.IconProvider.Settings
            widgetComponent: "Item.qml"
        }
        ListElement {
            title: "测试"
            logoIconPath: YT_M.IconProvider.FrameBug
            widgetComponent: "Item.qml"
        }
    }

    function formatTime(ms) {
        let totalSeconds = Math.floor(ms / 1000); // 将毫秒转换为秒
        let minutes = Math.floor(totalSeconds / 60); // 计算分钟
        let seconds = totalSeconds % 60; // 计算剩余秒数

        // 格式化为两位数
        let formattedMinutes = minutes.toString().padStart(2, '0');
        let formattedSeconds = seconds.toString().padStart(2, '0');

        return `${formattedMinutes}:${formattedSeconds}`;
    }

    function mapFromGlobal(_this, data, pos) {
        const global_pos = data.mapToGlobal(pos);
        return _this.mapFromGlobal(global_pos);
    }

    function findParent(data, type) {
        if (data === null)
            return null;
        if (data instanceof type)
            return data;
        return findParent(data.parent, type);
    }

    component RectChangedSignal: Connections {
        property bool is
        function onXChanged() {
            is = !is;
        }
        function onYChanged() {
            is = !is;
        }
        function onWidthChanged() {
            is = !is;
        }
        function onHeightChanged() {
            is = !is;
        }
    }
}
