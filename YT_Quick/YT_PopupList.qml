import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

import YT_Player

Popup {
    id: root
    property alias model: rootView.model
    property alias delegate: rootView.delegate
    function open_YT(target) {
        parent = target
        open()
    }

    enter: Transition {
        NumberAnimation { property: "height"; duration: 300; from: 0.0; to: height; easing.type: Easing.OutQuad}
        NumberAnimation { property: "opacity"; duration: 300; from: 0.0; to: 1.0; easing.type: Easing.OutQuad}
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; duration: 300; from: 1.0; to: 0.0; easing.type: Easing.OutQuad}
    }

    padding: 0
    property int followItemPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small) * 2
    readonly property YT_FollowBackground followBackground: YT_FollowBackground {
        parent: rootView.contentItem
        followItem: rootView.currentItem
        widthPadding: followItemPadding
        heightPadding: followItemPadding
    }

    background: Rectangle {
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.BackgroundColor)
        border.color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)
    }
    contentItem: ListView {
        id: rootView
        clip: true
        implicitWidth: contentItem/*.childrenRect*/.width
        implicitHeight: contentItem/*.childrenRect*/.height

        highlightFollowsCurrentItem: false

        model: null
        delegate: YT_PopupList_Delegate {}
    }

    component YT_PopupList_Delegate: YT_Button {
        required property int index
        required property var model
        horizontalPadding: verticalPadding * 2
        verticalPadding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small) * 2

        indicator: null
        background: null
        contentItem: YT_Button.LoadText {}
        onHoveredChanged: if (hovered) ListView.view.currentIndex = index
    }
    component YT_SelectedIndicator: Image {
        visible: parent.selected
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.margins: parent.verticalPadding + 2
        anchors.rightMargin: parent.horizontalPadding

        smooth: true
        source: "qrc:/Resource_UI/ok.png"
        fillMode: Image.PreserveAspectFit
    }
}
