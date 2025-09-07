import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import YT_Player

RowLayout {
    id: root
    spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

    Item {
        Layout.preferredWidth: parent.height
        Layout.preferredHeight: parent.height
        Image {
            anchors.fill: parent
            anchors.margins: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

            source: "qrc:/Resource_UI/music_logo.png"
            fillMode: Image.PreserveAspectFit  // 按比例缩放图片
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 7
            Layout.leftMargin: 25
            Layout.rightMargin: 25
            spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Big)

            Repeater {
                model: [YT.musicWidget.metaData_Title, YT.musicWidget.metaData_One, YT.musicWidget.metaData_Two]
                delegate: YT_TextList {
                    required property string modelData

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    currentIndex: 0
                    isPlayTextList: true
                    textList: [""]
                    Component.onCompleted: {
                        YT_AudioOutput.onCurMusicInfoChanged.connect(function() {
                            textList = YT_AudioOutput.getCurMusicInfo(modelData)
                        })
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 3
            spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius)

            Text {
                Layout.fillHeight: true

                text: YT.formatTime(YT_AudioOutput.position)
                color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)
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
                    if (pressed) return value = value
                    YT_AudioOutput.position = value
                    value = Qt.binding(function(){return YT_AudioOutput.position})
                    if (YT_AudioOutput.playing === false) YT_AudioOutput.play()
                }
            }

            Text {
                Layout.fillHeight: true

                text: YT.formatTime(YT_AudioOutput.duration)
                color: YT_ConfigureInfo.getData(YT_ConfigureInfo.FontColor)
                font.pointSize: Qt.application.font.pointSize - 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 15
            // Layout.alignment: Qt.AlignHCenter
            spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)

            YT_Button {
                Layout.preferredWidth: height
                Layout.fillHeight: true

                padding: 0
                modelData: modelDataList[modelDataIndex]
                contentItem: YT_Button.LoadImage {}

                property int modelDataIndex: 0
                readonly property list<string> modelDataList: [
                    "qrc:/Resource_UI/shuffle.png",
                    "qrc:/Resource_UI/list_loop.png",
                    "qrc:/Resource_UI/single_loop.png"
                ]
                onClicked: {
                    var index = modelDataIndex + 1
                    if (index >= modelDataList.length)
                        index = 0
                    modelDataIndex = index
                }
                Component.onCompleted: {
                    YT_AudioOutput.playModel = Qt.binding(function(){return modelData})
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
                onClicked: {
                    listSelectView.open()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 20
            spacing: YT_ConfigureInfo.getData(YT_ConfigureInfo.ItemRadius_Small)

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
                modelData: YT_AudioOutput.playing ?
                               "qrc:/Resource_UI/playing.png" :
                               "qrc:/Resource_UI/stoping.png"
                contentItem: YT_Button.LoadImage {}
                onClicked: YT_AudioOutput.playing ? YT_AudioOutput.pause() : YT_AudioOutput.play()
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

    readonly property Popup listSelectView: Popup {
        id: listSelectView
        parent: root
        y: -(height + 7)
        width: parent.width
        height: 30
        padding: 0
        popupType: Popup.Window

        background: null
        contentItem: YT_ListSelectView {
            // color: Qt.rgba(0, 0, 0, 0)
            // border.color: Qt.rgba(0, 0, 0, 0)

            onCurrentIndexChanged_YT: YT.musicItemModel.setViewPosition(view.currentItem.modelData)

            view.orientation: Qt.Horizontal
            followBackground.widthPadding: 3
            followBackground.heightPadding: 3
            // followBackground.border.color: "#FFD700"
            followBackground.followItem: view.currentItem

            model: ListModel {
                Component.onCompleted: {
                    for (var i = 0; i < 26; i++) {
                        append({ modelData: String.fromCharCode(65 + i) })
                    }
                }
            }
            delegate: ItemDelegate {
                required property int index
                required property string modelData

                width: listSelectView.height
                height: listSelectView.height

                background: null
                contentItem: Text {
                    text: modelData

                    color: "#FFD700"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    ListView.view.currentIndex = index
                    listSelectView.contentItem.currentIndexChanged_YT()
                }
            }
        }
    }
}
