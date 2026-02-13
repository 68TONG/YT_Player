import QtQuick
import QtQuick.Controls
import YT_Player

Container {
    id: root

    contentItem: Row {
        Repeater {
            id: producer
            model: ["dddd", "dasdasd", "dasd", "saaaa", "dsadasj"]
            delegate: YT_Button {
                required property var model

                modelData: model.modelData
                contentItem: loadText.createObject()
                background: loadBackground.createObject()
            }
        }
    }

    // YT_ComboBox {
    //     model: ["dddd", "dasdasd", "dasd", "saaaa", "dsadasj"]
    // }

    property alias model: producer.model
    property alias delegate: producer.delegate
}
