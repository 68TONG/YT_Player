import QtQuick
import QtQuick.Controls
import YT_Player

Button {
    id: root
    property int edgeType: 0
    property string modelData: ""

    padding: YT_Info.MarginSmall
    verticalPadding: padding
    horizontalPadding: padding

    implicitWidth: (edgeType === Qt.LeftEdge || edgeType === Qt.RightEdge) ? height : implicitWidth
    implicitHeight: (edgeType === Qt.TopEdge || edgeType === Qt.BottomEdge) ? width : implicitHeight

    contentItem: Image {
        smooth: true
        source: root.modelData
        fillMode: Image.PreserveAspectFit
    }

    background: YT_Rectangle {
        color: YT_Info.ItemFocusColor
        opacity: (parent.hovered || parent.down)
    }

    states: [
        State {
            name: "top"
            when: root.edgeType === Qt.TopEdge
            PropertyChanges {
                target: root
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: undefined
            }
        },
        State {
            name: "bottom"
            when: root.edgeType === Qt.BottomEdge
            PropertyChanges {
                target: root
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: undefined
            }
        },
        State {
            name: "left"
            when: root.edgeType === Qt.LeftEdge
            PropertyChanges {
                target: root
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: undefined
            }
        },
        State {
            name: "right"
            when: root.edgeType === Qt.RightEdge
            PropertyChanges {
                target: root
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: undefined
            }
        }
    ]
}
