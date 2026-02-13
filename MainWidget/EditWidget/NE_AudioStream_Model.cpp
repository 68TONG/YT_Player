#include "NE_AudioStream_Model.h"

NE_AudioStream_Model::NE_AudioStream_Model(QObject* parent)
    : NE_MediaData_Model{ parent }
{
    connect(this, &NE_AudioStream_Model::codec_idChanged, this, &NE_AudioStream_Model::updateStreamModel);
    connect(this, &NE_AudioStream_Model::codec_idChanged, this, [this]()
        {
            if (inputStream == nullptr) return;
            inputStream->outFormat().codec_id = (AVCodecID)codec_id; });
    connect(this, &NE_AudioStream_Model::sample_rateChanged, this, [this]()
        {
            if (inputStream == nullptr) return;
            inputStream->outFormat().sample_rate = sample_rate; });
    connect(this, &NE_AudioStream_Model::formatChanged, this, [this]()
        {
            if (inputStream == nullptr) return;
            inputStream->outFormat().format = (AVSampleFormat)format; });
}

void NE_AudioStream_Model::setStream(AudioBuffer* stream)
{
    inputStream = stream;
    isStreamValid = inputStream != nullptr;
    emit isStreamValidChanged();
}

void NE_AudioStream_Model::resetStream()
{
    if (inputStream == nullptr)
    {
        codec_id = AVCodecID::AV_CODEC_ID_NONE;
        sample_rate = 0;
        format = AVSampleFormat::AV_SAMPLE_FMT_NONE;
    }
    else
    {
        const auto in_format = inputStream->inFormat();
        codec_id = in_format.codec_id;
        sample_rate = in_format.sample_rate;
        format = in_format.format;
    }
    emit codec_idChanged();
    emit sample_rateChanged();
    emit formatChanged();
}

void NE_AudioStream_Model::updateStreamModel()
{
    auto codec = avcodec_find_encoder((AVCodecID)codec_id);
    if (codec == nullptr)
    {
        // qDebug() << "NE_AudioStream_Model updateStreamModel avcodec_find_encoder = nullptr";
        // codec_id_model = {AVCodecID::AV_CODEC_ID_NONE};
        // sample_rate_model = {0};
        // format_model = {AVSampleFormat::AV_SAMPLE_FMT_NONE};
        return;
    }

    codec_id_model = { AVCodecID::AV_CODEC_ID_FLAC, AVCodecID::AV_CODEC_ID_MP3 };

    auto sample_rates = codec->supported_samplerates;
    if (sample_rates == NULL)
    {
        sample_rate_model = { 44100, 48000, 96000 };
    }
    else
    {
        sample_rate_model.clear();
        while ((*sample_rates) != 0)
        {
            sample_rate_model.append(*sample_rates);
            sample_rates++;
        }
    }

    auto sample_fmts = codec->sample_fmts;
    if (sample_fmts != NULL)
    {
        format_model.clear();
        while ((*sample_fmts) != -1)
        {
            format_model.append(*sample_fmts);
            sample_fmts++;
        }
    }

    emit codec_id_modelChanged();
    emit sample_rate_modelChanged();
    emit format_modelChanged();
}

bool NE_AudioStream_Model::inputDataFunc(NE_Data* input_data, NE_Data* output_data)
{
    if (output_data->typeNE_ == NE_AudioStream && input_data->typeNE_ == NE_AudioStream)
        setStream(output_data->dataNE_.value<AudioBuffer*>());
    else
        setStream(nullptr);

    if (inputStream == nullptr)
    {
        resetStream();
        input_data->dataNE_ = input_data->dataNE_Default;
        return true;
    }

    codec_id = inputStream->outFormat().codec_id;
    sample_rate = inputStream->outFormat().sample_rate;
    format = inputStream->outFormat().format;

    emit codec_idChanged();
    emit sample_rateChanged();
    emit formatChanged();

    input_data->dataNE_ = output_data->dataNE_;
    return true;
}

bool NE_AudioStream_Model::disinputDataFunc(NE_Data* input_data, NE_Data* output_data)
{
    setStream(nullptr);
    resetStream();
    input_data->dataNE_ = input_data->dataNE_Default;

    return true;
}

QString NE_AudioStream_Model::getDisplay_codec_id(int data)
{
    return avcodec_get_name((AVCodecID)data);
}

QString NE_AudioStream_Model::getDisplay_sample_rate(int data)
{
    return QString::number(data) + "Hz";
}

QString NE_AudioStream_Model::getDisplay_format(int data)
{
    return av_get_sample_fmt_name((AVSampleFormat)data);
}
