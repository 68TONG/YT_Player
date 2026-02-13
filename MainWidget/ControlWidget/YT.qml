pragma Singleton

import QtQuick
import YT_Player

QtObject {
    property int widgetIndex: 0
    property YT_ScrollBarList mainScrollBar: null
    property YT_NavigationWidget navigationWidget: null

    property MusicWidget musicWidget: null
    property MusicItemModel musicItemModel: null
    property MusicListModel musicListModel: null

    property FileWidget fileWidget: null

    readonly property ObjectModel widgetModel: ObjectModel {
        QtObject {
            property string title: "音乐"
            property string logo_path: "qrc:/Resource_UI/music_logo.png"
            property MusicWidget widget: YT.musicWidget
            property list<string> widget_curPath: widget ? widget.curPath : []
        }
        QtObject {
            property string title: "文件"
            property string logo_path: "qrc:/Resource_UI/file_logo.png"
            property FileWidget widget: YT.fileWidget
            property list<string> widget_curPath: widget ? widget.curPath : []
        }
        QtObject {
            property string title: "壁纸"
            property string logo_path: "qrc:/Resource_UI/picture_logo.png"
            property Item widget: null
            property list<string> widget_curPath: widget ? widget.curPath : []
        }
        QtObject {
            property string title: "编辑"
            property string logo_path: "qrc:/Resource_UI/edit_logo.png"
            property Item widget: null
            property list<string> widget_curPath: widget ? widget.curPath : []
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
        const global_pos = data.mapToGlobal(pos)
        return _this.mapFromGlobal(global_pos)
    }

    function findParent(data, type) {
        if (data === null) return null
        if (data instanceof type) return data
        return findParent(data.parent, type)
    }

    component RectChangedSignal: Connections {
        property bool is
        function onXChanged() {
            is = !is
        }
        function onYChanged() {
            is = !is
        }
        function onWidthChanged() {
            is = !is
        }
        function onHeightChanged() {
            is = !is
        }
    }
}
