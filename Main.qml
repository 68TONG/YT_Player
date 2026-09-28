import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform
import QWindowKit as QWK
import YT_Material as YT_M
import HuskarUI.Basic
import YT_Player

Window {
    id: mainWindow
    width: 900
    height: 600
    visible: false
    color: YT_M.Theme.SurfaceContainer
    onWidthChanged: YT_M.Theme.breakPointValue = width

    QWK.WindowAgent {
        id: windowAgent
    }

    Component.onCompleted: {
        YT_M.Theme.breakPointList = [YT_M.Theme.BreakPointCompact, YT_M.Theme.BreakPointExpanded, YT_M.Theme.BreakPointExtraLarge];
        windowAgent.setup(mainWindow);
        // windowAgent.setTitleBar(titleBar);
        mainWindow.visible = true;
    }

    Item {
        Component.onCompleted: {
            YT.mainScrollBar.addTarget(YT.musicWidget.fileView.view);
            YT.mainScrollBar.addTarget(YT.fileWidget.listView.view);
            YT.mainScrollBar.addTarget(YT.fileWidget.itemView.view);
            HusTheme.darkMode = HusTheme.Dark;
        }
    }

    // YT_TransformWindow {
    //     target: Window.window
    // }

    GridLayout {
        id: mainCompactLayout
        visible: YT_M.Theme.breakPointType == YT_M.Theme.BreakPointCompact
        anchors.fill: parent

        LayoutItemProxy {
            target: mainWidget
            Layout.row: 0
            Layout.column: 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: YT_M.Theme.Space200
            Layout.rightMargin: YT_M.Theme.Space200
        }
    }

    GridLayout {
        id: mainExpandedLayout
        visible: YT_M.Theme.breakPointType == YT_M.Theme.BreakPointExpanded
        anchors.fill: parent

        LayoutItemProxy {
            target: navigationWidget
            Layout.row: 0
            Layout.rowSpan: 2
            Layout.column: 0
            Layout.preferredWidth: navigationWidget.collapseSize
            Layout.fillHeight: true
        }

        LayoutItemProxy {
            target: pathWidget
            Layout.row: 0
            Layout.column: 1
            Layout.fillWidth: true
            Layout.preferredHeight: pathWidget.implicitHeight
        }

        LayoutItemProxy {
            target: mainWidget
            Layout.row: 1
            Layout.column: 1
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rightMargin: YT_M.Theme.Space300
        }
    }

    GridLayout {
        id: mainExtraLargeLayout
        visible: YT_M.Theme.breakPointType == YT_M.Theme.BreakPointExtraLarge
        anchors.fill: parent

        LayoutItemProxy {
            target: navigationWidget
            Layout.row: 0
            Layout.rowSpan: 2
            Layout.column: 0
            Layout.preferredWidth: navigationWidget.expandSize
            Layout.fillHeight: true
        }

        LayoutItemProxy {
            target: pathWidget
            Layout.row: 0
            Layout.column: 1
            Layout.fillWidth: true
            Layout.preferredHeight: pathWidget.implicitHeight
        }

        LayoutItemProxy {
            target: mainWidget
            Layout.row: 1
            Layout.column: 1
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rightMargin: YT_M.Theme.Space300
        }
    }

    YT_PathWidget {
        id: pathWidget
    }

    YT_NavigationWidget {
        id: navigationWidget
        setPopup: mainExpandedLayout.visible
        setExpanded: mainExtraLargeLayout.visible
    }

    SwipeView {
        id: mainWidget
        currentIndex: YT.widgetIndex
        onCurrentIndexChanged: YT.widgetIndex = currentIndex
        onCurrentItemChanged: currentItem.forceActiveFocus()

        clip: true
        spacing: YT_Info.Spacing
        interactive: currentItem != editWidget

        YT_MusicWidget {
            id: musicWidget
        }

        YT_FileWidget {
            id: fileWidget
        }

        // YT_PictureWidget {
        //     id: pictureWidget
        // }

        NE_View {
            id: editWidget
        }

        Flow {
            id: testWidget
            YT_M.Slider {}
            Button {
                text: "open Drawer"
                onClicked: test.open()
            }
            YT_M.BottomSheet {
                id: test
                width: 640
                height: 400
                contentItem: Label {
                    text: "Content goes here!"
                    anchors.centerIn: parent
                }
            }
            YT_M.FoldableView {
                text: "Test"

                foldableView: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 50
                    border.color: 'red'
                }
            }
        }
    }

    SystemTrayIcon {
        visible: true
        icon.source: "qrc:/Ui_Image/music_logo.png"
    }

    YT_ScrollBarList {
        id: mainScrollBar

        // Layout.row: 1
        // Layout.column: 2
        // Layout.preferredWidth: 10
        // Layout.fillHeight: true
        // Layout.rightMargin: YT_Info.MarginSmall
        Component.onCompleted: {
            YT.mainScrollBar = this;
        }
    }

    HusMessage {
        id: mainMessage
        z: +true
        parent: Window.window.contentItem
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top

        Component.onCompleted: {
            YT.mainMessage = this;
        }
    }

    // YT_ControlWidget {
    //     id: controlWidget
    // }

    // YT_PageWidget {}
}
