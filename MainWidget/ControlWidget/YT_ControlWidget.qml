import QtQuick
import QtQuick.Layouts
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
        enabled_type: 0
    }
    Rectangle {
        id: rootBackground
        width: controlWidget.empty ? height : controlWidget.implicitWidth
        // width: parent.width
        height: parent.height
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.BackgroundColor)
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        // border.width: 2
        border.color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)

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
        anchors.margins: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)
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
