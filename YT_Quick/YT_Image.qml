import QtQuick
import QtQuick.Effects
import YT_Player

Item {
    id: root
    property alias radius: imageMask.radius
    property alias source: imageSource.source
    property alias asynchronous: imageSource.asynchronous
    property int padding: 0
    Image {
        id: imageSource
        anchors.fill: root
        anchors.margins: root.padding
        visible: false
        layer.enabled: true

        fillMode: Image.PreserveAspectFit
    }
    Rectangle {
        id: imageMask
        anchors.fill: imageSource
        radius: YT_Info.Radius
        visible: false
        layer.enabled: true
    }
    MultiEffect {
        anchors.fill: imageSource
        source: imageSource
        maskEnabled: true
        maskSource: imageMask
    }
    // Rectangle {
    //     anchors.fill: imageSource
    //     anchors.margins: -border.width
    //     radius: imageMask.radius
    //     color: Qt.rgba(0, 0, 0, 0)
    //     border.width: 1
    //     border.color: YT_Info.FontFocusColor
    //     z: +true
    // }
}
