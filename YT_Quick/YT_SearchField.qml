import QtQuick
import QtQuick.Controls

YT_TextInput {
    id: root
    onAccepted: focus = false
    onFocusChanged: {
        if (focus && model != null)
            popup.open();
        else
            popup.close();
    }

    text: clickedModelData
    property string clickedModelData
    property var model: null
    property Component delegate: YT_Button {
        required property int index
        required property var model

        implicitWidth: parent ? parent.width : 0
        modelData: model.modelData

        indicator: null
        background: null
        contentItem: YT_Button.LoadText {}

        onClicked: {
            root.clickedModelData = modelData;
            root.popup.close();
        }
        onHoveredChanged: {
            if (hovered)
                ListView.view.currentIndex = index;
        }
    }
    property Popup popup: YT_PopupList {
        model: root.model
        delegate: root.delegate
        onClosed: root.focus = false
    }
}
