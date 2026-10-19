import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import YT_Material as YT_M

Item {
    id: root

    readonly property var accentRoles: [
        {
            name: "Primary",
            color: YT_M.Theme.Primary,
            onColor: YT_M.Theme.OnPrimary,
            container: YT_M.Theme.PrimaryContainer,
            onContainer: YT_M.Theme.OnPrimaryContainer,
            fixed: YT_M.Theme.PrimaryFixed,
            fixedDim: YT_M.Theme.PrimaryFixedDim,
            onFixed: YT_M.Theme.OnPrimaryFixed,
            onFixedVariant: YT_M.Theme.OnPrimaryFixedVariant
        },
        {
            name: "Secondary",
            color: YT_M.Theme.Secondary,
            onColor: YT_M.Theme.OnSecondary,
            container: YT_M.Theme.SecondaryContainer,
            onContainer: YT_M.Theme.OnSecondaryContainer,
            fixed: YT_M.Theme.SecondaryFixed,
            fixedDim: YT_M.Theme.SecondaryFixedDim,
            onFixed: YT_M.Theme.OnSecondaryFixed,
            onFixedVariant: YT_M.Theme.OnSecondaryFixedVariant
        },
        {
            name: "Tertiary",
            color: YT_M.Theme.Tertiary,
            onColor: YT_M.Theme.OnTertiary,
            container: YT_M.Theme.TertiaryContainer,
            onContainer: YT_M.Theme.OnTertiaryContainer,
            fixed: YT_M.Theme.TertiaryFixed,
            fixedDim: YT_M.Theme.TertiaryFixedDim,
            onFixed: YT_M.Theme.OnTertiaryFixed,
            onFixedVariant: YT_M.Theme.OnTertiaryFixedVariant
        },
        {
            name: "Error",
            color: YT_M.Theme.Error,
            onColor: YT_M.Theme.OnError,
            container: YT_M.Theme.ErrorContainer,
            onContainer: YT_M.Theme.OnErrorContainer
        }
    ]

    readonly property var surfaceRoles: [
        {
            label: "Surface Dim",
            color: YT_M.Theme.SurfaceDim,
            textColor: YT_M.Theme.OnSurface
        },
        {
            label: "Surface",
            color: YT_M.Theme.Surface,
            textColor: YT_M.Theme.OnSurface
        },
        {
            label: "Surface Bright",
            color: YT_M.Theme.SurfaceBright,
            textColor: YT_M.Theme.OnSurface
        },
        {
            label: "Inverse Surface",
            color: YT_M.Theme.InverseSurface,
            textColor: YT_M.Theme.InverseOnSurface
        },
        {
            label: "Inverse On Surface",
            color: YT_M.Theme.InverseOnSurface,
            textColor: YT_M.Theme.InverseSurface
        },
        {
            label: "Inverse Primary",
            color: YT_M.Theme.InversePrimary,
            textColor: YT_M.Theme.OnPrimary
        }
    ]

    readonly property var surfaceContainerRoles: [
        {
            label: "Surface Container Lowest",
            color: YT_M.Theme.SurfaceContainerLowest
        },
        {
            label: "Surface Container Low",
            color: YT_M.Theme.SurfaceContainerLow
        },
        {
            label: "Surface Container",
            color: YT_M.Theme.SurfaceContainer
        },
        {
            label: "Surface Container High",
            color: YT_M.Theme.SurfaceContainerHigh
        },
        {
            label: "Surface Container Highest",
            color: YT_M.Theme.SurfaceContainerHighest
        }
    ]

    readonly property var outlineRoles: [
        {
            label: "On Surface",
            color: YT_M.Theme.OnSurface,
            textColor: YT_M.Theme.Surface
        },
        {
            label: "On Surface Variant",
            color: YT_M.Theme.OnSurfaceVariant,
            textColor: YT_M.Theme.Surface
        },
        {
            label: "Outline",
            color: YT_M.Theme.Outline,
            textColor: YT_M.Theme.Surface
        },
        {
            label: "Outline Variant",
            color: YT_M.Theme.OutlineVariant,
            textColor: YT_M.Theme.OnSurface
        },
        {
            label: "Scrim",
            color: YT_M.Theme.Scrim,
            textColor: YT_M.Theme.Surface
        },
        {
            label: "Shadow",
            color: YT_M.Theme.Shadow,
            textColor: YT_M.Theme.Surface
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

            RowLayout {
                Layout.fillWidth: true
                spacing: YT_M.Theme.Space100

                YT_M.Button {
                    text: "选择颜色"
                    styleType: YT_M.Button.Tonal
                    onClicked: colorDialog.open()
                }

                YT_M.Button {
                    text: "从图片选择"
                    styleType: YT_M.Button.Tonal
                    onClicked: fileDialog.open()
                }

                YT_M.Button {
                    text: "对比度"
                    styleType: YT_M.Button.Tonal
                    onClicked: {
                        contrastSlider.value = 0.0;
                        YT_M.Theme.themeContrastLevel = 0.0;
                    }
                }

                YT_M.Slider {
                    id: contrastSlider
                    from: -1
                    to: +1
                    onPressedChanged: {
                        if (pressed == false) {
                            YT_M.Theme.themeContrastLevel = value;
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                Layout.fillWidth: true

                YT_M.Button {
                    text: "深色模式"
                    styleType: YT_M.Button.Tonal
                    checkable: true
                    checked: YT_M.Theme.themeIsDark
                    onToggled: YT_M.Theme.themeIsDark = checked
                }

                YT_M.ButtonContainer {
                    sizeType: YT_M.Button.Xsmall
                    styleType: YT_M.Button.Tonal
                    exclusive: true

                    YT_M.ButtonContainer.M_Item {
                        text: "内容"
                        checked: YT_M.Theme.themeType === YT_M.Theme.SchemeContent
                        onToggled: if (checked)
                            YT_M.Theme.themeType = YT_M.Theme.SchemeContent
                    }
                    YT_M.ButtonContainer.M_Item {
                        text: "鲜艳"
                        checked: YT_M.Theme.themeType === YT_M.Theme.SchemeVibrant
                        onToggled: if (checked)
                            YT_M.Theme.themeType = YT_M.Theme.SchemeVibrant
                    }
                    YT_M.ButtonContainer.M_Item {
                        text: "表现"
                        checked: YT_M.Theme.themeType === YT_M.Theme.SchemeExpressive
                        onToggled: if (checked)
                            YT_M.Theme.themeType = YT_M.Theme.SchemeExpressive
                    }
                    YT_M.ButtonContainer.M_Item {
                        text: "忠实"
                        checked: YT_M.Theme.themeType === YT_M.Theme.SchemeFidelity
                        onToggled: if (checked)
                            YT_M.Theme.themeType = YT_M.Theme.SchemeFidelity
                    }
                    YT_M.ButtonContainer.M_Item {
                        text: "中性"
                        checked: YT_M.Theme.themeType === YT_M.Theme.SchemeNeutral
                        onToggled: if (checked)
                            YT_M.Theme.themeType = YT_M.Theme.SchemeNeutral
                    }
                    YT_M.ButtonContainer.M_Item {
                        text: "单色"
                        checked: YT_M.Theme.themeType === YT_M.Theme.SchemeMonochrome
                        onToggled: if (checked)
                            YT_M.Theme.themeType = YT_M.Theme.SchemeMonochrome
                    }
                    YT_M.ButtonContainer.M_Item {
                        text: "色调点"
                        checked: YT_M.Theme.themeType === YT_M.Theme.SchemeTonalSpot
                        onToggled: if (checked)
                            YT_M.Theme.themeType = YT_M.Theme.SchemeTonalSpot
                    }
                    YT_M.ButtonContainer.M_Item {
                        text: "彩虹"
                        checked: YT_M.Theme.themeType === YT_M.Theme.SchemeRainbow
                        onToggled: if (checked)
                            YT_M.Theme.themeType = YT_M.Theme.SchemeRainbow
                    }
                    YT_M.ButtonContainer.M_Item {
                        text: "果蔬沙拉"
                        checked: YT_M.Theme.themeType === YT_M.Theme.SchemeFruitSalad
                        onToggled: if (checked)
                            YT_M.Theme.themeType = YT_M.Theme.SchemeFruitSalad
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: width >= 800 ? 4 : width >= 480 ? 2 : 1
                columnSpacing: YT_M.Theme.Space150
                rowSpacing: YT_M.Theme.Space150

                Repeater {
                    model: root.accentRoles
                    delegate: ColumnLayout {
                        id: accentDelegate
                        required property var modelData
                        Layout.fillWidth: true
                        spacing: YT_M.Theme.Space50

                        Swatch {
                            Layout.fillWidth: true
                            label: accentDelegate.modelData.name
                            swatchColor: accentDelegate.modelData.color
                            textColor: accentDelegate.modelData.onColor
                        }
                        Swatch {
                            Layout.fillWidth: true
                            label: `On ${accentDelegate.modelData.name}`
                            swatchColor: accentDelegate.modelData.onColor
                            textColor: accentDelegate.modelData.color
                        }
                        Swatch {
                            Layout.fillWidth: true
                            label: `${accentDelegate.modelData.name} Container`
                            swatchColor: accentDelegate.modelData.container
                            textColor: accentDelegate.modelData.onContainer
                            visible: accentDelegate.modelData.container !== undefined
                        }
                        Swatch {
                            Layout.fillWidth: true
                            label: `On ${accentDelegate.modelData.name} Container`
                            swatchColor: accentDelegate.modelData.onContainer
                            textColor: accentDelegate.modelData.container
                            visible: accentDelegate.modelData.onContainer !== undefined
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: YT_M.Theme.Space25
                            visible: accentDelegate.modelData.fixed !== undefined

                            Swatch {
                                Layout.fillWidth: true
                                label: `${accentDelegate.modelData.name} Fixed`
                                swatchColor: accentDelegate.modelData.fixed !== undefined ? accentDelegate.modelData.fixed : YT_M.Theme.Surface
                                textColor: accentDelegate.modelData.onFixed !== undefined ? accentDelegate.modelData.onFixed : YT_M.Theme.OnSurface
                            }
                            Swatch {
                                Layout.fillWidth: true
                                label: `${accentDelegate.modelData.name} Fixed Dim`
                                swatchColor: accentDelegate.modelData.fixedDim !== undefined ? accentDelegate.modelData.fixedDim : YT_M.Theme.Surface
                                textColor: accentDelegate.modelData.onFixed !== undefined ? accentDelegate.modelData.onFixed : YT_M.Theme.OnSurface
                            }
                        }
                        Swatch {
                            Layout.fillWidth: true
                            label: `On ${accentDelegate.modelData.name} Fixed`
                            swatchColor: accentDelegate.modelData.onFixed !== undefined ? accentDelegate.modelData.onFixed : YT_M.Theme.OnSurface
                            textColor: accentDelegate.modelData.fixed !== undefined ? accentDelegate.modelData.fixed : YT_M.Theme.Surface
                            visible: accentDelegate.modelData.onFixed !== undefined
                        }
                        Swatch {
                            Layout.fillWidth: true
                            label: `On ${accentDelegate.modelData.name} Fixed Variant`
                            swatchColor: accentDelegate.modelData.onFixedVariant !== undefined ? accentDelegate.modelData.onFixedVariant : YT_M.Theme.OnSurface
                            textColor: accentDelegate.modelData.fixed !== undefined ? accentDelegate.modelData.fixed : YT_M.Theme.Surface
                            visible: accentDelegate.modelData.onFixedVariant !== undefined
                        }
                    }
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: width >= 800 ? 3 : width >= 480 ? 2 : 1
                columnSpacing: YT_M.Theme.Space150
                rowSpacing: YT_M.Theme.Space150

                Repeater {
                    model: root.surfaceRoles
                    delegate: Swatch {
                        id: surfaceRoleSwatch
                        required property var modelData
                        Layout.fillWidth: true
                        label: surfaceRoleSwatch.modelData.label
                        swatchColor: surfaceRoleSwatch.modelData.color
                        textColor: surfaceRoleSwatch.modelData.textColor
                    }
                }

                Repeater {
                    model: root.surfaceContainerRoles
                    delegate: Swatch {
                        id: surfaceContainerSwatch
                        required property var modelData
                        Layout.fillWidth: true
                        label: surfaceContainerSwatch.modelData.label
                        swatchColor: surfaceContainerSwatch.modelData.color
                        textColor: YT_M.Theme.OnSurface
                    }
                }

                Repeater {
                    model: root.outlineRoles
                    delegate: Swatch {
                        id: outlineSwatch
                        required property var modelData
                        Layout.fillWidth: true
                        label: outlineSwatch.modelData.label
                        swatchColor: outlineSwatch.modelData.color
                        textColor: outlineSwatch.modelData.textColor
                    }
                }
            }
        }
    }

    component Swatch: Rectangle {
        id: swatch
        required property string label
        required property color swatchColor
        required property color textColor

        implicitHeight: 54
        color: swatchColor

        Text {
            anchors.fill: parent
            anchors.margins: YT_M.Theme.Space100
            text: swatch.label
            color: swatch.textColor
            font.pixelSize: 14
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
        }
    }

    ColorDialog {
        id: colorDialog
        onAccepted: {
            YT_M.Theme.themeSource = selectedColor.toString();
        }
    }

    FileDialog {
        id: fileDialog
        fileMode: FileDialog.OpenFile
        nameFilters: ["Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.webp)"]
        onAccepted: {
            if (selectedFile) {
                YT_M.Theme.themeSource = selectedFile.toString();
            }
        }
    }
}
