import QtQuick
import Qt.labs.folderlistmodel
import YT_Player

NE_Group {
    id: root

    NE_TextInput {
        id: inputPath
        _NE.placeholderText: "Path"
        _NE.onTextEdited: dataNE_ = _NE.text
        Connections {
            function onDataNE_Changed() {
                modelNE_.folder = "file:///" + inputPath.dataNE_.replace(/\\/g, "/");
            }
        }
    }

    NE_ComboBox {
        id: fileList
        outputNode: NE_Node {}
        typeNE_: NE_Data_Model.NE_String

        _NE.model: modelNE_
        _NE.textRole: "fileName"
        _NE.valueRole: "filePath"
        _NE.delegate: YT_ComboBox.YT_Delegate {
            required property var modelData
            implicitWidth: parent ? parent.width : 0
            text: modelData.fileName
        }
    }

    readonly property FolderListModel modelNE_: FolderListModel {
        id: modelNE_
        showDirs: false
    }
}
