import QtQuick
import QtQuick.Controls.Basic

import YT_Player

Popup {
    id: root

    property var modelData: null
    property var acceptedFunction: null
    property var cancelFunction: null

    property alias text: editItem.text
    property alias placeholderText: editItem.placeholderText
    function open_YT(parent, acceptedFunction = null, cancelFunction = null) {
        this.parent = parent
        this.acceptedFunction = acceptedFunction
        this.cancelFunction = cancelFunction

        this.focus = true
        this.contentItem.focus = true
        // this.contentItem.text = text
        // this.contentItem.selectAll()

        this.open()
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
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0}
        NumberAnimation { property: "width"; from: 0; to: width}
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0}
        NumberAnimation { property: "width"; from: width; to: 0}
    }

    Component.onCompleted: {
        modelData = {}
        modelData["acceptData"] = ""
    }
    onClosed: {
        if(root.cancelFunction) root.cancelFunction()
        parent = null
        acceptedFunction = null
        cancelFunction = null

        modelData = {}
        modelData["acceptData"] = ""
        placeholderText = ""
        x = 0
        y = 0
    }

    width: parent ? parent.width : width
    height: parent ? parent.height : height

    padding: 0
    background: Rectangle {
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)
        border.color: "#FFD700"
    }
    contentItem: TextField {
        id: editItem
        background: null

        placeholderTextColor: color
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)

        padding: 0
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        onAccepted: {
            if (root.acceptedFunction) {
                root.modelData["acceptData"] = text
                root.acceptedFunction(root.modelData)
            }

            root.cancelFunction = null
            root.close()
        }
    }
}
