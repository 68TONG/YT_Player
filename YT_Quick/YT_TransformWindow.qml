import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import YT_Player

MouseArea {
    hoverEnabled: true
    anchors.fill: parent
    property bool enabled_move: true
    property int enabled_type: Qt.TopEdge | Qt.LeftEdge | Qt.RightEdge | Qt.BottomEdge
    property int transform_type: 0
    property int transform_margin: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

    onPressed: {
        if (enabled_move === false && transform_type === 0) return
        YT_ConfigureInfo.transformWindow(Window.window, transform_type)
    }
    onPositionChanged: function(mouse) {
        var type = 0
        if (mouse.y < transform_margin) type = type | Qt.TopEdge
        if (mouse.x < transform_margin) type = type | Qt.LeftEdge
        if (width - mouse.x < transform_margin) type = type | Qt.RightEdge
        if (height - mouse.y < transform_margin) type = type | Qt.BottomEdge
        type = enabled_type & type

        if (transform_type !== type) {
            transform_type = type
            if (type == 0) cursorShape = Qt.ArrowCursor
            else if (type == (Qt.TopEdge | Qt.LeftEdge)) cursorShape = Qt.SizeFDiagCursor
            else if (type == (Qt.TopEdge | Qt.RightEdge)) cursorShape = Qt.SizeBDiagCursor
            else if(type == (Qt.BottomEdge | Qt.LeftEdge)) cursorShape = Qt.SizeBDiagCursor
            else if(type == (Qt.BottomEdge | Qt.RightEdge)) cursorShape = Qt.SizeFDiagCursor
            else if(type == Qt.TopEdge) cursorShape = Qt.SizeVerCursor
            else if(type == Qt.LeftEdge) cursorShape = Qt.SizeHorCursor
            else if(type == Qt.RightEdge) cursorShape = Qt.SizeHorCursor
            else if(type == Qt.BottomEdge) cursorShape = Qt.SizeVerCursor
        }
    }
}
