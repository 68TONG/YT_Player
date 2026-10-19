pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YT_Material as YT_M

Item {
    id: root

    property real damping: 0.9
    property real epsilon: 0.01
    property real mass: 1.0
    property real modulus: 0
    property real spring: 20
    property real velocity: 0

    property real activeDamping: damping
    property real activeEpsilon: epsilon
    property real activeMass: mass
    property real activeModulus: modulus
    property real activeSpring: spring
    property real activeVelocity: velocity
    property real activeTargetX: targetX
    property real runStartedAt: 0
    property real runDuration: 0
    property bool collectingSamples: false
    property var samples: []

    readonly property real availableTravel: Math.max(0, demoTrack.width - demoMarker.width)
    readonly property real startX: availableTravel * 0.15
    readonly property real targetX: availableTravel * 0.75
    readonly property color rangeColor: YT_M.Theme.PrimaryContainer
    readonly property color inRangeColor: YT_M.Theme.Primary
    readonly property color outOfRangeColor: YT_M.Theme.Tertiary

    function outputSampleX(value: real): real {
        const minimum = Math.min(startX, targetX);
        const maximum = Math.max(startX, targetX);
        const range = Math.max(1, maximum - minimum);
        const rangeStart = outputRange.x;
        const rangeWidth = outputRange.width;
        const position = rangeStart + (value - minimum) / range * rangeWidth;
        return Math.max(0, Math.min(sampleTrack.width, position));
    }

    function captureSample() {
        const elapsed = Math.max(0, Date.now() - runStartedAt);
        const nextSamples = samples.slice();
        nextSamples.push({
            time: elapsed,
            value: demoMarker.x
        });
        samples = nextSamples;
        runDuration = elapsed;
    }

    function startAnimation() {
        collectingSamples = false;
        springAnimation.stop();
        activeDamping = damping;
        activeEpsilon = epsilon;
        activeMass = mass;
        activeModulus = modulus;
        activeSpring = spring;
        activeVelocity = velocity;
        activeTargetX = targetX;
        demoMarker.x = startX;
        samples = [];
        runDuration = 0;
        runStartedAt = Date.now();
        collectingSamples = true;
        captureSample();
        springAnimation.start();
    }

    function generateMd3SpringValue() {
        const springValue = YT_M.AnimationTemplate.getSpringValue(Number(md3DampingInput.text),
                                                                  Number(md3StiffnessInput.text));
        damping = springValue.damping;
        spring = springValue.spring;
        mass = springValue.mass;
    }

    component ParameterSlider: RowLayout {
        id: parameterSlider

        required property string label
        required property real value
        required property real fromValue
        required property real toValue
        required property real stepValue
        signal valueEdited(real editedValue)

        spacing: YT_M.Theme.Space100

        Label {
            text: parameterSlider.label
            color: YT_M.Theme.OnSurface
            Layout.preferredWidth: 82
        }

        Slider {
            Layout.fillWidth: true
            from: parameterSlider.fromValue
            to: parameterSlider.toValue
            stepSize: parameterSlider.stepValue
            snapMode: Slider.SnapAlways
            value: parameterSlider.value
            onMoved: parameterSlider.valueEdited(value)
        }

        Label {
            text: Number(parameterSlider.value).toFixed(2)
            color: YT_M.Theme.OnSurfaceVariant
            horizontalAlignment: Text.AlignRight
            Layout.preferredWidth: 58
        }
    }

    SpringAnimation {
        id: springAnimation

        target: demoMarker
        property: "x"
        to: root.activeTargetX
        spring: root.activeSpring
        damping: root.activeDamping
        epsilon: root.activeEpsilon
        mass: root.activeMass
        modulus: root.activeModulus
        velocity: root.activeVelocity

        onRunningChanged: {
            if (running == false && root.collectingSamples) {
                root.captureSample();
                root.collectingSamples = false;
            }
        }
    }

    Timer {
        interval: 16
        repeat: true
        running: springAnimation.running
        onTriggered: root.captureSample()
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        anchors.margins: YT_M.Theme.Space200
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: scrollView.availableWidth
            spacing: YT_M.Theme.Space200

            Text {
                text: "SpringAnimation演示"
                color: YT_M.Theme.OnSurface
                font.pixelSize: 24
                font.weight: Font.Medium
            }

            GridLayout {
                Layout.fillWidth: true
                columns: width >= 720 ? 3 : 1
                columnSpacing: YT_M.Theme.Space300
                rowSpacing: YT_M.Theme.Space100

                ColumnLayout {
                    Layout.fillWidth: true

                    Label {
                        text: "MD3 damping"
                        color: YT_M.Theme.OnSurface
                    }

                    TextField {
                        id: md3DampingInput
                        Layout.fillWidth: true
                        placeholderText: "输入 damping"
                        validator: DoubleValidator {
                            bottom: 0
                            notation: DoubleValidator.StandardNotation
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true

                    Label {
                        text: "MD3 stiffness"
                        color: YT_M.Theme.OnSurface
                    }

                    TextField {
                        id: md3StiffnessInput
                        Layout.fillWidth: true
                        placeholderText: "输入 stiffness"
                        validator: IntValidator {
                            bottom: 1
                        }
                    }
                }

                Button {
                    text: "生成动画属性"
                    enabled: md3DampingInput.text.length > 0
                             && md3DampingInput.acceptableInput
                             && md3StiffnessInput.text.length > 0
                             && md3StiffnessInput.acceptableInput
                    onClicked: root.generateMd3SpringValue()
                }
            }

            GridLayout {
                id: parameterGrid
                Layout.fillWidth: true
                columns: width >= 720 ? 2 : 1
                columnSpacing: YT_M.Theme.Space300
                rowSpacing: YT_M.Theme.Space100

                ParameterSlider {
                    Layout.fillWidth: true
                    label: "damping"
                    value: root.damping
                    fromValue: 0
                    toValue: 1
                    stepValue: 0.01
                    onValueEdited: editedValue => root.damping = editedValue
                }

                ParameterSlider {
                    Layout.fillWidth: true
                    label: "epsilon"
                    value: root.epsilon
                    fromValue: 0.01
                    toValue: 10
                    stepValue: 0.01
                    onValueEdited: editedValue => root.epsilon = editedValue
                }

                ParameterSlider {
                    Layout.fillWidth: true
                    label: "mass"
                    value: root.mass
                    fromValue: 0.1
                    toValue: 10
                    stepValue: 0.1
                    onValueEdited: editedValue => root.mass = editedValue
                }

                ParameterSlider {
                    Layout.fillWidth: true
                    label: "modulus"
                    value: root.modulus
                    fromValue: 0
                    toValue: 600
                    stepValue: 10
                    onValueEdited: editedValue => root.modulus = editedValue
                }

                ParameterSlider {
                    Layout.fillWidth: true
                    label: "spring"
                    value: root.spring
                    fromValue: 0
                    toValue: 100
                    stepValue: 1
                    onValueEdited: editedValue => root.spring = editedValue
                }

                ParameterSlider {
                    Layout.fillWidth: true
                    label: "velocity"
                    value: root.velocity
                    fromValue: -1500
                    toValue: 1500
                    stepValue: 10
                    onValueEdited: editedValue => root.velocity = editedValue
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Text {
                    Layout.fillWidth: true
                    text: root.collectingSamples ? "动画运行中" : "位置随时间变化"
                    color: YT_M.Theme.OnSurface
                    font.pixelSize: 18
                    font.weight: Font.Medium
                }

                Button {
                    text: root.collectingSamples ? "动画运行中…" : "启动动画"
                    enabled: root.collectingSamples == false
                    onClicked: root.startAnimation()
                }
            }

            Rectangle {
                id: demoSurface
                Layout.fillWidth: true
                Layout.preferredHeight: 148
                radius: YT_M.Theme.RadiusLarge
                color: YT_M.Theme.SurfaceContainerLow

                Item {
                    id: demoTrack
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: YT_M.Theme.Space300
                    anchors.rightMargin: YT_M.Theme.Space300
                    anchors.verticalCenter: parent.verticalCenter
                    height: 32

                    Rectangle {
                        anchors.fill: parent
                        anchors.verticalCenter: parent.verticalCenter
                        height: 6
                        radius: height / 2
                        color: YT_M.Theme.OutlineVariant
                    }

                    Rectangle {
                        x: root.startX + demoMarker.width / 2
                        width: Math.max(0, root.targetX - root.startX)
                        anchors.verticalCenter: parent.verticalCenter
                        height: 8
                        radius: height / 2
                        color: root.rangeColor
                    }

                    Rectangle {
                        id: demoMarker
                        x: root.startX
                        anchors.verticalCenter: parent.verticalCenter
                        width: 22
                        height: 22
                        radius: width / 2
                        color: root.inRangeColor
                        border.width: 3
                        border.color: YT_M.Theme.Surface
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.top: parent.bottom
                        anchors.topMargin: YT_M.Theme.Space100
                        text: "起点"
                        color: YT_M.Theme.OnSurfaceVariant
                        font.pixelSize: 12
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: root.targetX
                        anchors.top: parent.bottom
                        anchors.topMargin: YT_M.Theme.Space100
                        text: "目标"
                        color: YT_M.Theme.OnSurfaceVariant
                        font.pixelSize: 12
                    }
                }
            }

            Text {
                text: "动画输出数据"
                color: YT_M.Theme.OnSurface
                font.pixelSize: 18
                font.weight: Font.Medium
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: YT_M.Theme.Space200

                Rectangle {
                    Layout.preferredWidth: 12
                    Layout.preferredHeight: 12
                    radius: 3
                    color: root.inRangeColor
                }
                Text {
                    text: "范围内输出"
                    color: YT_M.Theme.OnSurfaceVariant
                    font.pixelSize: 12
                }

                Rectangle {
                    Layout.preferredWidth: 12
                    Layout.preferredHeight: 12
                    radius: 3
                    color: root.outOfRangeColor
                }
                Text {
                    text: "超出范围"
                    color: YT_M.Theme.OnSurfaceVariant
                    font.pixelSize: 12
                }

                Rectangle {
                    Layout.preferredWidth: 12
                    Layout.preferredHeight: 12
                    radius: 3
                    color: root.rangeColor
                }
                Text {
                    text: "数据范围"
                    color: YT_M.Theme.OnSurfaceVariant
                    font.pixelSize: 12
                }
            }

            Text {
                text: "采样值范围"
                color: YT_M.Theme.OnSurface
                font.pixelSize: 18
                font.weight: Font.Medium
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 124
                radius: YT_M.Theme.RadiusMedium
                color: YT_M.Theme.SurfaceContainerLow

                Item {
                    id: sampleTrack
                    anchors.fill: parent
                    anchors.leftMargin: YT_M.Theme.Space200
                    anchors.rightMargin: YT_M.Theme.Space200
                    anchors.topMargin: YT_M.Theme.Space150
                    anchors.bottomMargin: YT_M.Theme.Space300

                    Rectangle {
                        id: outputRange
                        x: parent.width * 0.06
                        width: parent.width * 0.82
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        radius: YT_M.Theme.RadiusSmall
                        color: root.rangeColor
                    }

                    Repeater {
                        model: root.samples.length

                        Rectangle {
                            required property int index

                            x: root.outputSampleX(root.samples[index].value) - width / 2
                            width: index === root.samples.length - 1 ? 6 : 2
                            anchors.top: sampleTrack.top
                            anchors.bottom: sampleTrack.bottom
                            color: {
                                const value = root.samples[index].value;
                                const minimum = Math.min(root.startX, root.targetX);
                                const maximum = Math.max(root.startX, root.targetX);
                                return value < minimum || value > maximum
                                    ? root.outOfRangeColor : root.inRangeColor;
                            }
                            opacity: index === root.samples.length - 1 ? 1 : 0.72
                            z: index === root.samples.length - 1 ? 1 : 0
                        }
                    }

                    Text {
                        anchors.left: outputRange.left
                        anchors.top: outputRange.bottom
                        anchors.topMargin: YT_M.Theme.Space050
                        text: "from " + Math.min(root.startX, root.targetX).toFixed(0)
                        color: YT_M.Theme.OnSurfaceVariant
                        font.pixelSize: 12
                    }

                    Text {
                        anchors.right: outputRange.right
                        anchors.top: outputRange.bottom
                        anchors.topMargin: YT_M.Theme.Space050
                        text: "to " + Math.max(root.startX, root.targetX).toFixed(0)
                        color: YT_M.Theme.OnSurfaceVariant
                        font.pixelSize: 12
                    }
                }

                Text {
                    anchors.centerIn: parent
                    visible: root.samples.length === 0
                    text: "点击“启动动画”开始采集"
                    color: YT_M.Theme.OnSurfaceVariant
                    font.pixelSize: 14
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 250
                radius: YT_M.Theme.RadiusMedium
                color: YT_M.Theme.SurfaceContainerLow

                Canvas {
                    id: chartCanvas
                    anchors.fill: parent
                    anchors.margins: YT_M.Theme.Space150

                    onPaint: {
                        const context = getContext("2d");
                        const left = 42;
                        const right = width - 12;
                        const top = 12;
                        const bottom = height - 24;
                        const plotWidth = right - left;
                        const plotHeight = bottom - top;
                        const values = root.samples;
                        const minimumRange = Math.min(root.startX, root.targetX);
                        const maximumRange = Math.max(root.startX, root.targetX);
                        let minimumValue = minimumRange;
                        let maximumValue = maximumRange;
                        let maximumTime = Math.max(1, root.runDuration);

                        for (let i = 0; i < values.length; ++i) {
                            minimumValue = Math.min(minimumValue, values[i].value);
                            maximumValue = Math.max(maximumValue, values[i].value);
                            maximumTime = Math.max(maximumTime, values[i].time);
                        }

                        const valuePadding = Math.max(8, (maximumValue - minimumValue) * 0.08);
                        minimumValue -= valuePadding;
                        maximumValue += valuePadding;
                        const valueToY = function(value) {
                            return top + (maximumValue - value) / (maximumValue - minimumValue) * plotHeight;
                        };
                        const timeToX = function(time) {
                            return left + time / maximumTime * plotWidth;
                        };

                        context.clearRect(0, 0, width, height);
                        context.fillStyle = root.rangeColor.toString();
                        context.fillRect(left, valueToY(maximumRange), plotWidth,
                                         valueToY(minimumRange) - valueToY(maximumRange));

                        context.strokeStyle = YT_M.Theme.OutlineVariant.toString();
                        context.lineWidth = 1;
                        context.beginPath();
                        for (let i = 0; i <= 4; ++i) {
                            const y = top + plotHeight * i / 4;
                            context.moveTo(left, y);
                            context.lineTo(right, y);
                        }
                        context.stroke();

                        context.fillStyle = YT_M.Theme.OnSurfaceVariant.toString();
                        context.font = "11px sans-serif";
                        context.textAlign = "right";
                        context.fillText(maximumValue.toFixed(0), left - 6, top + 4);
                        context.fillText(minimumValue.toFixed(0), left - 6, bottom);
                        context.textAlign = "left";
                        context.fillText("0 s", left, height - 4);
                        context.textAlign = "right";
                        context.fillText((maximumTime / 1000).toFixed(2) + " s", right, height - 4);

                        for (let i = 1; i < values.length; ++i) {
                            const previous = values[i - 1];
                            const current = values[i];
                            const middleValue = (previous.value + current.value) / 2;
                            const inRange = middleValue >= minimumRange && middleValue <= maximumRange;
                            context.beginPath();
                            context.moveTo(timeToX(previous.time), valueToY(previous.value));
                            context.lineTo(timeToX(current.time), valueToY(current.value));
                            context.strokeStyle = (inRange ? root.inRangeColor : root.outOfRangeColor).toString();
                            context.lineWidth = 2.5;
                            context.stroke();
                        }
                    }

                    onWidthChanged: requestPaint()
                    onHeightChanged: requestPaint()
                    Connections {
                        target: root
                        function onSamplesChanged() {
                            chartCanvas.requestPaint();
                        }
                        function onRunDurationChanged() {
                            chartCanvas.requestPaint();
                        }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    visible: root.samples.length === 0
                    text: "点击“启动动画”开始采集"
                    color: YT_M.Theme.OnSurfaceVariant
                    font.pixelSize: 14
                }
            }
        }
    }
}
