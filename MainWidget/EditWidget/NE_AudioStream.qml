import QtQuick

import YT_Player

NE_Group {
    id: root

    readonly property NE_AudioStream_Model modelNE_: NE_AudioStream_Model {
        id: modelNE_
        codec_id: input_codec_id._NE.currentValue != null ? input_codec_id._NE.currentValue : codec_id
        onCodec_idChanged: {
            input_codec_id._NE.currentIndex = codec_id_model.indexOf(codec_id);
        }
        sample_rate: input_sample_rate._NE.currentValue != null ? input_sample_rate._NE.currentValue : sample_rate
        onSample_rateChanged: {
            input_sample_rate._NE.currentIndex = sample_rate_model.indexOf(sample_rate);
        }
        format: input_format._NE.currentValue != null ? input_format._NE.currentValue : format
        onFormatChanged: {
            input_format._NE.currentIndex = format_model.indexOf(format);
        }
    }

    NE_Display {
        id: inputStream
        inputNode: NE_Node {}
        outputNode: NE_Node {}
        contentItem: YT_Button {
            enabled: modelNE_.isStreamValid
            hoverEnabled: modelNE_.isStreamValid

            anchors.fill: parent
            modelData: parent.displayData
            contentItem: YT_Button.LoadText {}
            background: Rectangle {
                radius: YT_Info.RadiusSmall
                color: parent.hovered ? YT_Info.ItemFocusColor : YT_Info.ItemColor
            }
            onClicked: modelNE_.resetStream()
        }

        typeNE_: NE_MediaData_Model.NE_AudioStream
        dataModelNE_: modelNE_
        displayData: contentItem.hovered ? "AudioStream Reset" : "AudioStream"
    }

    NE_ComboBox {
        id: input_codec_id
        enabled: modelNE_.isStreamValid
        _NE.hoverEnabled: modelNE_.isStreamValid
        _NE.displayText: enabled ? modelNE_.getDisplay_codec_id(_NE.currentValue) : "编码格式"
        _NE.model: modelNE_.codec_id_model
        _NE.delegate: YT_ComboBox.YT_Delegate {
            required property var modelData
            implicitWidth: parent.width
            text: modelNE_.getDisplay_codec_id(modelData)
        }
    }

    NE_ComboBox {
        id: input_sample_rate
        enabled: modelNE_.isStreamValid
        _NE.hoverEnabled: modelNE_.isStreamValid
        _NE.displayText: enabled ? modelNE_.getDisplay_sample_rate(_NE.currentValue) : "采样率"
        _NE.model: modelNE_.sample_rate_model
        _NE.delegate: YT_ComboBox.YT_Delegate {
            required property var modelData
            implicitWidth: parent.width
            text: modelNE_.getDisplay_sample_rate(modelData)
        }
    }

    NE_ComboBox {
        id: input_format
        enabled: modelNE_.isStreamValid
        _NE.hoverEnabled: modelNE_.isStreamValid
        _NE.displayText: enabled ? modelNE_.getDisplay_format(_NE.currentValue) : "位格式"
        _NE.model: modelNE_.format_model
        _NE.delegate: YT_ComboBox.YT_Delegate {
            required property var modelData
            implicitWidth: parent.width
            text: modelNE_.getDisplay_format(modelData)
        }
    }
}
