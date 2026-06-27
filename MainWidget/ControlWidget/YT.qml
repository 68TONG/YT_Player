pragma Singleton

import QtQuick
import YT_Player
import HuskarUI.Basic

QtObject {
    objectName: "YT"
    property HusMessage mainMessage: null
    property YT_ScrollBarList mainScrollBar: null
    property YT_NavigationWidget navigationWidget: null

    property MusicWidget musicWidget: null
    property MusicFileModel musicFileModel: null
    property MusicListModel musicListModel: null

    property FileWidget fileWidget: null

    property int widgetIndex: 0
    readonly property ObjectModel widgetInfos: ObjectModel {
        YT_WidgetInfo {
            title: "音乐"
            widget: YT.musicWidget
            widgetName: "MusicWidget"
            logoPath: "qrc:/Ui_Image/music_logo.png"
        }
        YT_WidgetInfo {
            title: "文件"
            widget: YT.fileWidget
            widgetName: "FileWidget"
            logoPath: "qrc:/Ui_Image/file_logo.png"
        }
        YT_WidgetInfo {
            title: "壁纸"
            widget: null
            widgetName: "WallpaperWidget"
            logoPath: "qrc:/Ui_Image/picture_logo.png"
        }
        YT_WidgetInfo {
            title: "编辑"
            widget: null
            widgetName: "EditWidget"
            logoPath: "qrc:/Ui_Image/edit_logo.png"
        }
    }

    component YT_WidgetInfo: QtObject {
        property string title: ""
        property Item widget: null
        property string widgetName: ""
        property string logoPath: ""
        readonly property YT_PathWidgetModel pathModel: widget ? widget.pathModel : null
        readonly property YT_SearchWidgetModel searchModel: widget ? widget.searchModel : null
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
