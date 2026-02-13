import QtQuick
import QtQuick.Controls
import YT_Player

ComboBox {
    id: root

    component YT_Delegate: AbstractButton {
        required property int index
        property bool is_current: ListView.view.currentIndex == index

        padding: YT_Info.MarginSmall
        indicator: null
        background: null
        contentItem: YT_Text {
            text: parent.text
            color: parent.is_current ? YT_Info.FontFocusColor : YT_Info.FontColor
        }
    }

    padding: 0
    indicator: null
    contentItem: YT_Delegate {
        enabled: false
        is_current: false
        text: parent.displayText
    }
    background: YT_Rectangle {
        color: YT_Info.ItemFocusColor
        opacity: (parent.hovered || parent.down || popup.visible)
    }
    popup: YT_PopupList {
        model: visible ? root.delegateModel : null
        view.snapMode: ListView.SnapToItem
        view.currentIndex: root.currentIndex
        view.maximumFlickVelocity: 0
        followBackground.followItem: view.itemAtIndex(root.highlightedIndex)
    }
    delegate: YT_Delegate {
        implicitWidth: parent ? parent.width : 0
        text: root.model[index]
    }
}
