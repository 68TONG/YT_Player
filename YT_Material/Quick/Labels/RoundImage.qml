import QtQuick

Item {
    id: root
    property alias radius: shaderEffect.radius
    property alias padding: shaderEffect.anchors.margins
    property rect imageClipRect: Qt.rect(0, 0, 0, 0)

    property alias image: image
    property alias cache: image.cache
    property alias source: image.source
    property alias fillMode: image.fillMode
    property alias asynchronous: image.asynchronous

    implicitWidth: image.implicitWidth
    implicitHeight: image.implicitHeight
    Image {
        id: image
        width: fillMode != Image.Pad ? root.width : implicitWidth
        height: fillMode != Image.Pad ? root.height : implicitHeight
        fillMode: Image.PreserveAspectFit
        onStatusChanged: {
            if (status === Image.Error) {
                visible = false;
            }
        }
    }

    ShaderEffect {
        id: shaderEffect
        anchors.fill: root
        anchors.margins: root.padding

        property real radius: 0
        property variant source: ShaderEffectSource {
            hideSource: true
            sourceItem: image
            sourceRect: root.imageClipRect
        }

        fragmentShader: "qrc:/Resources/Shaders/RoundImage.frag.qsb"
    }
}
