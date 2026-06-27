import QtQuick
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform
import YT_Player
import HuskarUI.Basic

YT_Window {
    id: mainWindow

    Item {
        Component.onCompleted: {
            YT.mainScrollBar.addTarget(YT.musicWidget.fileView.view);
            YT.mainScrollBar.addTarget(YT.fileWidget.listView.view);
            YT.mainScrollBar.addTarget(YT.fileWidget.itemView.view);
            HusTheme.darkMode = HusTheme.Dark;
        }
    }

    YT_TransformWindow {
        target: Window.window
    }

    SystemTrayIcon {
        visible: true
        icon.source: "qrc:/Ui_Image/music_logo.png"
    }

    Rectangle {
        id: mainBackground
        anchors.fill: parent
        color: YT_Info.BackgroundColor
        radius: YT_Info.Radius
    }

    GridLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: YT_Info.Margin

        rows: 2
        rowSpacing: anchors.margins
        columns: 3
        columnSpacing: anchors.margins
    }

    YT_PathWidget {
        id: pathWidget
        parent: mainLayout

        Layout.row: 0
        Layout.column: 1
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Layout.preferredHeight: 30
    }

    Item {
        z: +true
        parent: mainLayout

        Layout.row: 0
        Layout.rowSpan: 2
        Layout.column: 0
        Layout.preferredWidth: navigationWidget.collapseSize + navigationWidget.padding * 2
        Layout.fillHeight: true

        YT_NavigationWidget {
            id: navigationWidget
            width: implicitWidth
            height: parent.height

            Behavior on implicitWidth {
                NumberAnimation {
                    duration: 180
                    easing.type: Easing.OutQuad
                }
            }
        }

        MultiEffect {
            source: navigationWidget
            anchors.fill: navigationWidget

            visible: opacity != 0
            opacity: navigationWidget.expanded
            Behavior on opacity {
                NumberAnimation {
                    duration: 180
                    easing.type: Easing.OutQuad
                }
            }

            shadowEnabled: true
            shadowColor: YT_Info.FontColor
            shadowBlur: 0.3
        }
    }

    YT_ScrollBarList {
        id: mainScrollBar
        parent: mainLayout

        Layout.row: 1
        Layout.column: 2
        Layout.preferredWidth: 10
        Layout.fillHeight: true
        Layout.rightMargin: YT_Info.MarginSmall
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

    HusNotification {}

    YT_ControlWidget {
        id: controlWidget
    }

    YT_PageWidget {}

    SwipeView {
        id: mainWidget
        parent: mainLayout
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

        YT_PictureWidget {
            id: pictureWidget
        }

        NE_View {
            id: editWidget
        }

        // Flow {
        //     spacing: 15
        //     SplitView {
        //         width: parent.width
        //         height: 100
        //         horizontalPadding: 10
        //         Rectangle {
        //             implicitWidth: 100
        //             implicitHeight: 100
        //             color: 'red'
        //         }
        //         Rectangle {
        //             implicitWidth: 100
        //             implicitHeight: 100
        //             color: 'green'
        //         }
        //     }
        //     HusButton {
        //         type: HusButton.Type_Primary
        //         text: 'Display normal message'
        //         property int datas: 0
        //         onClicked: {
        //             YT_Func.sendMessage(datas % 4, "hello message");
        //             datas = datas + 1;
        //         }
        //     }
        //     YT_RoundImage {
        //         asynchronous: true
        //         radius: width / 2
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         source: "image://MusicImage/" + 2777
        //     }
        //     YT_RoundImage {
        //         asynchronous: true
        //         imageSource.mipmap: true
        //         radius: width / 2
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         source: "image://MusicImage/" + 2777
        //     }
        //     YT_RoundImage {
        //         asynchronous: true
        //         imageSource.smooth: true
        //         radius: width / 2
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         source: "image://MusicImage/" + 2777
        //     }
        //     YT_RoundImage {
        //         radius: width / 2
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         source: 'https://avatars.githubusercontent.com/u/33405710?v=4'
        //     }
        //     YT_RoundImage {
        //         radius: 15
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         source: 'https://avatars.githubusercontent.com/u/33405710?v=4'
        //     }
        //     HusAvatar {
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         imageSource: 'https://avatars.githubusercontent.com/u/33405710?v=4'
        //     }
        //     YT_Image {
        //         id: imageSource
        //         radius: 0
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         source: 'https://avatars.githubusercontent.com/u/33405710?v=4'
        //     }
        //     ShaderEffect {
        //         property real radius: 50
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         property variant source: ShaderEffectSource {
        //             sourceItem: imageSource
        //         }
        //         fragmentShader: "qrc:/Shaders/YT_RoundImage.frag.qsb"
        //     }
        //     YT_Image {
        //         radius: width / 2
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         source: 'https://avatars.githubusercontent.com/u/33405710?v=4'
        //     }
        //     Rectangle {
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         color: 'green'
        //         radius: width / 2
        //     }
        //     Rectangle {
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         color: 'green'
        //         radius: width / 2
        //         antialiasing: true
        //     }
        //     ShaderEffect {
        //         implicitWidth: 100
        //         implicitHeight: 100
        //         //! [properties]
        //         property variant source: ShaderEffectSource {
        //             sourceItem: imageSource
        //         }
        //         property real bend
        //         property real minimize
        //         property real side: genieSlider.value
        //         SequentialAnimation on bend {
        //             loops: Animation.Infinite
        //             NumberAnimation {
        //                 to: 1
        //                 duration: 700
        //                 easing.type: Easing.InOutSine
        //             }
        //             PauseAnimation {
        //                 duration: 1600
        //             }
        //             NumberAnimation {
        //                 to: 0
        //                 duration: 700
        //                 easing.type: Easing.InOutSine
        //             }
        //             PauseAnimation {
        //                 duration: 1000
        //             }
        //         }
        //         SequentialAnimation on minimize {
        //             loops: Animation.Infinite
        //             PauseAnimation {
        //                 duration: 300
        //             }
        //             NumberAnimation {
        //                 to: 1
        //                 duration: 700
        //                 easing.type: Easing.InOutSine
        //             }
        //             PauseAnimation {
        //                 duration: 1000
        //             }
        //             NumberAnimation {
        //                 to: 0
        //                 duration: 700
        //                 easing.type: Easing.InOutSine
        //             }
        //             PauseAnimation {
        //                 duration: 1300
        //             }
        //         }
        //         //! [properties]
        //         //! [vertex]
        //         mesh: Qt.size(10, 10)
        //         vertexShader: "qrc:/Shaders/genie.vert.qsb"
        //         //! [vertex]
        //         Slider {
        //             id: genieSlider
        //             anchors.left: parent.left
        //             anchors.right: parent.right
        //             anchors.bottom: parent.bottom
        //             anchors.leftMargin: 4
        //             anchors.rightMargin: 4
        //             height: 40
        //         }
        //     }
        // }
    }
}
