pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT_M

T.Slider {
    id: root

    handle: Rectangle {
        x: root.horizontal ? root.lPosition + root.handlePadding : (parent.width - width) / 2
        y: root.vertical ? root.lPosition + root.handlePadding : (parent.height - height) / 2
        width: root.horizontal ? root.handleSize : 44
        height: root.vertical ? root.handleSize : 44
        radius: YT_M.Theme.RadiusFull
        color: root.handleColor
    }

    background: Item {
        Rectangle {
            x: 0
            y: 0
            width: root.horizontal ? root.lPosition : parent.width
            height: root.vertical ? root.lPosition : parent.height

            topLeftRadius: root.horizontal ? root.containerOuterRadius : root.containerInnerRadius
            topRightRadius: root.horizontal ? root.containerInnerRadius : root.containerInnerRadius
            bottomLeftRadius: root.horizontal ? root.containerOuterRadius : root.containerOuterRadius
            bottomRightRadius: root.horizontal ? root.containerInnerRadius : root.containerOuterRadius
            color: root.activeContainerColor
        }

        Rectangle {
            x: root.horizontal ? root.rPosition : 0
            y: root.vertical ? root.rPosition : 0
            width: root.horizontal ? parent.width - root.rPosition : parent.width
            height: root.vertical ? parent.height - root.rPosition : parent.height

            topLeftRadius: root.horizontal ? root.containerInnerRadius : root.containerOuterRadius
            topRightRadius: root.horizontal ? root.containerOuterRadius : root.containerInnerRadius
            bottomLeftRadius: root.horizontal ? root.containerInnerRadius : root.containerInnerRadius
            bottomRightRadius: root.horizontal ? root.containerOuterRadius : root.containerOuterRadius
            color: root.inactiveContainerColor
        }

        Item {
            anchors.fill: parent
            anchors.topMargin: root.vertical ? root.topPadding : 0
            anchors.leftMargin: root.horizontal ? root.leftPadding : 0
            anchors.rightMargin: root.horizontal ? root.rightPadding : 0
            anchors.bottomMargin: root.vertical ? root.bottomPadding : 0

            M_Indicator {
                position: 1.0
                visible: (root.snapMode == Slider.NoSnap) && position > root.position + 0.03
            }
            Repeater {
                id: indicatorRepeater
                readonly property real padding: root.indicatorSize / 2
                readonly property real lrRange: 0.5 / model
                enabled: root.snapMode != Slider.NoSnap
                model: (root.to - root.from) / root.stepSize + 1
                delegate: M_Indicator {
                    required property int index
                    position: (index) / (indicatorRepeater.model - 1)
                    visible: position < root.position - indicatorRepeater.lrRange || position > root.position + indicatorRepeater.lrRange
                }
            }
        }
    }

    padding: handlePadding
    implicitWidth: 240
    implicitHeight: 16

    property int sizeType: Slider.Xsmall
    enum SizeT {
        Xsmall,
        Small,
        Medium,
        Large,
        Xlarge
    }

    property real iconSize: 24
    property color iconColor
    property variant iconSource

    property real positionPadding: (handleSize / 2) + (handlePadding)
    property real mPosition: position * (horizontal ? availableWidth : availableHeight) + padding
    property real lPosition: mPosition - positionPadding
    property real rPosition: mPosition + positionPadding
    Behavior on mPosition {
        NumberAnimation {
            easing: YT_M.Easing.SpringFast
            duration: YT_M.Easing.Medium3
        }
    }

    property real handleSize: pressed ? 2 : 4
    property real handlePadding: YT_M.Theme.Space75
    property color handleColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
        }
        return YT_M.Theme.Primary;
    }

    property real containerFullRadius: (vertical ? width : height) / 2
    property real containerInnerRadius: {
        if (position == 0.0 || position == 1.0) {
            return containerFullRadius;
        } else if (pressed) {
            return 2;
        }
        return YT_M.Theme.RadiusExtraSmall;
    }
    property real containerOuterRadius: {
        return containerFullRadius;
    }
    Behavior on containerInnerRadius {
        NumberAnimation {
            easing: YT_M.Easing.Standard
            duration: YT_M.Easing.Medium3
        }
    }

    property color activeContainerColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.38);
        }
        return YT_M.Theme.Primary;
    }
    property color inactiveContainerColor: {
        if (enabled == false) {
            return YT_M.Theme.setAlphaF(YT_M.Theme.OnSurface, 0.12);
        }
        return YT_M.Theme.SecondaryContainer;
    }

    property real indicatorSize: 4
    property color activeIndicatorColor: {
        if (enabled == false) {
            return YT_M.Theme.InverseOnSurface;
        }
        return YT_M.Theme.OnPrimary;
    }
    property color inactiveIndicatorColor: {
        if (enabled == false) {
            return YT_M.Theme.OnSurface;
        }
        return YT_M.Theme.OnSecondaryContainer;
    }

    component M_Indicator: Rectangle {
        required property real position
        z: +true
        x: root.horizontal ? position * (parent.width) - indicatorRepeater.padding : (parent.width - width) / 2
        y: root.vertical ? position * (parent.height) - indicatorRepeater.padding : (parent.height - height) / 2
        width: root.indicatorSize
        height: root.indicatorSize
        radius: YT_M.Theme.RadiusFull
        color: position < root.position ? root.activeIndicatorColor : root.inactiveIndicatorColor
    }
}
