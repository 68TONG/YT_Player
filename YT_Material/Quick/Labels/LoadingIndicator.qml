import QtQuick
import YT_Material as YT_M

Item {
    id: root

    property real padding: 5
    property real implicitSize: 48
    implicitWidth: Math.max(0, implicitSize)
    implicitHeight: Math.max(0, implicitSize)

    property color color: containerVisible ? YT_M.Theme.OnPrimaryContainer : YT_M.Theme.Primary
    property color containerColor: containerVisible ? YT_M.Theme.PrimaryContainer : YT_M.Theme.Transparent
    property bool containerVisible: false

    property bool running: true
    property real progress: 0.0
    onRunningChanged: {
        shapeIndex = 0
        globalRotation = 0
        rotationTargetAngle = 90
    }
    
    property int fromShape: YT_M.MaterialShape.Circle
    property int toShape: YT_M.MaterialShape.SoftBurst
    property int shapeIndex: 0
    property variant shapeList: [YT_M.MaterialShape.SoftBurst, YT_M.MaterialShape.Cookie9Sided, YT_M.MaterialShape.Pentagon, YT_M.MaterialShape.Pill, YT_M.MaterialShape.Sunny, YT_M.MaterialShape.Cookie4Sided, YT_M.MaterialShape.Oval]
    property real globalRotation: 0.0
    property real rotationTargetAngle: 90.0
    readonly property real rotationAngle: progress * (running ? 90.0 : 180.0) + rotationTargetAngle + globalRotation

    readonly property real finalScale: 0.78 * elasticScale
    readonly property real elasticScale: {
        let coercedProgress = Math.max(0, Math.min(1, progress));
        const overshoot = Math.abs(progress - coercedProgress);
        return 1.0 - 0.2 * overshoot;
    }

    SequentialAnimation {
        running: root.running
        loops: Animation.Infinite
        ParallelAnimation {
            PauseAnimation {
                duration: 650
            }
            NumberAnimation {
                target: root
                property: "progress"
                from: 0.0
                to: 1.0
                duration: YT_M.Easing.Long3
                easing: YT_M.Easing.SpringSlowEx
            }
        }
        ScriptAction {
            script: {
                root.shapeIndex = (root.shapeIndex + 1) % root.shapeList.length;
                root.rotationTargetAngle = (root.rotationTargetAngle + 90.0) % 360.0;
            }
        }
    }

    NumberAnimation {
        running: root.running
        loops: Animation.Infinite

        target: root
        property: "globalRotation"
        from: 0.0
        to: 360.0
        duration: 4666
        easing.type: Easing.Linear
    }

    YT_M.MaterialShape {
        id: shapeItem
        anchors.fill: root
        anchors.margins: root.padding
        color: root.color
        scale: root.finalScale
        rotation: root.rotationAngle
        fromShape: root.running ? root.shapeList[root.shapeIndex] : root.fromShape
        toShape: root.running ? root.shapeList[(root.shapeIndex + 1) % root.shapeList.length] : root.toShape
        morphProgress: root.progress
    }

    Rectangle {
        z: -1
        width: root.width
        height: root.height
        color: root.containerColor
        radius: YT_M.Theme.RadiusFull
    }
}
