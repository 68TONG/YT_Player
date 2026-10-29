import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT_M

T.Control {
    id: root

    property alias labelText: labelText
    property alias inputText: inputText

    property alias text: inputText.text
    property alias placeholderText: labelText.text
    contentItem: FocusScope {
        implicitWidth: Math.max(labelText.implicitWidth, inputText.implicitWidth)
        implicitHeight: labelText.implicitHeight + inputText.implicitHeight
        Text {
            id: labelText
            y: inputText.activeFocus ? 0 : (parent.height - height) / 2
            text: "LabelText"
            font.pixelSize: inputText.activeFocus ? 12 : 16
            color: root.labelColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            // lineHeight: inputText.activeFocus ? 16 : 24
            // lineHeightMode: Text.FixedHeight

            Behavior on y {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.OutQuad
                }
            }
            // Behavior on lineHeight {
            //     NumberAnimation {
            //         duration: 100
            //         easing.type: Easing.OutQuad
            //     }
            // }
            Behavior on font.pixelSize {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad
                }
            }
            Behavior on color {
                ColorAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad
                }
            }
        }
        TextInput {
            id: inputText
            anchors.bottom: parent.bottom

            focus: true
            font: YT_M.Font.BodyLarge
            // font.pixelSize: 16
            // font.letterSpacing: 0.5
            color: root.inputColor
            visible: opacity != 0.0
            opacity: activeFocus
            Behavior on opacity {
                NumberAnimation {
                    duration: 120
                    easing.type: Easing.OutQuad
                }
            }

            // cursorDelegate: Rectangle {
            //     width: 1
            //     height: parent.height
            //     color: YT_M.Theme.OnSurface
            // }
        }
    }

    background: Rectangle {
        color: root.containerColor
        topLeftRadius: YT_M.Theme.RadiusExtraSmall
        topRightRadius: YT_M.Theme.RadiusExtraSmall

        Rectangle {
            anchors.fill: parent
            topLeftRadius: parent.topLeftRadius
            topRightRadius: parent.topRightRadius
            bottomLeftRadius: parent.bottomLeftRadius
            bottomRightRadius: parent.bottomRightRadius
            visible: root.hovered
            opacity: 0.08
            color: root.containerLayerColor
        }

        Rectangle {
            width: parent.width
            height: root.outlineWidth
            anchors.top: parent.bottom
            color: root.outlineColor

            Behavior on height {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad
                }
            }
            Behavior on color {
                ColorAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad
                }
            }
        }
    }

    TapHandler {
        onTapped: root.contentItem.focus = true
    }
    Connections {
        target: inputText
        function onActiveFocusChanged() {
            if (inputText.activeFocus == false)
                root.contentItem.focus = false;
        }
    }

    spacing: YT_M.Theme.Space0
    verticalPadding: YT_M.Theme.Space100
    horizontalPadding: YT_M.Theme.Space200
    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: 56

    property real outlineWidth: inputText.activeFocus ? 3 : 1
    property color outlineColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.10);
        }
        if (inputText.activeFocus) {
            return YT_M.Theme.Primary;
        } else if (hovered) {
            return YT_M.Theme.OnSurface;
        }
        return YT_M.Theme.OnSurfaceVariant;
    }

    property real iconSize: 24
    property color iconColor: labelColor
    property variant iconSource

    property color inputColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
        }
        return YT_M.Theme.OnSurface;
    }
    property color labelColor: {
        if (enabled == false) {
            return YT_M.Theme.OnSurface;
        }
        if (inputText.activeFocus) {
            return YT_M.Theme.Primary;
        }
        return YT_M.Theme.OnSurfaceVariant;
    }
    property color containerColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.04);
        }
        return YT_M.Theme.SurfaceContainerHighest;
    }
    property color containerLayerColor: YT_M.Theme.OnSurface

    // property font supportingTextFont: Qt.font({
    //     family: "Roboto",
    //     pixelSize: 12,
    //     weight: Font.Normal,
    //     letterSpacing: 0.4
    // })
    // property real supportingTextLineHeight: 16
}
