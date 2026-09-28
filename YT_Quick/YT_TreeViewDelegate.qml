import QtQuick
import QtQuick.Controls
import YT_Player

AbstractButton {
    id: root
    implicitWidth: ListView.view.width

    leftPadding: indicatorPadding + indicator.width + spacing
    rightPadding: YT_Info.MarginSmall
    verticalPadding: YT_Info.Margin
    readonly property int indicatorPadding: depth * indent + YT_Info.MarginSmall
    property int indent: indicator.width

    required property int index
    required property int depth
    required property bool expanded
    required property bool hasChildren

    indicator: YT_Indicator {
        edgeType: Qt.LeftEdge
        modelData: "qrc:/Ui_Image/next_page.png"
        background: null

        anchors.margins: YT_Info.Margin
        anchors.leftMargin: root.indicatorPadding

        rotation: root.expanded ? 90 : 0
        Behavior on rotation {
            NumberAnimation {
                duration: 180
                easing.type: Easing.OutInQuad
            }
        }
    }
    background: null
    contentItem: YT_Text {
        text: root.text
        elide: Text.ElideRight
    }

    Repeater {
        model: root.depth
        delegate: Rectangle {
            required property int index
            x: YT_Info.MarginSmall + root.indicator.width / 2 + index * root.indent
            y: 0
            z: -true
            width: +true
            height: parent.height
            color: Qt.lighter(YT_Info.ItemFocusColor, 1.4)
        }
    }
}
