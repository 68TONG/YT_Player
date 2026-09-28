import QtQuick

Item {
    id: root
    property int padding: 0
    property alias radius: shaderEffect.radius

    property alias source: imageSource.source
    property alias imageSource: imageSource
    property alias fillMode: imageSource.fillMode
    property alias asynchronous: imageSource.asynchronous

    implicitWidth: imageSource.implicitWidth
    implicitHeight: imageSource.implicitHeight
    Image {
        id: imageSource
        anchors.fill: root
        anchors.margins: root.padding
        fillMode: Image.PreserveAspectFit
        onStatusChanged: {
            if (status === Image.Error) {
                visible = false;
            }
        }
    }

    ShaderEffect {
        id: shaderEffect
        anchors.fill: imageSource

        property real radius: 0
        property variant source: ShaderEffectSource {
            hideSource: true
            sourceItem: imageSource
            mipmap: imageSource.mipmap
        }

        fragmentShader: "qrc:/Shaders/YT_RoundImage.frag.qsb"
    }
}
