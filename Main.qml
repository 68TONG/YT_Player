import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform
import YT_Player

YT_Window {
    id: mainWindow
    width: 640
    height: 480
    visible: true

    Item {
        Component.onCompleted: {
            YT.navigationWidget.model.append(YT.musicWidget.listView);
            YT.mainScrollBar.addTarget(YT.musicWidget.itemView.view);
            YT.mainScrollBar.addTarget(YT.fileWidget.listView.view);
            YT.mainScrollBar.addTarget(YT.fileWidget.itemView.view);
        }
    }

    YT_TransformWindow {
        target: Window.window
    }

    SystemTrayIcon {
        visible: true
        icon.source: "qrc:/Resource_UI/music_logo.png"
    }

    Rectangle {
        id: mainBackground
        anchors.fill: parent
        color: YT_Info.BackgroundColor
        radius: YT_Info.Radius
    }

    GridLayout {
        id: mainWindow_Layout
        anchors.fill: parent
        anchors.margins: YT_Info.Margin

        rows: 2
        rowSpacing: anchors.margins
        columns: 3
        columnSpacing: anchors.margins
    }

    Rectangle {
        id: mainTool
        parent: mainWindow_Layout

        color: YT_Info.ItemColor
        radius: YT_Info.Radius

        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true
        Layout.preferredHeight: 30

        RowLayout {
            anchors.fill: parent
            spacing: 0

            YT_MainPathBar {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Repeater {
                model: ListModel {
                    ListElement {
                        path: "qrc:/Resource_UI/full_screen.png"
                        operation: function () {
                            console.log("Function 1 called");
                        }
                    }
                    ListElement {
                        path: "qrc:/Resource_UI/minimize.png"
                        operation: function () {
                            console.log("Function 2 called");
                        }
                    }
                    ListElement {
                        path: "qrc:/Resource_UI/close.png"
                        operation: function () {
                            Qt.quit();
                        }
                    }
                }

                YT_Button {
                    Layout.preferredWidth: parent.height
                    Layout.preferredHeight: parent.height

                    inset: 4
                    padding: YT_Info.Radius

                    modelData: model.path
                    contentItem: YT_Button.LoadImage {}

                    onClicked: model.operation()
                }
            }
        }
    }

    YT_NavigationWidget {
        id: mainNavigation
        parent: mainWindow_Layout

        Layout.row: 0
        Layout.rowSpan: 2
        Layout.column: 0
        Layout.preferredWidth: implicitWidth
        Layout.fillHeight: true
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
            YT.mainScrollBar = this;
        }
    }

    YT_ControlWidget {
        id: controlWidget
    }

    YT_PageWidget {}

    SwipeView {
        id: mainWidget
        parent: mainWindow_Layout
        currentIndex: YT.widgetIndex
        onCurrentIndexChanged: YT.widgetIndex = currentIndex
        onCurrentItemChanged: currentItem.forceActiveFocus()

        clip: true
        spacing: YT_Info.Spacing
        interactive: currentItem != editWidget

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

        NE_View {
            id: editWidget
        }

        Column {
            id: test
            Rectangle {
                width: parent.width
                height: parent.height > 500 ? 100 : 0
                color: 'blue'
                Behavior on height {
                    NumberAnimation {}
                }
            }
            Rectangle {
                width: parent.width
                height: 100
                color: 'red'
            }
        }
    }
}
