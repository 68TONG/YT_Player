import QtQuick
import QtQuick.Controls
import YT_Player

Control {
    id: root
    readonly property bool expanded: hovered || setExpanded
    property bool setExpanded: false

    property int expandSize: 0
    property int collapseSize: 30
    readonly property ObjectModel model: ObjectModel {}
    function updateModel() {
        const widget = YT.widgetInfos.get(YT.widgetIndex).widget;
        const items = widget && widget.navigationWidget_Items ? widget.navigationWidget_Items : [];

        // model.clear();
        for (let i = 1; i < model.count; i++) {
            const it = model.get(i);
            it.implicitWidthChanged.disconnect(updateExpandSize);
        }
        if (model.count > 1) {
            model.remove(1, model.count - 1);
        }

        for (let it of items) {
            model.append(it);
            it.implicitWidthChanged.connect(updateExpandSize);
        }
        updateExpandSize();
    }
    function updateExpandSize() {
        expandSize = 0;
        for (let i = 0; i < model.count; i++) {
            const it = model.get(i);
            expandSize = Math.max(expandSize, it.implicitWidth);
        }
    }

    Connections {
        target: YT
        function onWidgetIndexChanged() {
            updateModel();
        }
    }
    Component.onCompleted: {
        YT.navigationWidget = this;
        model.append(mainView);
        updateModel();
    }

    clip: true
    padding: YT_Info.Margin
    hoverEnabled: true
    // Math.floor(Window.width / 6 / 60) * 60
    implicitWidth: (expanded ? expandSize : collapseSize) + padding * 2

    background: Rectangle {
        color: YT_Info.ItemColor
        radius: YT_Info.Radius
    }
    contentItem: ListView {
        id: rootView
        model: root.model
        spacing: YT_Info.Spacing
        interactive: false

        remove: Transition {
            NumberAnimation {
                properties: "opacity"
                duration: 300
                from: 1.0
                to: 0.0
                easing.type: Easing.InOutQuad
            }
        }
        add: Transition {
            NumberAnimation {
                properties: "opacity"
                duration: 300
                from: 0.0
                to: 1.0
                easing.type: Easing.InOutQuad
            }
        }
    }

    Instantiator {
        model: root.model.count > 1 ? root.model.count - 1 : null
        delegate: Rectangle {
            readonly property Item target: root.model.get(index + 1)
            parent: target && target.parent ? target.parent : null
            anchors.left: parent ? target.left : undefined
            anchors.right: parent ? target.right : undefined
            anchors.bottom: parent ? target.top : undefined
            anchors.margins: YT_Info.Margin
            anchors.bottomMargin: YT_Info.MarginSmall
            visible: target.visible
            // opacity: target.opacity
            height: +true
            color: YT_Info.ItemFocusColor
        }
    }

    readonly property Item mainView: YT_Delegate {
        id: mainView
        currentIndex: YT.widgetIndex
        onCurrentIndexChanged: YT.widgetIndex = currentIndex

        model: YT.widgetInfos.count
        delegate: YT_DelegateItem {
            readonly property QtObject model: YT.widgetInfos.get(index)
            textData: model.title
            imageData: model.logoPath

            onClicked: {
                if (ListView.view.currentIndex == index) {
                    ListView.view.closeVertical = !ListView.view.closeVertical;
                    return;
                }
                YT.widgetIndex = index;
            }
        }
    }

    component YT_Delegate: ListView {
        id: root_YT_Delegate
        width: parent ? parent.width : 0
        height: implicitHeight
        implicitWidth: contentWidth
        implicitHeight: contentHeight
        spacing: YT_Info.SpacingSmall
        interactive: false

        readonly property YT_FollowBackground followBackground: YT_FollowBackground {
            parent: root_YT_Delegate.contentItem
            opacity: root_YT_Delegate.closeVertical == false && followItem !== null
            followItem: root_YT_Delegate.hoverItem ? root_YT_Delegate.hoverItem : root_YT_Delegate.currentItem
        }

        property bool hovered: false
        property Item hoverItem: null
        HoverHandler {
            parent: root_YT_Delegate
            target: root_YT_Delegate
            onHoveredChanged: {
                root_YT_Delegate.hovered = hovered;
                if (hovered == false)
                    root_YT_Delegate.hoverItem = null;
            }
        }

        displaced: Transition {
            NumberAnimation {
                properties: "x,y"
                duration: 300
                easing.type: Easing.OutQuad
            }
        }
        move: Transition {
            NumberAnimation {
                properties: "opacity"
                duration: 300
                from: 0.0
                to: 1.0
                easing.type: Easing.InOutQuad
            }
        }

        property int closeSize: 30
        property bool closeVertical: false
        property bool closeHorizontal: YT.navigationWidget.expanded == false
        onCloseVerticalChanged: {
            scrollSizeAni.to = closeVertical ? closeSize : implicitHeight;
            scrollSizeAni.start();
            if (closeVertical && currentIndex != 0)
                scrollOpacityAni.start();
        }
        NumberAnimation {
            id: scrollSizeAni
            target: root_YT_Delegate
            property: "height"
            from: target.height
            duration: 180
            easing.type: Easing.OutQuad
        }
        SequentialAnimation {
            id: scrollOpacityAni
            NumberAnimation {
                target: root_YT_Delegate
                property: "opacity"
                to: 0.0
                duration: 180
                easing.type: Easing.OutQuad
            }
            ScriptAction {
                script: {
                    positionViewAtIndex(currentIndex, ListView.Visible);
                }
            }
            NumberAnimation {
                target: root_YT_Delegate
                property: "opacity"
                to: 1.0
                duration: 380
                easing.type: Easing.InQuad
            }
        }
    }
    component YT_DelegateItem: AbstractButton {
        id: root_YT_DelegateItem
        required property int index
        property string textData
        property string imageData
        property Component imageDisplay: Image {
            source: root_YT_DelegateItem.imageData
            asynchronous: true
            fillMode: Image.PreserveAspectFit
        }
        property Component textDisplay: YT_Text {
            text: root_YT_DelegateItem.textData.charAt(0)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        width: ListView.view.width
        height: implicitHeight
        implicitWidth: implicitContentWidth + indicator.width + spacing + padding * 2
        implicitHeight: ListView.view.closeSize
        onImplicitWidthChanged: {
            const view = ListView.view;
            view.contentWidth = Math.max(view.contentWidth, implicitWidth);
        }

        spacing: 0
        padding: YT_Info.Margin
        leftPadding: indicator.width + spacing

        background: null
        contentItem: YT_Text {
            text: root_YT_DelegateItem.textData
            visible: root_YT_DelegateItem.ListView.view.closeHorizontal == false
        }
        indicator: YT_Indicator {
            enabled: false
            edgeType: Qt.LeftEdge

            padding: root_YT_DelegateItem.padding
            background: null
            contentItem: Loader {
                sourceComponent: root_YT_DelegateItem.imageData != "" ? root_YT_DelegateItem.imageDisplay : root_YT_DelegateItem.textDisplay
            }
        }
        onHoveredChanged: {
            if (hovered)
                ListView.view.hoverItem = this;
        }
    }
}
