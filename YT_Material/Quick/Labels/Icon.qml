pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Basic as T

T.Control {
    id: root

    property bool active: root.iconSource != null && root.iconSource != undefined && root.iconSource != ""
    property int iconSize
    property color iconColor
    property variant iconSource
    property bool fontFill: false
    property int fontWght: 400
    property int fontGrade: 0
    property int fontOpticalSize: 24

    enabled: false
    implicitWidth: active ? iconSize + leftPadding + rightPadding : 0
    implicitHeight: active ? iconSize + topPadding + bottomPadding : 0
    contentItem: Loader {
        active: root.active
        sourceComponent: Number.isFinite(root.iconSource) ? comText : comImage
    }

    readonly property Component comText: Component {
        id: comText
        Text {
            text: String.fromCharCode(root.iconSource)
            font.family: "Material Symbols Rounded"
            font.pixelSize: root.iconSize
            color: root.iconColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            renderType: Text.NativeRendering // 关键：切换为原生渲染
            font.variableAxes: {
                "FILL": root.fontFill,
                "wght": root.fontWght,
                "opsz": root.fontOpticalSize,
                "GRAD": root.fontGrade
            }
        }
    }

    readonly property Component comImage: Component {
        id: comImage
        Image {
            width: root.iconSize
            height: root.iconSize
            source: root.iconSource
            // sourceSize: Qt.size(width, height)
            fillMode: Image.PreserveAspectFit
        }
    }
}
