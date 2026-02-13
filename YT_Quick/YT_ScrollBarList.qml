import QtQuick
import QtQuick.Controls
import YT_Player

Item {
    id: root
    readonly property ScrollBar visibleItem: YT_ScrollBar {
        parent: root
        anchors.fill: parent

        enabled: false
        hoverEnabled: enabled
        policy: ScrollBar.AlwaysOn
        size: currentItem ? currentItem.size : 1.0
        position: currentItem ? currentItem.position : 0.0
        opacity: {
            if(currentItem === null) return 0.3
            if(currentItem.active) return 1.0
            return 0.7
        }
        Behavior on opacity {
            NumberAnimation {}
        }
        ParallelAnimation {
            id: itemAnimation
            NumberAnimation {
                id: itemAnimation_Size
                target: visibleItem

                duration: 250
                property: "size"
            }
            NumberAnimation {
                id: itemAnimation_Position
                target: visibleItem

                duration: 250
                property: "position"
            }
        }
    }
    property ScrollBar currentItem: null

    readonly property list<Flickable> targetList: []
    function addTarget (target) {
        if((target instanceof Flickable) === false) {
            console.error("ScrollBar addTarget target not Flickable, got:", typeof target);
            return
        }

        if(target.ScrollBar.vertical === null) {
            target.ScrollBar.vertical = Qt.createComponent("YT_ScrollBar.qml").createObject()
        }
        var scroll_bar = target.ScrollBar.vertical
        scroll_bar.parent = root
        scroll_bar.anchors.fill = root
        scroll_bar.anchors.margins = 0

        scroll_bar.opacity = 0.0
        scroll_bar.policy = ScrollBar.AlwaysOn
        scroll_bar.visible = Qt.binding(function () {return this === currentItem})
        scroll_bar.active = Qt.binding(function () {return target.moving || scroll_bar.pressed || scroll_bar.hovered})

        targetList.push(target)
        target.onFocusChanged.connect(function () {
            setCurrentItem(target.focus ? scroll_bar : null)
        })
        target.onMovementEnded.connect(function () {
            target.focus = true
        })
    }

    function setCurrentItem(target) {
        if(currentItem === target) return

        if(itemAnimation.running) itemAnimation.stop()

        itemAnimation_Size.to = target === null ? 1.0 : target.size
        itemAnimation_Size.from = visibleItem.size
        itemAnimation_Position.to = target === null ? 0.0 : target.position
        itemAnimation_Position.from = visibleItem.position
        currentItem = target

        itemAnimation.start()
    }
}
