import QtQuick
import YT_Player

NE_Item {
    id: root

    component LoadText: YT_Button {
        anchors.fill: parent
        enabled: false
        modelData: parent.displayData
        contentItem: YT_Button.LoadText { }
        background: Rectangle {
            radius: YT_Info.RadiusSmall
            color: YT_Info.ItemColor
        }
    }

    implicitWidth: parent ? parent.width : 0
    implicitHeight: contentItem ? contentItem.implicitHeight : 0

    property var displayData
    required property Item contentItem
    onContentItemChanged: {
        if (contentItem && contentItem.parent === null)
            contentItem.parent = root
    }
}
