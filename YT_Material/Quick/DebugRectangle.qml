import QtQuick
import QtQuick.Controls.Basic

Rectangle {
    id: root
    anchors.fill: target == parent || target == null ? parent : undefined
    opacity: 0.618
    color: Qt.hsla(Math.random(), 0.7, 0.5, 1)

    property Item target: null
    onTargetChanged: {
        if (target && target != parent) {
            parent = Qt.binding(function () {
                return Overlay.overlay;
            });
        }
    }

    Timer {
        running: root.target && root.target.visible && root.target != root.parent
        interval: 16
        repeat: true
        onTriggered: {
            const target = root.target;
            const globalPos = target.mapToGlobal(0, 0);
            const pos = Overlay.overlay.mapFromGlobal(globalPos.x, globalPos.y);

            root.x = pos.x;
            root.y = pos.y;
            root.width = target.width;
            root.height = target.height;
        }
    }
}
