import QtQuick
import YT_Player

Item {
    id: root

    property var textList: [""]
    property int currentIndex: 0
    property bool isPlayTextList: false
    property alias contentItem: contentItem
    onTextListChanged: currentIndex = 0

    implicitWidth: {
        var ret = 0
        for (var i = 0; i < textList.length; i++) {
            ret = contentItem_FontMetrics.boundingRect(textList[i]).width
        }
        return ret
    }
    implicitHeight: contentItem.implicitHeight

    function setCurrentIndex(index){
        setCurrentIndex_Animation.target_index = index
        setCurrentIndex_Animation.start()
    }

    YT_Text {
        id: contentItem
        anchors.fill: parent
        text: textList[currentIndex] ?? ""

        FontMetrics {
            id: contentItem_FontMetrics
            font: contentItem.font
        }
    }

    SequentialAnimation {
        id: setCurrentIndex_Animation
        property int target_index

        NumberAnimation {
            target: contentItem
            property: "opacity"
            to: 0 // 透明度从1变为0
            duration: 300 // 持续时间1秒
            easing.type: Easing.OutCubic
        }
        ScriptAction {
            script: {
                root.currentIndex = setCurrentIndex_Animation.target_index
            }
        }
        NumberAnimation {
            target: contentItem
            property: "opacity"
            to: 1 // 透明度从0变为1
            duration: 300 // 持续时间1秒
            easing.type: Easing.InQuad
        }
        // PauseAnimation {
        //     duration: 2000 // 等待2秒
        // }
    }

    Loader {
        sourceComponent: isPlayTextList ? setCurrentIndex_Animation_Loop : null
        Component {
            id: setCurrentIndex_Animation_Loop
            SequentialAnimation {
                running: root.textList.length > 1
                loops: Animation.Infinite // 设置为无限循环

                ScriptAction {
                    script: {
                        var index = (root.currentIndex + 1) % root.textList.length; // 更新文本索引
                        setCurrentIndex(index)
                    }
                }
                PauseAnimation {
                    duration: 2000 // 等待2秒
                }
            }
        }
    }
}
