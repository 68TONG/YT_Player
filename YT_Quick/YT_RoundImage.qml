import QtQuick

Item {
    id: root
    property int padding: 0
    property alias radius: shaderEffect.radius

    property alias imageSource: imageSource
    property alias source: imageSource.source
    property alias asynchronous: imageSource.asynchronous
    Image {
        id: imageSource
        anchors.fill: root
        anchors.margins: root.padding
        fillMode: Image.PreserveAspectFit
    }

    ShaderEffect {
        id: shaderEffect
        anchors.fill: imageSource

        property real radius: 0
        property variant source: ShaderEffectSource {
            hideSource: true
            sourceItem: imageSource
        }

        fragmentShader: "qrc:/Shaders/YT_RoundImage.frag.qsb"
    }
}
