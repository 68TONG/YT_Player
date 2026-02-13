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

            source: "qrc:/Resource_UI/music_logo.png"
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
                model: [YT.musicItemModel.titleTag].concat(YT.musicItemModel.selectTag)
                delegate: YT_TextList {
                    required property string modelData
                    width: contentItem.implicitWidth
                    height: parent.height

                    currentIndex: 0
                    isPlayTextList: true
                    textList: [""]
                    function updateTextList() {
                        textList = YT_AudioOutput.getCurMusicInfo(modelData);
                    }
                    Component.onCompleted: {
                        YT_AudioOutput.onCurMusicInfoChanged.connect(updateTextList);
                    }
                    Component.onDestruction: {
                        YT_AudioOutput.onCurMusicInfoChanged.disconnect(updateTextList);
                    }
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
                modelData: modelDataList[modelDataIndex]
                contentItem: YT_Button.LoadImage {}

                property int modelDataIndex: 0
                readonly property list<string> modelDataList: ["qrc:/Resource_UI/shuffle.png", "qrc:/Resource_UI/list_loop.png", "qrc:/Resource_UI/single_loop.png"]
                onClicked: {
                    var index = modelDataIndex + 1;
                    if (index >= modelDataList.length)
                        index = 0;
                    modelDataIndex = index;
                }
                Component.onCompleted: {
                    YT_AudioOutput.playModel = Qt.binding(function () {
                        return modelData;
                    });
                }
            }
            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: "qrc:/Resource_UI/volume.png"
                contentItem: YT_Button.LoadImage {}
            }
            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: "qrc:/Resource_UI/operate.png"
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
                modelData: "qrc:/Resource_UI/previous.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: YT_AudioOutput.playPreviousMusic()
            }
            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: YT_AudioOutput.playing ? "qrc:/Resource_UI/playing.png" : "qrc:/Resource_UI/stoping.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: YT_AudioOutput.playing ? YT_AudioOutput.pauseMusic() : YT_AudioOutput.playMusic()
            }
            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: "qrc:/Resource_UI/next.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: YT_AudioOutput.playNextMusic()
            }
        }
    }
}
