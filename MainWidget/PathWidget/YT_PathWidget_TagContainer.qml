import QtQuick
import YT_Material as YT_M

Row {
    id: root
    spacing: YT_M.Theme.Space75
    property alias model: repeater.model
    property alias delegate: repeater.delegate
    Repeater {
        id: repeater
        model: parent.model
        delegate: parent.delegate
    }
}
