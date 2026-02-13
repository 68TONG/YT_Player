import QtQuick
import QtQuick.Controls
import YT_Player

Window {
    id: root
    width: 200
    height: 50 /*+ controlWidget.anchors.margins * 2*/

    visible: true
    color: Qt.rgba(0, 0, 0, 0)
    flags: Qt.Window | Qt.FramelessWindowHint
    // Component.onCompleted: width = rootWidget.width

    YT_TransformWindow {
        target: Window.window
        enabled_type: 0
    }
    Rectangle {
        id: rootBackground
        width: controlWidget.empty ? height : controlWidget.implicitWidth
        // width: parent.width
        height: parent.height
        color: YT_Info.BackgroundColor
        radius: YT_Info.Radius
        // border.width: 2
        border.color: YT_Info.ItemFocusColor

        Behavior on width {
            id: rootBackgroundAnimation
            SequentialAnimation {
                ScriptAction {
                    script: {
                        const width = Math.max(rootBackgroundAnimation.targetValue, root.width)
                        root.setWidth(width)
                    }
                }
                NumberAnimation {
                    target: rootBackground
                    property: "width"
                    to: rootBackgroundAnimation.targetValue
                    duration: 500
                    easing.type: Easing.OutCubic
                }
                ScriptAction {
                    script: {
                        root.setWidth(rootBackgroundAnimation.targetValue)
                    }
                }
            }
        }
    }

    StackView {
        id: controlWidget

        parent: rootBackground
        anchors.fill: parent
        anchors.margins: YT_Info.MarginSmall
        implicitWidth: currentItem.implicitWidth + anchors.margins * 2

        initialItem : musicControl
        // Rectangle {
        //     id: ssss
        //     color: "red"
        //     opacity: 0.5
        //     implicitWidth: musicControl.implicitWidth
        //     implicitHeight: musicControl.implicitHeight

        // }
        YT_MusicControl {
            id: musicControl
            anchors.fill: parent
        }
    }
}
