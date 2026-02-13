import QtQuick
import YT_Player

NE_Data {
    id: root

    enabled: dataNE_Shared == false || (dataNE_Shared == true && inputData == null)
    typeNE_: NE_Data_Model.NE_Not
    viewNE_: YT.findParent(parent, NE_View)

    inputPos: Qt.point(0, height / 2)
    outputPos: Qt.point(width, height / 2)

    onInputNodeChanged: {
        if (inputNode === null)
            return;
        inputNode.parent = root;
        inputNode.viewNE_ = Qt.binding(function () {
            return viewNE_;
        });
        inputNode.parentNE_ = Qt.binding(function () {
            return root;
        });
    }
    onOutputNodeChanged: {
        if (outputNode === null)
            return;
        outputNode.parent = root;
        outputNode.viewNE_ = Qt.binding(function () {
            return viewNE_;
        });
        outputNode.parentNE_ = Qt.binding(function () {
            return root;
        });
    }
}
