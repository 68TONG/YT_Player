import QtQuick
import YT_Player

MouseArea {
    hoverEnabled: true
    anchors.fill: parent
    preventStealing: true
    propagateComposedEvents: true
    property bool enabled_move: true
    property int enabled_type: Qt.TopEdge | Qt.LeftEdge | Qt.RightEdge | Qt.BottomEdge
    property int transform_type: 0
    property int transform_margin: YT_Info.Margin

    property var target: parent
    property point global_pos: Qt.point(0, 0)
    property rect target_rect: Qt.rect(0, 0, 0, 0)

    onPressed: function (mouse) {
        if (enabled_move === false && transform_type === 0) {
            mouse.accepted = false;
            return;
        }
        if (target === Window.window) {
            YT_Func.transformWindow(target, transform_type);
        } else if (target !== null) {
            global_pos = mapToGlobal(Qt.point(mouseX, mouseY));
            target_rect = Qt.rect(target.x, target.y, target.width, target.height);
        } else {
            mouse.accepted = false;
            return;
        }
    }
    onPositionChanged: function (mouse) {
        if (mouse.buttons === Qt.NoButton) {
            var type = 0;
            if (mouse.y < transform_margin)
                type = type | Qt.TopEdge;
            if (mouse.x < transform_margin)
                type = type | Qt.LeftEdge;
            if (width - mouse.x < transform_margin)
                type = type | Qt.RightEdge;
            if (height - mouse.y < transform_margin)
                type = type | Qt.BottomEdge;
            type = enabled_type & type;

            if (transform_type !== type) {
                transform_type = type;
                if (type == 0)
                    cursorShape = Qt.ArrowCursor;
                else if (type == (Qt.TopEdge | Qt.LeftEdge))
                    cursorShape = Qt.SizeFDiagCursor;
                else if (type == (Qt.TopEdge | Qt.RightEdge))
                    cursorShape = Qt.SizeBDiagCursor;
                else if (type == (Qt.BottomEdge | Qt.LeftEdge))
                    cursorShape = Qt.SizeBDiagCursor;
                else if (type == (Qt.BottomEdge | Qt.RightEdge))
                    cursorShape = Qt.SizeFDiagCursor;
                else if (type == Qt.TopEdge)
                    cursorShape = Qt.SizeVerCursor;
                else if (type == Qt.LeftEdge)
                    cursorShape = Qt.SizeHorCursor;
                else if (type == Qt.RightEdge)
                    cursorShape = Qt.SizeHorCursor;
                else if (type == Qt.BottomEdge)
                    cursorShape = Qt.SizeVerCursor;
            }
            mouse.accepted = false;
            return;
        }

        if (target === null || target === Window.window) {
            mouse.accepted = false;
            return;
        }

        if (enabled_move === false && transform_type === 0) {
            mouse.accepted = false;
            return;
        }

        const pre_pos = mapFromGlobal(global_pos);
        const pos = Qt.point(mouseX - pre_pos.x, mouseY - pre_pos.y);

        var top = target_rect.y;
        var left = target_rect.x;
        var right = target_rect.x + target_rect.width;
        var bottom = target_rect.y + target_rect.height;

        if (transform_type == 0) {
            top = top + pos.y;
            left = left + pos.x;
            right = right + pos.x;
            bottom = bottom + pos.y;
        }
        if (transform_type & Qt.TopEdge) {
            top = top + pos.y;
        }
        if (transform_type & Qt.LeftEdge) {
            left = left + pos.x;
        }
        if (transform_type & Qt.RightEdge) {
            right = right + pos.x;
        }
        if (transform_type & Qt.BottomEdge) {
            bottom = bottom + pos.y;
        }

        if (transform_type & Qt.TopEdge && transform_margin > bottom - top) {
            top = bottom - transform_margin;
        }
        if (transform_type & Qt.LeftEdge && transform_margin > right - left) {
            left = right - transform_margin;
        }

        if (transform_type & Qt.TopEdge) {
            target.y = top;
            target.height = bottom - top;
        }
        if (transform_type & Qt.LeftEdge) {
            target.x = left;
            target.width = right - left;
        }
        if (transform_type & Qt.RightEdge) {
            target.width = right - left;
        }
        if (transform_type & Qt.BottomEdge) {
            target.height = bottom - top;
        }
    }
}
