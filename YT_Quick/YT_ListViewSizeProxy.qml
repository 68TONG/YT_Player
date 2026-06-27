import QtQuick
import YT_Player

QtObject {
    required property Item target
    readonly property list<Item> targetItems: target.children
    onTargetItemsChanged: console.log(targetItems)
}