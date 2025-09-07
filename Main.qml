import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

import YT_Player

YT_Window {
    id: mainWindow
    width: 640
    height: 480
    visible: true

    YT_TransformWindow {}
    Rectangle {
        id: mainBackground
        anchors.fill: parent
        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.BackgroundColor)
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
    }

    GridLayout {
        id: mainWindow_Layout
        anchors.fill: parent
        anchors.margins: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

        rows: 2
        rowSpacing: anchors.margins
        columns: 3
        columnSpacing: anchors.margins
    }

    Rectangle {
        id: mainTool
        parent: mainWindow_Layout

        color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemColor)
        radius: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true
        Layout.preferredHeight: 30

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // YT_TextField {
            //     Layout.fillWidth: true
            //     Layout.fillHeight: true

            //     font.pixelSize: 12
            //     placeholderText: "搜索框"
            //     background.anchors.margins: 3
            // }
            YT_MainPathBar {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Repeater {
                model: ListModel {
                    ListElement {
                        path: "qrc:/Resource_UI/full_screen.png"
                        operation: function() {
                            console.log("Function 1 called")
                        }
                    }
                    ListElement {
                        path: "qrc:/Resource_UI/minimize.png"
                        operation: function() {
                            console.log("Function 2 called")
                        }
                    }
                    ListElement {
                        path: "qrc:/Resource_UI/close.png"
                        operation: function() {
                            Qt.quit()
                        }
                    }
                }

                YT_Button {
                    Layout.preferredWidth: parent.height
                    Layout.preferredHeight: parent.height

                    background.anchors.margins: 4
                    padding: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

                    modelData: model.path
                    contentItem: YT_Button.LoadImage {}

                    onClicked: model.operation()
                }
            }
        }
    }

    ListView {
        id: mainLogo
        parent: mainWindow_Layout

        interactive: false
        currentIndex: YT.widgetIndex
        onCurrentIndexChanged: YT.widgetIndex = currentIndex

        spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)
        topMargin: spacing

        Layout.row: 0
        Layout.rowSpan: 2
        Layout.column: 0
        Layout.preferredWidth: 50
        Layout.fillHeight: true

        model: YT.widgetModel.count
        delegate: YT_Button {
            required property int index
            readonly property QtObject model: YT.widgetModel.get(index)
            readonly property string logo_path: model.logo_path

            property int implicitSize: parent.width - padding * 2
            padding: ListView.view.spacing
            implicitWidth: implicitSize
            implicitHeight: implicitSize
            anchors.horizontalCenter: parent.horizontalCenter

            modelData: logo_path
            contentItem: YT_Button.LoadImage {}
            background: null

            onClicked: YT.setWidgetIndex(index)
        }

        highlight: Rectangle {
            anchors.left: ListView.view.currentItem.left
            anchors.right: ListView.view.currentItem.right

            color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemFocusColor)
            radius: ListView.view.spacing
        }

        Rectangle {
            z: -true
            anchors.fill: parent
            color: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemColor)
            radius: parent.spacing
        }
    }

    YT_ScrollBarList {
        id: mainBar
        parent: mainWindow_Layout

        Layout.row: 0
        Layout.rowSpan: 2
        Layout.column: 2
        Layout.preferredWidth: 10
        Layout.fillHeight: true

        Component.onCompleted: {
            addTarget(musicWidget.listView.view)
            addTarget(musicWidget.itemView.view)
            addTarget(fileWidget.listView.view)
            addTarget(fileWidget.itemView.view)
        }
    }

    YT_ControlWidget {
        id: controlWidget
    }

    SwipeView {
        id: mainWidget
        parent: mainWindow_Layout
        currentIndex: YT.widgetIndex
        onCurrentIndexChanged:  YT.widgetIndex = currentIndex

        clip: true
        spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

        Layout.row: 1
        Layout.column: 1
        Layout.fillWidth: true
        Layout.fillHeight: true

        YT_MusicWidget {
            id: musicWidget
        }

        YT_FileWidget {
            id: fileWidget
        }


        YT_ListSelectView {
            id: listSelectView
            model: ListModel {
                id: listModel
                Component.onCompleted: {
                    for (var i = 1; i <= 100; i++) {
                        append({ display: "Item " + i })
                    }
                }
            }
            delegate: Rectangle {
                width: parent ? parent.width : width
                height: 40
                border.color: "black"
                color: listSelectView.view.currentIndex === index ? "red" : "green"
                Text {
                    text: model.display
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: listSelectView.view.currentIndex = index
                }
            }
        }

        Rectangle {
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(0, 0, 255, 0.5) } // 从透明开始
                GradientStop { position: 0.5; color: Qt.rgba(0, 0, 255, 0.5) } // 半透明的蓝色
                GradientStop { position: 1.0; color: "blue" } // 到不透明的蓝色
            }
        }
    }

    // Rectangle {
    //     id: mainWidget
    //     parent: mainWindow_Layout

    //     color: "green"

    //     Layout.row: 1
    //     Layout.column: 0
    //     Layout.fillWidth: true
    //     Layout.fillHeight: true
    // }

    // YT_TreeView {
    //     id: mainWidget
    //     parent: mainWindow_Layout

    //     model: FileListModel { }

    //     padding: 7

    //     Layout.row: 1
    //     Layout.column: 0
    //     Layout.fillWidth: true
    //     Layout.fillHeight: true
    // }

    // YT_TabBar {
    //     id: bar
    //     parent: mainWindow_Layout

    //     Layout.row: 1
    //     Layout.column: 0
    //     Layout.fillWidth: true

    //     // TabButton {
    //     //     text: qsTr("Home")
    //     // }
    //     // TabButton {
    //     //     text: qsTr("Discover")
    //     // }
    //     // TabButton {
    //     //     text: qsTr("Activity")
    //     // }

    //     // Rectangle {
    //     //     parent: bar
    //     //     anchors.fill: parent
    //     //     color: "red"
    //     // }
    // }

    // YT_ListView {
    //     parent: mainWindow_Layout

    //     Layout.row: 1
    //     Layout.column: 0
    //     Layout.fillWidth: true
    //     Layout.fillHeight: true

    //     padding: 7
    //     model: ListModel {
    //         Component.onCompleted: {
    //             for (var i = 0; i < 100; i++) {
    //                 append({"display": "Item " + (i + 1)})
    //             }
    //         }
    //     }
    // }

    // YT_SplitView {
    //     anchors.fill: parent
    //     Rectangle {
    //         SplitView.minimumWidth: 25
    //         SplitView.preferredWidth: 50
    //         SplitView.maximumWidth: 100

    //         color: "red"
    //     }

    //     Rectangle {
    //         SplitView.minimumWidth: 25
    //         SplitView.preferredWidth: 50
    //         SplitView.maximumWidth: 100

    //         color: "red"
    //     }
    // }
}
