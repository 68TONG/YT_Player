import QtQuick
import YT_Player
import YT_Material as YT_M

YT_M.TextButton {
    required property YT_PathWidgetModel pathModel
    required property string pathData
    property string pathType

    text: pathData
    elide: Text.ElideRight
    mouseArea.onClicked: function (mouse) {
        if (mouse.modifiers == Qt.NoModifier) {
            pathModel.addSrcData(pathData);
        } else if (mouse.modifiers & Qt.ControlModifier) {
            pathModel.addSrcData('(' + pathType + ')' + pathData);
        }
    }
}
