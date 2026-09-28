import QtQuick
import QtQuick.Controls.Basic
import YT_Material as YT_M
import YT_Player

Item {
    id: root

    implicitWidth: rootControl.implicitWidth
    implicitHeight: rootControl.implicitHeight

    property bool setPopup: false
    property bool setExpanded: false
    readonly property bool expanded: rootControl.hovered || setExpanded

    property real expandSize: 0
    property real collapseSize: 32 + rootControl.leftPadding + rootControl.rightPadding
    readonly property ObjectModel model: ObjectModel {}
    function updateModel() {
        const widget = YT.widgetInfos.get(YT.widgetIndex).widget;
        const items = widget && widget.navigationWidget_Items ? widget.navigationWidget_Items : [];

        // model.clear();
        for (let i = 1; i < model.count; i++) {
            const it = model.get(i) as Item;
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
        let type = 0;
        for (let i = 0; i < model.count; i++) {
            const it = model.get(i) as Item;
            type = Math.max(type, it.implicitWidth);
        }
        expandSize = type + rootControl.leftPadding + rootControl.rightPadding;
    }
    Connections {
        target: YT
        function onWidgetIndexChanged() {
            root.updateModel();
        }
    }
    Component.onCompleted: {
        YT.navigationWidget = this;
        model.append(mainView);
        updateModel();
    }

    Control {
        id: rootControl
        clip: true
        hoverEnabled: true
        parent: root.setPopup && (root.expanded || rootControl_ExpanedAni.running) ? Overlay.overlay : root

        padding: YT_M.Theme.Space75
        topPadding: YT_M.Theme.Space200
        width: implicitWidth
        height: root.height
        implicitWidth: root.expanded ? root.expandSize : root.collapseSize // Math.floor(Window.width / 6 / 60) * 60
        implicitHeight: rootView.contentHeight + topPadding + bottomPadding
        Behavior on width {
            NumberAnimation {
                id: rootControl_ExpanedAni
                duration: 240
                easing.type: Easing.OutCubic
            }
        }

        contentItem: ListView {
            id: rootView
            spacing: 7
            interactive: false
            model: root.model

            add: Transition {
                NumberAnimation {
                    properties: "opacity"
                    duration: 300
                    from: 0.0
                    to: 1.0
                    easing.type: Easing.InOutQuad
                }
            }
            remove: Transition {
                NumberAnimation {
                    properties: "opacity"
                    duration: 300
                    from: 1.0
                    to: 0.0
                    easing.type: Easing.InOutQuad
                }
            }
        }

        background: Rectangle {
            color: YT_M.Theme.SurfaceContainerHighest
            radius: root.setPopup && root.expanded ? YT_M.Theme.RadiusLarge : YT_M.Theme.RadiusMedium
            topLeftRadius: 0
            topRightRadius: radius
            bottomLeftRadius: 0
            bottomRightRadius: radius
            Behavior on radius {
                NumberAnimation {
                    duration: 240
                    easing.type: Easing.OutQuad
                }
            }
        }

        Loader {
            parent: Overlay.overlay
            active: rootControl.parent == Overlay.overlay
            sourceComponent: rootControl.Overlay.modal

            z: -true
            anchors.fill: parent
            opacity: (rootControl.width - root.collapseSize) / (root.expandSize - root.collapseSize)
        }

        Overlay.modal: Rectangle {
            color: YT_M.Theme.setAlphaF(YT_M.Theme.Scrim, 0.38)
            MouseArea {
                anchors.fill: parent
            }
        }

        Instantiator {
            model: root.model.count > 1 ? root.model.count - 1 : null
            delegate: Rectangle {
                required property int index
                readonly property Item target: root.model.get(index + 1)
                parent: target && target.parent ? target.parent : null
                anchors.left: parent ? target.left : undefined
                anchors.right: parent ? target.right : undefined
                anchors.bottom: parent ? target.top : undefined
                anchors.margins: 7
                anchors.bottomMargin: anchors.margins / 2
                visible: target.visible

                height: +true
                color: YT_M.Theme.OutlineVariant
            }
        }
    }

    M_View {
        id: mainView
        currentIndex: YT.widgetIndex
        onCurrentIndexChanged: YT.widgetIndex = currentIndex

        model: YT.widgetInfos.count
        delegate: M_ViewItem {
            readonly property variant model: YT.widgetInfos.get(index)
            m.text: model.title
            m.iconSource: model.logoPath
            m.onClicked: {
                if (ListView.view.currentIndex == index) {
                    ListView.view.closeVertical = !(ListView.view as M_View).closeVertical;
                    return;
                }
                YT.widgetIndex = index;
            }
        }
    }

    component M_View: YT_M.ListView {
        id: root_M_View
        width: parent ? parent.width : 0
        height: implicitHeight
        implicitWidth: contentWidth
        implicitHeight: contentHeight
        interactive: false

        readonly property YT_FollowBackground followBackground: YT_FollowBackground {
            parent: root_M_View.contentItem
            color: YT_M.Theme.SecondaryContainer
            radius: YT_M.Theme.RadiusSmall
            opacity: root_M_View.closeVertical || root_M_View.dragingItem ? 0.0 : 1.0
            followItem: {
                if (root_M_View.dragingItem) {
                    return root_M_View.dragingItem;
                } else if (root_M_View.hoveredItem) {
                    return root_M_View.hoveredItem;
                }
                return root_M_View.currentItem;
            }
        }

        property Item hoveredItem: null
        property Item dragingItem: null
        HoverHandler {
            parent: root_M_View
            target: root_M_View
            onHoveredChanged: {
                if (hovered == false) {
                    root_M_View.hoveredItem = null;
                }
            }
        }

        property bool closeVertical: false
        property bool closeHorizontal: YT.navigationWidget.expanded == false
        onCloseVerticalChanged: {
            scrollSizeAni.to = closeVertical ? currentItem.implicitHeight : implicitHeight;
            scrollSizeAni.start();
            if (closeVertical && currentIndex != 0)
                scrollOpacityAni.start();
        }
        NumberAnimation {
            id: scrollSizeAni
            target: root_M_View
            property: "height"
            from: root_M_View.height
            duration: 180
            easing.type: Easing.OutQuad
        }
        SequentialAnimation {
            id: scrollOpacityAni
            NumberAnimation {
                target: root_M_View
                property: "opacity"
                to: 0.0
                duration: 180
                easing.type: Easing.OutQuad
            }
            ScriptAction {
                script: {
                    root_M_View.positionViewAtIndex(root_M_View.currentIndex, ListView.Visible);
                }
            }
            NumberAnimation {
                target: root_M_View
                property: "opacity"
                to: 1.0
                duration: 380
                easing.type: Easing.InQuad
            }
        }
    }

    component M_ViewItem: YT_M.ListViewItemDrag {
        id: root_M_ViewItem
        required property int index

        width: ListView.view.width
        height: implicitHeight
        onImplicitWidthChanged: {
            const view = ListView.view;
            view.contentWidth = Math.max(view.contentWidth, implicitWidth);
        }

        dragEnabled: false
        dropEnabled: false

        m.width: width
        m.height: height
        m.implicitWidth: m.implicitContentWidth + m.leftPadding + m.rightPadding
        m.implicitHeight: 32

        m.radius: YT_M.Theme.RadiusSmall
        m.spacing: YT_M.Theme.Space50
        m.verticalPadding: YT_M.Theme.Space75
        m.horizontalPadding: YT_M.Theme.Space75

        m.indicator: Item {
            anchors.left: parent.left
            anchors.leftMargin: root_M_ViewItem.m.leftPadding
            anchors.verticalCenter: parent.verticalCenter
            implicitWidth: root_M_ViewItem.m.iconSize
            implicitHeight: root_M_ViewItem.m.iconSize
            readonly property bool isImageOrText: root_M_ViewItem.m.iconSource != null && root_M_ViewItem.m.iconSource != ""
            Image {
                visible: parent.isImageOrText == true
                anchors.fill: parent
                source: visible ? root_M_ViewItem.m.iconSource : ""
                fillMode: Image.PreserveAspectFit
            }
            Text {
                visible: parent.isImageOrText == false
                anchors.fill: parent
                font.bold: true
                font.pointSize: 11
                text: root_M_ViewItem.m.text.charAt(0)
                color: root_M_ViewItem.m.labelColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        m.contentItem: Text {
            visible: (root_M_ViewItem.ListView.view as M_View).closeHorizontal == false
            text: root_M_ViewItem.m.text
            font: root_M_ViewItem.m.font
            color: root_M_ViewItem.m.labelColor
            leftPadding: root_M_ViewItem.m.indicator.width + root_M_ViewItem.m.spacing
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
        m.background: YT_M.ControlBackground {
            radius: root_M_ViewItem.m.radius
            background.color: root_M_ViewItem.m.containerColor
            background.visible: background.opacity != 0.0
            background.opacity: root_M_ViewItem.m.draging

            defaultLayer.visible: false
            diffusionLayerColor: root_M_ViewItem.m.containerLayerColor
            diffusionLayerPoint: mapFromItem(root_M_ViewItem, root_M_ViewItem.m.pressX, root_M_ViewItem.m.pressY)
            diffusionLayerVisible: root_M_ViewItem.m.pressed
            elevation: root_M_ViewItem.m.elevation
        }
        m.onHoveredChanged: {
            if (m.hovered) {
                ListView.view.hoveredItem = this;
            }
        }
        m.onDragingChanged: {
            if (m.draging) {
                ListView.view.dragingItem = this;
            } else {
                ListView.view.dragingItem = null;
            }
            YT.navigationWidget.setExpanded = m.draging;
        }
    }
}
