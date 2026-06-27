import QtQuick
import QtQuick.Controls
import YT_Player

Menu {
    id: root

    width: delegateWidth + padding * 2
    height: delegateHeight + padding * 2

    property int delegateWidth: 0
    property int delegateHeight: 0
    onAboutToShow: {
        delegateWidth = delegateHeight = 0;
        for (var i = 0; i < count; i++) {
            const it = root.itemAt(i);
            delegateWidth = Math.max(delegateWidth, it.implicitWidth);
            delegateHeight = delegateHeight + it.implicitHeight;
        }
    }

    property int radius: YT_Info.Radius
    padding: YT_Info.Margin
    overlap: -padding / 2

    property alias model: instantiator.model
    readonly property Instantiator instantiator: Instantiator {
        id: instantiator
        model: null
        delegate: root.delegate
        onObjectAdded: function (index, object) {
            root.addItem(object);
        }
        onObjectRemoved: function (index, object) {
            root.removeItem(object);
        }
    }

    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: root.contentItem.contentItem
        followItem: root.itemAt(root.currentIndex)
    }

    component YT_MenuItem: MenuItem {
        verticalPadding: YT_Info.MarginSmall
        horizontalPadding: YT_Info.Margin
        implicitWidth: implicitContentWidth + leftPadding + rightPadding
        implicitHeight: implicitContentHeight + topPadding + bottomPadding

        indicator: null
        background: null
        contentItem: YT_Text {
            text: parent.text
        }
    }

    component YT_MenuInputItem: YT_MenuItem {
        property string placeholderText: ""

        text: contentItem.text
        contentItem: YT_TextInput {
            padding: 0
            background: null
            placeholderText: parent.placeholderText
        }
    }

    component YT_MenuSelectItem: YT_MenuItem {
        required property bool selected

        indicator: YT_Indicator {
            enabled: false
            visible: parent.selected
            edgeType: Qt.RightEdge
            modelData: "qrc:/Ui_Image/ok.png"
            background: null
        }
        contentItem: YT_Text {
            text: parent.text
        }
    }

    component YT_MenuSeparator: MenuSeparator {
        width: parent ? parent.width : 0
        height: +true + verticalPadding * 2
        verticalPadding: YT_Info.MarginSmall
        horizontalPadding: YT_Info.Margin

        contentItem: Rectangle {
            color: YT_Info.ItemFocusColor
        }
    }

    delegate: YT_MenuItem {}
    background: Rectangle {
        radius: root.radius
        color: YT_Info.BackgroundColor
        border.color: YT_Info.ItemFocusColor
    }
    contentItem: ListView {
        model: root.contentModel
        implicitWidth: contentItem/*.childrenRect*/.width
        implicitHeight: contentItem/*.childrenRect*/.height
    }

    enter: Transition {
        NumberAnimation {
            property: "height"
            duration: 300
            from: 0.0
            to: root.height
            easing.type: Easing.OutQuad
        }
        NumberAnimation {
            property: "opacity"
            duration: 300
            from: 0.0
            to: 1.0
            easing.type: Easing.OutQuad
        }
    }
    exit: Transition {
        NumberAnimation {
            property: "opacity"
            duration: 300
            from: 1.0
            to: 0.0
            easing.type: Easing.OutQuad
        }
    }
}
