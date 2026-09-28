import QtQuick
import QtQuick.Controls.Basic
import YT_Player

AbstractButton {
    id: root
    required property string modelData
    property alias currentIndex: rootView.currentIndex
    property alias model: rootInstantiator.model
    property alias delegate: rootInstantiator.delegate

    topPadding: indicator.height + spacing
    indicator: Row {
        x: root.padding
        y: root.padding
        width: parent.width - 2 * root.padding
        height: implicitHeight
        spacing: YT_Info.SpacingSmall
        YT_Text {
            width: Math.min(parent.width - 2 * parent.height - 2 * parent.spacing, implicitWidth)
            height: implicitHeight
            text: root.modelData
        }
        YT_Button {
            width: parent.height
            height: parent.height
            modelData: "qrc:/Ui_Image/previous_page.png"
            contentItem: YT_Button.LoadImage {}
            onClicked: rootView.decrementCurrentIndex()
        }
        YT_Button {
            width: parent.height
            height: parent.height
            modelData: "qrc:/Ui_Image/next_page.png"
            contentItem: YT_Button.LoadImage {}
            onClicked: rootView.incrementCurrentIndex()
        }
    }
    contentItem: SwipeView {
        id: rootView
        clip: true
        onWidthChanged: root.updateVisibleItem()
    }
    background: null

    readonly property Component rootView_Delegate: Row {
        id: rootView_Delegate
        spacing: YT_Info.Spacing
    }

    Instantiator {
        id: rootInstantiator
        onObjectAdded: function (index, object) {
            // console.log("rootInstantiator.onObjectAdded", index, object, model.length);
            if (index + 1 == model.length) {
                root.updateVisibleItem();
            }
        }
        onObjectRemoved: function (index, object) {
            // console.log("rootInstantiator.onObjectRemoved", index, object, model.length);
            object.parent = null;
        }
    }

    function updateVisibleItem() {
        let container_index = 0;
        let container_size = 0;
        if (rootView.count == 0) {
            let container = rootView_Delegate.createObject(rootView);
            rootView.addItem(container);
        }

        for (let i = 0; i < rootInstantiator.count; i++) {
            let container = rootView.itemAt(container_index);
            const it = rootInstantiator.objectAt(i);
            const next_size = container_size + it.implicitWidth + YT_Info.Spacing;
            // console.log(i, rootView.count, rootView.width, next_size, container.children.length, container.width, container.implicitWidth, it.implicitWidth);
            if (container.children.length > 0 && next_size > rootView.width) {
                container_index = container_index + true;
                container_size = 0;
                if (container_index < rootView.count) {
                    container = rootView.itemAt(container_index);
                } else {
                    container = rootView_Delegate.createObject(rootView);
                    rootView.addItem(container);
                }
            }
            it.parent = container;
            container_size = container_size + it.implicitWidth + YT_Info.Spacing;
        }

        let removeItem_List = [];
        container_index = container_index + true;
        for (; container_index < rootView.count; container_index++) {
            removeItem_List.push(rootView.itemAt(container_index));
        }
        for (let it of removeItem_List) {
            rootView.removeItem(it);
        }
    }
}
