import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YT_Material as YT_M

Item {
    id: iconTestPage

    property bool fontFill: false
    property int fontWght: 400
    property int fontGrade: 0
    property int fontOpticalSize: 24

    readonly property list<QtObject> iconSamples: [
        QtObject {
            property string label: "LibraryMusic"
            property int source: YT_M.IconProvider.LibraryMusic
        },
        QtObject {
            property string label: "Favorite"
            property int source: YT_M.IconProvider.Favorite
        },
        QtObject {
            property string label: "PlayArrow"
            property int source: YT_M.IconProvider.PlayArrow
        },
        QtObject {
            property string label: "Home"
            property int source: YT_M.IconProvider.Home
        },
        QtObject {
            property string label: "SettingsApplications"
            property int source: YT_M.IconProvider.SettingsApplications
        }
    ]

    ScrollView {
        id: scrollView
        anchors.fill: parent
        anchors.margins: YT_M.Theme.Space200
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: scrollView.availableWidth
            spacing: YT_M.Theme.Space200

            GridLayout {
                Layout.fillWidth: true
                columns: 3
                rowSpacing: YT_M.Theme.Space100
                columnSpacing: YT_M.Theme.Space100

                Repeater {
                    model: iconTestPage.iconSamples

                    delegate: Rectangle {
                        id: iconCard
                        required property QtObject modelData

                        Layout.fillWidth: true
                        implicitHeight: 112
                        radius: 8
                        color: YT_M.Theme.SurfaceContainerLow

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: YT_M.Theme.Space100
                            spacing: YT_M.Theme.Space50

                            YT_M.Icon {
                                Layout.alignment: Qt.AlignHCenter
                                iconSource: iconCard.modelData.source
                                iconSize: 48
                                iconColor: YT_M.Theme.OnSurface
                                fontFill: iconTestPage.fontFill
                                fontWght: iconTestPage.fontWght
                                fontGrade: iconTestPage.fontGrade
                                fontOpticalSize: iconTestPage.fontOpticalSize
                            }

                            Text {
                                Layout.alignment: Qt.AlignHCenter
                                text: iconCard.modelData.label
                                color: YT_M.Theme.OnSurface
                            }
                        }
                    }
                }
            }

            CheckBox {
                text: "fontFill"
                checked: iconTestPage.fontFill
                onToggled: iconTestPage.fontFill = checked
            }

            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: "fontWght"
                    Layout.preferredWidth: 150
                }
                Slider {
                    Layout.fillWidth: true
                    from: 100
                    to: 700
                    stepSize: 1
                    snapMode: Slider.SnapAlways
                    value: iconTestPage.fontWght
                    onValueChanged: iconTestPage.fontWght = Math.round(value)
                }
                Label {
                    text: iconTestPage.fontWght
                    horizontalAlignment: Text.AlignRight
                    Layout.preferredWidth: 45
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: "fontGrade"
                    Layout.preferredWidth: 150
                }
                Slider {
                    Layout.fillWidth: true
                    from: -50
                    to: 200
                    stepSize: 1
                    snapMode: Slider.SnapAlways
                    value: iconTestPage.fontGrade
                    onValueChanged: iconTestPage.fontGrade = Math.round(value)
                }
                Label {
                    text: iconTestPage.fontGrade
                    horizontalAlignment: Text.AlignRight
                    Layout.preferredWidth: 45
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: "fontOpticalSize"
                    Layout.preferredWidth: 150
                }
                Slider {
                    Layout.fillWidth: true
                    from: 20
                    to: 48
                    stepSize: 1
                    snapMode: Slider.SnapAlways
                    value: iconTestPage.fontOpticalSize
                    onValueChanged: iconTestPage.fontOpticalSize = Math.round(value)
                }
                Label {
                    text: iconTestPage.fontOpticalSize
                    horizontalAlignment: Text.AlignRight
                    Layout.preferredWidth: 45
                }
            }
        }
    }
}
