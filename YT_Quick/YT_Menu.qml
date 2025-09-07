import QtQuick
import QtQuick.Controls.Basic

import YT_Player

Menu {
    id: root

    implicitWidth: 200
    implicitHeight: contentItem.implicitHeight + padding * 2

    property alias radius: rootBackground.radius
    padding: radius
    overlap: -radius / 2

    property alias model: instantiator.model
    readonly property Instantiator instantiator: Instantiator {
        id: instantiator
        model: null
        delegate: root.delegate
        onObjectAdded: function (index, object) {
            root.insertItem(index, object)
        }
        onObjectRemoved: function (index, object) {
            root.removeItem(object)
        }
    }

    component YT_MenuItem: MenuItem {
        padding: 4
        background: null
        contentItem: Text {
            text: parent.text
            color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
        }
    }

    delegate: YT_MenuItem {}
    background: Rectangle {
        id: rootBackground
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.BackgroundColor)
        border.color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)

        Rectangle {
            id: hoveredItemBackground

            property Item currentItem: currentIndex > -true ? root.itemAt(currentIndex) : null
            x: currentItem ? currentItem.x + root.padding : 0
            y: currentItem ? currentItem.y + root.padding : 0
            width: currentItem ? currentItem.width : 0
            height: currentItem ? currentItem.height : 0

            radius: parent.radius
            color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)
            Behavior on y {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad // 动画的缓动类型
                }
            }
        }
    }

    enter: Transition {
        NumberAnimation { property: "opacity"; duration: 300; from: 0.0; to: 1.0; easing.type: Easing.OutQuad}
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; duration: 300; from: 1.0; to: 0.0; easing.type: Easing.OutQuad}
    }
}
