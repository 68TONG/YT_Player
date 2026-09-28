import QtQuick
import YT_Material as YT

Item {
    id: root

    property alias button: button
    property alias trailingButton: trailingButton

    property alias text: button.text
    property alias iconSource: button.iconSource
    property alias sizeType: button.sizeType
    property alias styleType: button.styleType
    property alias labelColor: button.labelColor
    property alias containerColor: button.containerColor
    property alias containerLayerColor: button.containerLayerColor

    implicitWidth: button.implicitWidth + spacing + trailingButton.implicitWidth
    implicitHeight: button.implicitHeight

    YT.Button {
        id: button

        radius: 0
        topLeftRadius: YT.Theme.RadiusFull
        topRightRadius: innerRadius
        bottomLeftRadius: YT.Theme.RadiusFull
        bottomRightRadius: innerRadius
        property real innerRadius: {
            if (hovered || pressed) {
                return root.innerRadiusFocus;
            }
            return root.innerRadius;
        }

        leftPadding: {
            return horizontalPadding;
        }
        rightPadding: {
            if (sizeType == YT.Button.Xsmall) {
                return YT.Theme.Space125;
            } else if (sizeType == YT.Button.Small) {
                return YT.Theme.Space150;
            }
            return horizontalPadding;
        }
    }

    YT.IconButton {
        id: trailingButton

        checkable: true
        contentItem.rotation: -90.0

        anchors.left: button.right
        anchors.leftMargin: root.spacing

        radius: 0
        topLeftRadius: innerRadius
        topRightRadius: YT.Theme.RadiusFull
        bottomLeftRadius: innerRadius
        bottomRightRadius: YT.Theme.RadiusFull
        property real innerRadius: {
            if (checked) {
                return YT.Theme.RadiusFull;
            } else if (hovered || pressed) {
                return root.innerRadiusFocus;
            }
            return root.innerRadius;
        }

        iconSource: YT.IconProvider.KeyboardArrowLeft
        sizeType: root.sizeType
        styleType: root.styleType
        labelColor: root.labelColor
        containerColor: root.containerColor
        containerLayerColor: root.containerLayerColor

        iconSize: {
            if (sizeType == YT.Button.Xsmall || sizeType == YT.Button.Small) {
                return 22;
            } else if (sizeType == YT.Button.Medium) {
                return 26;
            } else if (sizeType == YT.Button.Large) {
                return 38;
            } else if (sizeType == YT.Button.Xlarge) {
                return 50;
            }
            return 0;
        }

        horizontalPadding: {
            if (sizeType == YT.Button.Xsmall || sizeType == YT.Button.Small) {
                return 13;
            } else if (sizeType == YT.Button.Medium) {
                return 15;
            } else if (sizeType == YT.Button.Large) {
                return 29;
            } else if (sizeType == YT.Button.Xlarge) {
                return 43;
            }
            return 0;
        }

        states: State {
            name: "checked"
            when: trailingButton.checked
            PropertyChanges {
                trailingButton.contentItem.rotation: 90.0
            }
        }
        transitions: Transition {
            NumberAnimation {
                property: "rotation"
                easing.type: Easing.OutCubic
                duration: 180
            }
        }
    }

    property real spacing: 2
    property real innerRadius: {
        if (sizeType == YT.Button.Xsmall || sizeType == YT.Button.Small || sizeType == YT.Button.Medium) {
            return 4;
        } else if (sizeType == YT.Button.Large) {
            return 8;
        } else if (sizeType == YT.Button.Xlarge) {
            return 12;
        }
        return 0;
    }
    property real innerRadiusFocus: {
        if (sizeType == YT.Button.Xsmall) {
            return 8;
        } else if (sizeType == YT.Button.Small || sizeType == YT.Button.Medium) {
            return 12;
        } else if (sizeType == YT.Button.Large || sizeType == YT.Button.Xlarge) {
            return 20;
        }
        return 0;
    }
}
