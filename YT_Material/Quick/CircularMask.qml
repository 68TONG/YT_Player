import QtQuick
import QtQuick.Effects

Item {
    id: root

    required property Item targetItem
    onTargetItemChanged: targetItem.parent = this
    property alias radius: maskItem.radius
    property alias topLeftRadius: maskItem.topLeftRadius
    property alias topRightRadius: maskItem.topRightRadius
    property alias bottomLeftRadius: maskItem.bottomLeftRadius
    property alias bottomRightRadius: maskItem.bottomRightRadius

    Rectangle {
        id: maskItem
        anchors.fill: root.targetItem
        visible: false
        color: "black"      // 黑色 = 不透明（保留），白色/透明 = 遮罩
        layer.enabled: true   // 关键！必须开启
    }

    MultiEffect {
        source: root.targetItem
        anchors.fill: root.targetItem

        maskEnabled: true
        maskSource: maskItem   // 指向遮罩源
    }
}
