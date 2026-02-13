import QtQuick
import QtQuick.Controls
import YT_Player

Popup {
    id: root

    property var userData: null
    property var acceptedFunction: null
    property var cancelFunction: null

    property alias editItem: editItem
    property alias text: editItem.text
    property alias placeholderText: editItem.placeholderText
    function open_YT(acceptedFunction = null, cancelFunction = null) {
        this.acceptedFunction = acceptedFunction;
        this.cancelFunction = cancelFunction;

        this.focus = true;
        this.contentItem.focus = true;
        // this.contentItem.text = text
        // this.contentItem.selectAll()

        this.open();
    }

    enter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0.0
            to: 1.0
        }
    }
    exit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1.0
            to: 0.0
        }
    }

    onClosed: {
        if (root.acceptedFunction) {
            root.acceptedFunction(root.text, root.userData);
        }

        acceptedFunction = null;
        cancelFunction = null;

        x = 0;
        y = 0;
        userData = null;
    }

    anchors.centerIn: parent
    width: parent ? parent.width / 1.5 : 0
    height: parent ? parent.height / 1.5 : 0
    implicitWidth: parent ? parent.width / 1.5 : 0
    implicitHeight: parent ? parent.height / 1.5 : 0

    padding: 0
    background: YT_Rectangle {
        color: YT_Info.ItemFocusColor
        border.color: YT_Info.FontFocusColor
    }
    contentItem: ScrollView {
        TextArea {
            id: editItem
            background: null

            padding: 0
            placeholderTextColor: color
            color: YT_Info.FontColor
        }
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical: YT_ScrollBar {
            anchors.margins: 2
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }
}
