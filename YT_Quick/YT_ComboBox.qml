import QtQuick
import QtQuick.Controls.Basic

import YT_Player

ComboBox {
    id: root

    component TextItem: ItemDelegate {
        width: parent.width
        height: contentItem.implicitHeight

        padding: 0
        indicator: null
        background: null

        leftPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        rightPadding:  YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        contentItem: Text {
            text: parent.text
            color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
    }

    indicator: null
    contentItem: TextItem {
        enabled: false
        text: root.displayText
    }
    background: Rectangle {
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        color: (parent.hovered || parent.down)
               ? YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)
               : YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemColor)
    }
    popup: Popup {
        y: root.height + 2
        implicitWidth: root.implicitWidth
        implicitHeight: contentItem.implicitHeight

        padding: 0
        background: null
        contentItem: YT_ListView {
            clip: true
            implicitHeight: view.contentItem.height

            model: root.delegateModel
            view.currentIndex: root.highlightedIndex
            followBackground.followItem: view.currentItem
        }
    }
    delegate: TextItem { text: root.model[index] }
}
