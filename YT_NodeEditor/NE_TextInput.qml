import QtQuick
import YT_Player

NE_Item {
    id: root

    dataNE_: ""
    typeNE_: NE_Data_Model.NE_String
    implicitWidth: parent ? parent.width : 0
    implicitHeight: contentItem.implicitHeight

    readonly property alias _NE: contentItem
    YT_SearchField {
        id: contentItem
        anchors.fill: parent

        background: YT_Rectangle {
            radius: YT_Info.RadiusSmall
            color: (parent.focus || parent.hovered) ? YT_Info.ItemFocusColor : YT_Info.ItemColor
        }
    }

    Binding {
        contentItem.text: root.dataNE_
    }
}
