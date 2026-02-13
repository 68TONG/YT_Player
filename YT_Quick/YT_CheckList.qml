import QtQuick
import QtQuick.Controls
import YT_Player

Control {
    id: root

    property string displayText
    property list<string> selectDatas: []
    signal itemSelectChanged(var data)

    padding: 0
    contentItem: YT_Button {
        modelData: parent.displayText
        indicator: null
        background: null
        contentItem: YT_Button.LoadText {}

        onClicked: popup.open()
    }
    background: YT_Rectangle {
        color: YT_Info.ItemFocusColor
        opacity: (parent.hovered || parent.down || popup.visible)
    }

    property var model: null
    property Component delegate: YT_Button {
        required property int index
        required property var model
        property bool selected: false

        implicitWidth: parent ? parent.width : 0
        modelData: model.modelData

        indicator: YT_Button.LoadSelectIndicator {}
        background: null
        contentItem: YT_Button.LoadText {}

        onClicked: selected = !selected
        onHoveredChanged: {
            if (hovered)
                ListView.view.currentIndex = index;
        }
        onSelectedChanged: {
            if (selected) {
                root.selectDatas.push(modelData);
            } else {
                root.selectDatas = root.selectDatas.filter(function (it) {
                    return it !== modelData;
                });
            }
            root.itemSelectChanged(this);
        }
    }
    property Popup popup: YT_PopupList {
        model: root.model
        delegate: root.delegate
    }
}
