import QtQuick
import QtQuick.Controls
import YT_Player

Control {
    id: root

    property string displayText

    padding: 0
    contentItem: YT_Text {
        padding: YT_Info.MarginSmall
        color: YT_Info.FontColor
        text: parent.displayText
    }
    background: YT_Rectangle {
        color: YT_Info.ItemFocusColor
        opacity: (parent.hovered || parent.down || popup.visible)
    }

    property var model: null
    property Component delegate: YT_Text {
        required property int index
        property bool is_current: ListView.view.currentIndex == index

        padding: YT_Info.MarginSmall
        color: is_current ? YT_Info.FontFocusColor : YT_Info.FontColor
        text: parent.displayText
    }
    property Popup popup: YT_PopupList {
        model: root.model
        delegate: root.delegate
    }
}
