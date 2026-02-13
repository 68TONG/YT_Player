import QtQuick
import YT_Player

NE_Item {
    id: root

    dataNE_: ""
    typeNE_: NE_Data_Model.NE_String
    implicitWidth: parent ? parent.width : 0
    implicitHeight: contentItem.implicitHeight

    YT_Button {
        id: contentItem
        anchors.fill: parent
        modelData: editDelegate.placeholderText
        contentItem: YT_Button.LoadText {}
        background: YT_Rectangle {
            radius: YT_Info.RadiusSmall
            color: parent.hovered ? YT_Info.ItemFocusColor : YT_Info.ItemColor
        }
        onClicked: editDelegate.open_YT()
    }

    readonly property alias _NE: editDelegate
    YT_EditAreaDelegate {
        id: editDelegate
        parent: root.viewNE_

        text: root.dataNE_
        editItem.onTextEdited: root.dataNE_ = text;
    }
}
