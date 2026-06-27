import QtQuick
import QtQuick.Layouts
import YT_Player

RowLayout {
    id: root
    spacing: YT_Info.Spacing

    Item {
        Layout.preferredWidth: parent.height
        Layout.preferredHeight: parent.height
        Image {
            anchors.fill: parent
            anchors.margins: YT_Info.Margin

            source: "qrc:/Ui_Image/music_logo.png"
            fillMode: Image.PreserveAspectFit  // 按比例缩放图片
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: YT_Info.SpacingSmall

        Row {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 7
            Layout.leftMargin: 25
            Layout.rightMargin: 25
            spacing: YT_Info.SpacingBig
            move: Transition {
                NumberAnimation {
                    property: "x"
                    duration: 300
                    easing.type: Easing.OutCubic
                }
            }

            Repeater {
                model: [YT.musicFileModel.titleTag].concat(YT.musicFileModel.selectTag)
                delegate: YT_TextList {
                    required property string modelData
                    width: contentItem.implicitWidth
                    height: parent.height

                    currentIndex: 0
                    isPlayTextList: true
                    textList: YT_AudioOutput.playInfoID >= 0 ? YT_AudioOutput.getPlayInfoTags(modelData) : [""]
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 3
            spacing: YT_Info.Spacing

            Text {
                Layout.fillHeight: true

                text: YT.formatTime(YT_AudioOutput.position)
                color: YT_Info.FontColor
                font.pointSize: Qt.application.font.pointSize - 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            YT_Slider {
                Layout.fillWidth: true
                Layout.fillHeight: true
                background.anchors.centerIn: this

                from: 0
                value: YT_AudioOutput.position
                to: YT_AudioOutput.duration

                onPressedChanged: {
                    if (pressed)
                        return value = value;
                    YT_AudioOutput.position = value;
                    value = Qt.binding(function () {
                        return YT_AudioOutput.position;
                    });
                    if (YT_AudioOutput.playing === false)
                        YT_AudioOutput.play();
                }
            }

            Text {
                Layout.fillHeight: true

                text: YT.formatTime(YT_AudioOutput.duration)
                color: YT_Info.FontColor
                font.pointSize: Qt.application.font.pointSize - 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: YT_Info.SpacingSmall

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 15
            Layout.alignment: Qt.AlignHCenter
            spacing: YT_Info.SpacingSmall

            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: YT_AudioOutput.playMode
                contentItem: YT_Button.LoadImage {}

                readonly property list<string> modelDataList: ["qrc:/Ui_Image/shuffle.png", "qrc:/Ui_Image/list_loop.png", "qrc:/Ui_Image/single_loop.png"]
                onClicked: {
                    let index = modelDataList.indexOf(YT_AudioOutput.playMode);
                    index = index < 0 || index + 1 >= modelDataList.length ? 0 : index + 1;
                    YT_AudioOutput.playMode = modelDataList[index];
                }
            }
            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: "qrc:/Ui_Image/volume.png"
                contentItem: YT_Button.LoadImage {}
            }
            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: "qrc:/Ui_Image/operate.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: {}
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 20
            spacing: YT_Info.SpacingSmall

            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: "qrc:/Ui_Image/previous.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: YT_AudioOutput.playPreviousMusic()
            }
            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: YT_AudioOutput.playing ? "qrc:/Ui_Image/playing.png" : "qrc:/Ui_Image/stoping.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: YT_AudioOutput.playing ? YT_AudioOutput.pauseMusic() : YT_AudioOutput.playMusic()
            }
            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: "qrc:/Ui_Image/next.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: YT_AudioOutput.playNextMusic()
            }
        }
    }
}
