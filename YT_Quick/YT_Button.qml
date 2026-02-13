import QtQuick
import QtQuick.Controls
import YT_Player

Button {
    id: root

    property int inset: 0
    topInset: inset
    leftInset: inset
    rightInset: inset
    bottomInset: inset

    padding: YT_Info.MarginSmall
    verticalPadding: padding
    horizontalPadding: padding

    property string modelData
    component LoadImage: Image {
        anchors.fill: parent
        anchors.margins: parent.padding

        smooth: true
        source: parent.modelData
        fillMode: Image.PreserveAspectFit
    }
    component LoadText: YT_Text {
        text: parent.modelData
    }
    component LoadSelectIndicator: Image {
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

    background: YT_Rectangle {
        color: YT_Info.ItemFocusColor
        opacity: (parent.hovered || parent.down)
    }

    property bool signal_YT: false
    TapHandler {
        enabled: signal_YT
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onTapped: function (eventPoint, button) {
            root.clicked_YT(eventPoint, button);
        }
    }
    signal clicked_YT(eventPoint eventPoint, int button)
}
