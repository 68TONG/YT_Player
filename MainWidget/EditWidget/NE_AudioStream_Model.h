#ifndef NE_AUDIOSTREAM_MODEL_H
#define NE_AUDIOSTREAM_MODEL_H

#include <YT_Info.h>
#include <MediaDecoder.h>
#include <NE_Data.h>
#include <NE_MediaData_Model.h>

class NE_AudioStream_Model : public NE_MediaData_Model
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit NE_AudioStream_Model(QObject *parent = nullptr);

protected:
    AudioBuffer *inputStream = nullptr;
    void setStream(AudioBuffer *stream);

    Q_INVOKABLE void resetStream();
    Q_INVOKABLE void updateStreamModel();

    virtual bool inputDataFunc(NE_Data *input_data, NE_Data *output_data) override;
    virtual bool disinputDataFunc(NE_Data *input_data, NE_Data *output_data) override;

    Q_PROPERTY(bool isStreamValid MEMBER isStreamValid NOTIFY isStreamValidChanged)
    bool isStreamValid = inputStream != nullptr;
    Q_SIGNAL void isStreamValidChanged();

    Q_PROPERTY(int codec_id MEMBER codec_id NOTIFY codec_idChanged)
    int codec_id = AVCodecID::AV_CODEC_ID_NONE;
    Q_SIGNAL void codec_idChanged();
    Q_INVOKABLE QString getDisplay_codec_id(int data);

    Q_PROPERTY(QList<int> codec_id_model MEMBER codec_id_model NOTIFY codec_id_modelChanged)
    QList<int> codec_id_model = {AVCodecID::AV_CODEC_ID_FLAC, AVCodecID::AV_CODEC_ID_MP3};
    Q_SIGNAL void codec_id_modelChanged();

    Q_PROPERTY(int sample_rate MEMBER sample_rate NOTIFY sample_rateChanged)
    int sample_rate = 0;
    Q_SIGNAL void sample_rateChanged();
    Q_INVOKABLE QString getDisplay_sample_rate(int data);

    Q_PROPERTY(QList<int> sample_rate_model MEMBER sample_rate_model NOTIFY sample_rate_modelChanged)
    QList<int> sample_rate_model = {};
    Q_SIGNAL void sample_rate_modelChanged();

    Q_PROPERTY(int format MEMBER format NOTIFY formatChanged)
    int format = AVSampleFormat::AV_SAMPLE_FMT_NONE;
    Q_SIGNAL void formatChanged();
    Q_INVOKABLE QString getDisplay_format(int data);

    Q_PROPERTY(QList<int> format_model MEMBER format_model NOTIFY format_modelChanged)
    QList<int> format_model = {};
    Q_SIGNAL void format_modelChanged();
};

#endif // NE_AUDIOSTREAM_MODEL_H
