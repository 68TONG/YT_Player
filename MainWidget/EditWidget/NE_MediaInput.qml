import QtQuick
import YT_Player

NE_Group {
    id: root
    enabled: modelNE_.isInputing.length == 0
    onEnabledChanged: {
        if (enabled == false)
            return;

        for (let it of it_Url.outputData) {
            let model = it.dataModelNE_;
            if (model instanceof NE_MediaOutput_Model == false)
                continue;
            for (let i = 0; i < _NE_Instantiator.count; i++) {
                let obj = _NE_Instantiator.objectAt(i);
                if (obj.modelData[NE_MediaInput_Model.IndexAutoConnect] == false)
                    continue;
                model.insertOutputData(obj.create_item);
            }
        }
    }

    readonly property NE_MediaInput_Model modelNE_: NE_MediaInput_Model {
        id: modelNE_
    }

    NE_TextInput {
        id: it_Url
        inputNode: NE_Node {}
        outputNode: NE_Node {}

        dataNE_Shared: true
        _NE.placeholderText: "URL"
        _NE.onEditingFinished: dataNE_ = _NE.text
        Component.onCompleted: dataNE_ = "C:/Users/YI_TONG/Music/日语/物语系列/さよならのゆくえ.flac"
        Binding {
            modelNE_.inputUrl: it_Url.dataNE_
        }
    }

    NE_CheckList {
        id: it_MatchUrl
        _NE.displayText: "MatchUrl"
        _NE.model: modelNE_.getMatchUrlModel()
        Binding {
            it_MatchUrl._NE.selectDatas: modelNE_.matchUrl
            modelNE_.matchUrl: it_MatchUrl._NE.selectDatas
        }
    }

    NE_TextInputArea {
        id: it_AdditionalData
        _NE.placeholderText: "附加数据"
        Binding {
            it_AdditionalData.dataNE_: modelNE_.additionalData
            modelNE_.additionalData: it_AdditionalData.dataNE_
        }
    }

    readonly property Instantiator _NE_Instantiator: Instantiator {
        id: _NE_Instantiator
        model: modelNE_.dataModel
        delegate: QtObject {
            required property list<var> modelData
            readonly property Component _component: {
                const data = modelData[NE_MediaInput_Model.IndexData];
                const type = modelData[NE_MediaInput_Model.IndexType];
                const is_string = [NE_MediaInput_Model.NE_String, NE_MediaInput_Model.NE_Json];
                if (is_string.includes(type)) {
                    if (data.includes('\n')) {
                        return root.stringAreaDelegate;
                    } else {
                        return root.stringDelegate;
                    }
                }
                return root.generalDelegate;
            }

            property Item create_item: null
            function createObject(_parent) {
                create_item = _component.createObject(_parent, {
                    modelData,
                    modelData
                });
            }
            function destroyObject() {
                if (create_item == null)
                    return;

                create_item.visible = false;
                create_item.destroy();
            }
        }
        onObjectAdded: function (index, object) {
            object.createObject(root.contentItem);
        }
        onObjectRemoved: function (index, object) {
            object.destroyObject();
        }
    }

    readonly property Component generalDelegate: Component {
        NE_Display {
            required property list<var> modelData
            contentItem: NE_Display.LoadText {}

            outputNode: NE_Node {}
            displayData: modelData[NE_MediaInput_Model.IndexTitle]
            dataNE_: modelData[NE_MediaInput_Model.IndexData]
            typeNE_: modelData[NE_MediaInput_Model.IndexType]
        }
    }
    readonly property Component stringDelegate: Component {
        NE_TextInput {
            required property list<var> modelData

            outputNode: NE_Node {}
            _NE.placeholderText: modelData[NE_MediaInput_Model.IndexTitle]
            dataNE_: modelData[NE_MediaInput_Model.IndexData]
            typeNE_: modelData[NE_MediaInput_Model.IndexType]
        }
    }
    readonly property Component stringAreaDelegate: Component {
        NE_TextInputArea {
            required property list<var> modelData

            outputNode: NE_Node {}
            _NE.placeholderText: modelData[NE_MediaInput_Model.IndexTitle]
            dataNE_: modelData[NE_MediaInput_Model.IndexData]
            typeNE_: modelData[NE_MediaInput_Model.IndexType]
        }
    }
}
