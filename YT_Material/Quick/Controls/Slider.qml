pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic as T
import YT_Material as YT

T.Slider {
    id: root

    handle: null
    // handle: Rectangle {
    //     x: root.horizontal ? root.lPosition + root.handlePadding : 0
    //     y: root.vertical ? root.lPosition + root.handlePadding : 0
    //     width: root.horizontal ? root.handleSize : parent.width
    //     height: root.vertical ? root.handleSize : parent.height
    //     radius: YT.Theme.RadiusFull
    //     color: root.handleColor
    // }

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

        // Repeater {
        //     id: repeater
        //     model: (root.to - root.from) / root.stepSize + 1
        //     delegate: Rectangle {
        //         required property int index
        //         property real position: index / (repeater.model - 1)
        //         property real padding: (root.containerFullRadius + root.positionPadding)
        //         z: +true
        //         x: root.horizontal ? position * (parent.width - padding * 2) + padding : (parent.width - width) / 2
        //         y: root.vertical ? position * (parent.height - padding * 2) + padding : (parent.height - height) / 2
        //         width: root.indicatorSize
        //         height: root.indicatorSize
        //         radius: YT.Theme.RadiusFull
        //         visible: (root.lPosition - root.indicatorSize < x && root.rPosition > x) == false
        //         color: root.lPosition > x ? root.activeIndicatorColor : root.inactiveIndicatorColor
        //         Component.onCompleted: console.log(index, parent, position, repeater.model)
        //     }
        // }
    }

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
    property real mPosition: position * (horizontal ? width : height)
    property real lPosition: mPosition - positionPadding
    property real rPosition: mPosition + positionPadding
    Behavior on mPosition {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: 150
        }
    }

    property real handleSize: 5
    property real handlePadding: 0
    // property real handleSize: pressed ? 2 : 4
    // property real handlePadding: YT.Theme.Space75
    property color handleColor: {
        if (enabled == false) {
            return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.38);
        }
        return YT.Theme.Primary;
    }

    property real containerFullRadius: (vertical ? width : height) / 2
    property real containerInnerRadius: {
        if (position == 0.0 || position == 1.0) {
            return containerFullRadius;
        } else if (pressed) {
            return 2;
        }
        return YT.Theme.RadiusExtraSmall;
    }
    property real containerOuterRadius: {
        return containerFullRadius;
    }
    Behavior on containerInnerRadius {
        NumberAnimation {
            easing.type: Easing.OutQuad
            duration: 150
        }
    }
    property color activeContainerColor: {
        if (enabled == false) {
            return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.38);
        }
        return YT.Theme.Primary;
    }
    property color inactiveContainerColor: {
        if (enabled == false) {
            return YT.Theme.setAlphaF(YT.Theme.OnSurface, 0.12);
        }
        return YT.Theme.SecondaryContainer;
    }

    property real indicatorSize: 4
    property color activeIndicatorColor: {
        if (enabled == false) {
            return YT.Theme.InverseOnSurface;
        }
        return YT.Theme.OnPrimary;
    }
    property color inactiveIndicatorColor: {
        if (enabled == false) {
            return YT.Theme.OnSurface;
        }
        return YT.Theme.OnSecondaryContainer;
    }
}
