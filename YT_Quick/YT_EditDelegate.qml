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
    function open_YT(parent, acceptedFunction = null, cancelFunction = null) {
        this.parent = parent;
        this.acceptedFunction = acceptedFunction;
        this.cancelFunction = cancelFunction;

        this.focus = true;
        this.contentItem.focus = true;
        // this.contentItem.text = text
        // this.contentItem.selectAll()

        this.open();
    }

    // enter: Transition {
    //     NumberAnimation { property: "opacity"; from: 0.0; to: 1.0}
    //     NumberAnimation { property: "y"; from: y - height; to: y}
    // }
    // exit: Transition {
    //     NumberAnimation { property: "opacity"; from: 1.0; to: 0.0}
    //     NumberAnimation { property: "y"; from: y; to: y - height}
    // }

    enter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0.0
            to: 1.0
        }
        NumberAnimation {
            property: "width"
            from: 0
            to: width
        }
    }
    exit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1.0
            to: 0.0
        }
        NumberAnimation {
            property: "width"
            from: width
            to: 0
        }
    }

    onClosed: {
        if (root.cancelFunction)
            root.cancelFunction();

        parent = null;
        acceptedFunction = null;
        cancelFunction = null;

        x = 0;
        y = 0;
        userData = null;

        text = "";
        placeholderText = "";
    }

    width: parent ? parent.width : width
    height: parent ? parent.height : height

    padding: 0
    background: YT_Rectangle {
        color: YT_Info.ItemFocusColor
        border.color: YT_Info.FontFocusColor
    }
    contentItem: TextField {
        id: editItem
        background: null

        placeholderTextColor: color
        color: YT_Info.FontColor

        padding: 0
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        onAccepted: {
            if (root.acceptedFunction) {
                root.acceptedFunction(root.text, root.userData);
            }

            root.cancelFunction = null;
            root.close();
        }
    }
}
