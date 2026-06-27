import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls
import YT_Player
import HuskarUI.Basic

YT_Popup {
    id: root

    contentItem: GridLayout {
        id: rootContent
        rows: 3
        rowSpacing: 0
        columns: 1
        columnSpacing: 0
    }

    readonly property YT_TextInput searchItem: YT_TextInput {
        id: searchItem
        parent: rootContent
        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true
        Layout.preferredHeight: YT_Info.ItemSize.height

        leftPadding: indicatorLogo.width
        rightPadding: indicatorClear.width + indicatorClear.anchors.margins * 2
        placeholderText: "SearchWidget"

        background: YT_Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: +true
            color: parent.hovered || parent.cursorVisible ? YT_Info.ItemFocusColor : YT_Info.ItemColor
        }
        readonly property YT_Indicator indicatorLogo: YT_Indicator {
            enabled: false
            parent: searchItem
            edgeType: Qt.LeftEdge
            modelData: "qrc:/Ui_Image/search_logo.png"
            background: null
        }
        readonly property YT_Indicator indicatorClear: YT_Indicator {
            parent: searchItem
            visible: parent.text
            anchors.margins: YT_Info.Margin

            edgeType: Qt.RightEdge
            modelData: "qrc:/Ui_Image/close.png"
            Component.onCompleted: {
                background.radius = Qt.bind(function () {
                    return height / 2;
                });
            }
            onClicked: searchItem.clear()
        }
    }

    readonly property YT_SearchWidget_FoldableView pathItem: YT_SearchWidget_FoldableView {
        id: pathItem
        parent: rootContent
        Layout.row: 1
        Layout.column: 0
        Layout.fillWidth: true
        Layout.preferredHeight: implicitHeight

        modelData: "路径选项"
        model: ["默认列表", "粤语", "日语", "东方Project"]
        delegate: YT_SearchWidget_SelectItem {}
    }

    readonly property YT_SearchWidget_FoldableView filterItem: YT_SearchWidget_FoldableView {
        id: filterItem
        parent: rootContent
        Layout.row: 2
        Layout.column: 0
        Layout.fillWidth: true
        Layout.preferredHeight: implicitHeight

        modelData: "过滤选项"
        model: ["Title", "Album", "Artist"]
        delegate: YT_SearchWidget_SelectItem {}
    }

    readonly property YT_SearchWidget_FoldableView historyItem: YT_SearchWidget_FoldableView {
        id: historyItem
        parent: rootContent
        Layout.row: 3
        Layout.column: 0
        Layout.fillWidth: true
        Layout.preferredHeight: implicitHeight

        modelData: "历史选项"
        model: ["花塔", "晓R", "森罗万象"]
        delegate: YT_Button {
            id: root_SelectItem
            required property string modelData
            highlighted: hovered || down

            leftPadding: YT_Info.Margin
            rightPadding: YT_Info.Margin
            indicator: null
            background: YT_Rectangle {
                color: YT_Info.BackgroundColor
                border.color: root_SelectItem.highlighted ? YT_Info.FontColor : YT_Info.ItemFocusColor
                // border.width: +true * 2
            }
            contentItem: YT_Button.LoadText {}
            onClicked: searchItem.text = modelData
        }
    }

    component YT_SearchWidget_FoldableView: Control {
        id: root_FoldableView
        property var model: null
        property Component delegate: null
        property bool expanded: false
        property string modelData: ""

        clip: true
        spacing: YT_Info.MarginSmall
        implicitWidth: 0
        implicitHeight: expanded ? root_FoldableView_OpenItem.height + root_FoldableView_OpenView.height + spacing : root_FoldableView_OpenItem.height
        YT_Button {
            id: root_FoldableView_OpenItem
            width: parent.width
            height: implicitHeight

            padding: 0
            background: null
            contentItem: RowLayout {
                spacing: YT_Info.Spacing
                YT_Rectangle {
                    Layout.preferredWidth: 15
                    Layout.preferredHeight: +true
                    color: (root_FoldableView_OpenItem.hovered || root_FoldableView_OpenItem.down) ? YT_Info.ItemFocusColor : YT_Info.ItemColor
                }
                YT_Text {
                    text: root_FoldableView.modelData
                }
                YT_Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: +true
                    color: (root_FoldableView_OpenItem.hovered || root_FoldableView_OpenItem.down) ? YT_Info.ItemFocusColor : YT_Info.ItemColor
                }
            }
            onClicked: root_FoldableView.expanded = !root_FoldableView.expanded
        }
        Flow {
            id: root_FoldableView_OpenView
            width: parent.width
            height: implicitHeight
            anchors.top: root_FoldableView_OpenItem.bottom
            anchors.topMargin: parent.spacing

            spacing: YT_Info.Spacing
            // move: Transition {
            //     NumberAnimation {
            //         properties: "x,y"
            //         duration: 300
            //         easing.type: Easing.OutCubic
            //     }
            // }
            Repeater {
                model: root_FoldableView.model
                delegate: root_FoldableView.delegate
            }
        }
    }

    component YT_SearchWidget_SelectItem: YT_Button {
        id: root_SelectItem
        required property string modelData
        property bool selected: false
        highlighted: selected || hovered || down

        leftPadding: YT_Info.Margin
        rightPadding: indicator.width
        indicator: YT_Indicator {
            enabled: false
            edgeType: Qt.RightEdge
            rightPadding: YT_Info.Margin
            modelData: "qrc:/Ui_Image/ok.png"
            background: null
            contentItem.visible: false
            MultiEffect {
                anchors.fill: source
                source: root_SelectItem.indicator.contentItem
                brightness: 1.0
                colorization: 1.0
                colorizationColor: root_SelectItem.selected ? YT_Info.FontColor : YT_Info.ItemFocusColor
                Behavior on colorizationColor {
                    ColorAnimation {
                        duration: 300
                        easing.type: Easing.OutInQuad
                    }
                }
            }
        }
        background: YT_Rectangle {
            color: YT_Info.BackgroundColor
            border.color: root_SelectItem.highlighted ? YT_Info.FontColor : YT_Info.ItemFocusColor
            // border.width: +true * 2
        }
        contentItem: YT_Button.LoadText {}
        onClicked: selected = !selected
    }
}
