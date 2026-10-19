import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YT_Material as YT_M

Item {
    id: root

    property int itemCount: 0
    readonly property var itemColors: ["#6750A4", "#006A60", "#8C4A60", "#526000", "#415F91", "#8A4F00"]

    function addItem() {
        const sizes = [[112, 52], [78, 68], [142, 44], [92, 58], [126, 62], [68, 48]];
        const size = sizes[itemCount % sizes.length];
        const item = itemComponent.createObject(flow, {
            width: size[0],
            height: size[1],
            label: "Item " + (itemCount + 1),
            cardColor: itemColors[itemCount % itemColors.length]
        });

        if (!item) {
            console.error("Failed to create Flow demo item");
            return;
        }
        ++itemCount;
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Label {
            Layout.fillWidth: true
            text: "紧凑 Flow 布局"
            font.pixelSize: 22
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label {
                text: "Flow 宽度"
            }

            Slider {
                id: widthSlider
                Layout.fillWidth: true
                from: 120
                to: Math.max(from, preview.width - 24)
                value: Math.min(280, to)
            }

            Label {
                text: Math.round(widthSlider.value) + " px"
                Layout.minimumWidth: 55
            }

            Button {
                text: "添加 Item"
                onClicked: root.addItem()
            }
        }

        Rectangle {
            id: preview
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#F3F0F7"
            radius: 12
            clip: true

            YT_M.Flow {
                id: flow
                width: widthSlider.value
                height: Math.max(preview.height, implicitHeight)
                spacing: 8
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Component {
        id: itemComponent

        Rectangle {
            required property string label
            required property color cardColor

            radius: 8
            color: cardColor

            Label {
                anchors.centerIn: parent
                text: parent.label
                color: "white"
                font.bold: true
            }
        }
    }

    Component.onCompleted: {
        for (let i = 0; i < 6; ++i)
            addItem();
    }
}
