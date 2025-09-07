import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

import YT_Player

Popup {
    id: root

    property string message
    property var acceptedFunction: null
    property var cancelFunction: null
    function open_YT(message, acceptedFunction = null, cancelFunction = null) {
        this.message = message
        this.acceptedFunction = acceptedFunction
        this.cancelFunction = cancelFunction
        this.open()
    }

    onClosed: {
        if(root.cancelFunction) root.cancelFunction()

        message = ""
        acceptedFunction = null
        cancelFunction = null
    }

    enter: Transition {
        NumberAnimation { property: "opacity"; duration: 300; from: 0.0; to: 1.0; easing.type: Easing.OutQuad}
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; duration: 300; from: 1.0; to: 0.0; easing.type: Easing.OutQuad}
    }

    anchors.centerIn: parent
    width: parent ? parent.width / 2 : 0
    height: parent ? parent.height / 3 : 0

    contentItem: GridLayout {
        id: rootLayout
        rows: 2
        columns: 2
    }
    background: Rectangle {
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.BackgroundColor)
        border.color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)
    }

    Text {
        parent: rootLayout

        text: root.message
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        Layout.row: 0
        Layout.column: 0
        Layout.columnSpan: 2
        Layout.fillWidth: true
    }
    YT_Button {
        parent: rootLayout
        modelData: "取消"
        contentItem: YT_Button.LoadText {
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Layout.row: 1
        Layout.column: 0
        Layout.fillWidth: true

        onClicked: {
            root.close()
        }
    }
    YT_Button {
        parent: rootLayout
        modelData: "确定"
        contentItem: YT_Button.LoadText {
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Layout.row: 1
        Layout.column: 1
        Layout.fillWidth: true

        onClicked: {
            if(root.acceptedFunction) root.acceptedFunction()

            root.cancelFunction = null
            root.close()
        }
    }
}
