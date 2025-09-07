import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

import YT_Player

Button {
    id: root

    property string modelData
    component LoadImage: Image {
        anchors.fill: parent
        anchors.margins: parent.padding

        smooth: true
        source: parent.modelData
        fillMode: Image.PreserveAspectFit
    }
    component LoadText: Text {
        text: parent.modelData
        color: link_hovered ? "#FFD700" : YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)
        font.underline: link_hovered

        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter

        property bool link_YT: false
        property bool link_hovered: false
        MouseArea {
            x: parent.leftPadding
            y: parent.topPadding
            width: parent.contentWidth
            height: parent.contentHeight
            enabled: parent.link_YT
            hoverEnabled: true
            // Rectangle {
            //     anchors.fill: parent
            //     color: "red"
            //     opacity: 0.5
            // }

            onEntered: parent.link_hovered = true
            onExited: parent.link_hovered = false
            onClicked: parent.linkActivated(parent.text)
        }
    }

    background: Rectangle {
        anchors.fill: parent
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)

        opacity: (parent.hovered || parent.down)
        visible: opacity !== 0

        Behavior on opacity {
            NumberAnimation { duration: 300; easing.type: Easing.OutInQuad}
        }
    }

    property bool signal_YT: false
    TapHandler {
        enabled: signal_YT
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onTapped: function (eventPoint, button) {
            root.clicked_YT(eventPoint, button)
        }
    }
    signal clicked_YT(eventPoint eventPoint, int button)
}
