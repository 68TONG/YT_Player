import QtQuick
import QtQuick.Controls
import YT_Player

Control {
    id: root
    property int imageSize: 30
    property int delegateSize: 100
    readonly property ObjectModel model: ObjectModel {}
    Component.onCompleted: {
        model.append(mainView);
        YT.navigationWidget = this;
    }
    
    padding: YT_Info.Margin
    hoverEnabled: true
    implicitWidth: (expanded ? delegateSize : imageSize) + padding * 2
    Behavior on implicitWidth {
        NumberAnimation {
            duration: 180
            easing.type: Easing.OutQuad
        }
    }

    readonly property bool expanded: hovered || setExpanded
    property bool setExpanded: false

    background: Rectangle {
        color: YT_Info.ItemColor
        radius: YT_Info.Radius
    }
    contentItem: ListView {
        model: root.model
        spacing: YT_Info.Spacing
        interactive: false
    }

    Instantiator {
        model: root.model.count > 1 ? root.model.count - 1 : null
        delegate: Rectangle {
            readonly property Item target: root.model.get(index + 1)
            parent: target.parent
            height: +true
            anchors.left: target.left
            anchors.right: target.right
            anchors.bottom: target.top
            anchors.margins: YT_Info.Margin
            anchors.bottomMargin: YT_Info.MarginSmall
            color: YT_Info.ItemFocusColor
        }
    }

    readonly property Item mainView: YT_Delegate {
        id: mainView
        currentIndex: YT.widgetIndex
        onCurrentIndexChanged: YT.widgetIndex = currentIndex

        closeVertical: false
        closeHorizontal: root.expanded == false

        model: YT.widgetModel.count
        delegate: YT_DelegateItem {
            readonly property QtObject model: YT.widgetModel.get(index)
            textData: model.title
            imageData: model.logo_path

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
        implicitWidth: contentItem.width
        implicitHeight: contentItem.height

        clip: true
        spacing: YT_Info.SpacingSmall
        interactive: false

        property YT_FollowBackground followBackground: YT_FollowBackground {
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

        property bool closeVertical: false
        property bool closeHorizontal: false
        readonly property int closeSize: 30
        onCloseVerticalChanged: {
            scrollSizeAni.to = closeVertical ? closeSize : implicitHeight;
            scrollSizeAni.start();
            if (closeVertical && currentIndex != 0)
                scrollOpacityAni.start();
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
        property string textData
        property string imageData
        required property int index
        readonly property int closeSize: ListView.view.closeSize

        width: parent.width
        height: implicitHeight
        implicitWidth: implicitContentWidth + closeSize + spacing
        implicitHeight: closeSize

        padding: YT_Info.Margin
        spacing: YT_Info.SpacingSmall

        background: null
        contentItem: YT_Text {
            text: parent.textData
            visible: parent.ListView.view.closeHorizontal == false
            anchors.top: parent.top
            anchors.left: indicator.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: parent.padding
            anchors.leftMargin: parent.spacing
        }
        indicator: Item {
            x: parent.padding
            y: parent.padding
            width: parent.closeSize - parent.padding * 2
            height: parent.closeSize - parent.padding * 2
            Image {
                anchors.fill: parent
                visible: imageData != ""
                source: imageData
                smooth: true
                fillMode: Image.PreserveAspectFit
            }
            YT_Text {
                anchors.fill: parent
                visible: imageData == ""
                text: textData.charAt(0)
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        onHoveredChanged: {
            if (hovered)
                ListView.view.hoverItem = this;
        }
    }
}
