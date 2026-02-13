import QtQuick
import YT_Player

NE_Group {
    id: root

    readonly property NE_MediaOutput_Model modelNE_: NE_MediaOutput_Model {
        id: modelNE_
    }

    NE_TextInput {
        id: it_FileName
        enabled: true
        inputNode: NE_Node {}
        dataModelNE_: modelNE_
        dataNE_Shared: true
        _NE.placeholderText: "FileName"

        _NE.onTextChanged: dataNE_ = _NE.text
        Binding {
            modelNE_.fileName: it_FileName.dataNE_
        }
        Connections {
            target: modelNE_
            function onFileNameChanged() {
                it_FileName.dataNE_ = modelNE_.fileName;
            }
        }
    }

    NE_TextInput {
        id: it_FileType
        _NE.placeholderText: "FileType"
        _NE.model: YT_FilterModel {
            filter_data: it_FileType.dataNE_
            filter_model: ["mp3", "mp4", "mkv", "flac", "wav", "aac", "png", "jpeg"]
        }

        _NE.onTextChanged: dataNE_ = _NE.text
        _NE.onClickedModelDataChanged: dataNE_ = _NE.clickedModelData
        Binding {
            modelNE_.fileType: it_FileType.dataNE_
        }
        Connections {
            target: modelNE_
            function onFileTypeChanged() {
                it_FileType.dataNE_ = modelNE_.fileType;
            }
        }
    }

    YT_Button {
        implicitWidth: parent ? parent.width : 0

        modelData: "确定"
        contentItem: YT_Button.LoadText {}
        background: YT_Rectangle {
            radius: YT_Info.RadiusSmall
            color: (parent.hovered || parent.down) ? YT_Info.ItemFocusColor : YT_Info.ItemColor
        }
        onClicked: modelNE_.start()
    }

    property bool droping: false
    background.border.color: droping || selected ? YT_Info.FontFocusColor : YT_Info.ItemFocusColor
    DropArea {
        id: dropArea
        parent: root
        anchors.fill: parent
        onEntered: function (drag) {
            root.droping = true;
        }
        onExited: function () {
            root.droping = false;
        }
        onDropped: function (drop) {
            const traget = drop.source;
            if (traget.typeIO_ !== true)
                return;
            modelNE_.insertOutputData(traget.dataNE_);
        }
    }

    readonly property Instantiator _NE_Instantiator: Instantiator {
        id: _NE_Instantiator
        model: modelNE_.dataModel
        delegate: NE_Display {
            required property int index
            required property NE_Data modelData
            inputNode: NE_Node {}
            contentItem: NE_Display.LoadText {}

            dataNE_Shared: true
            displayData: "inputItem"
            dataNE_: modelData ? modelData.dataNE_ : dataNE_
            typeNE_: modelData ? modelData.typeNE_ : typeNE_
            onInputDataChanged: {
                if (inputData)
                    return;
                modelNE_.removeOutputData(index);
            }
        }
        onObjectAdded: function (index, object) {
            object.parent = root.contentItem;
            object.connectData(false, object.modelData);
        }
    }
}
