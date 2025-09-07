#ifndef MEDIAFORMAT_H
#define MEDIAFORMAT_H

#include <MutexContainer.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

namespace Media {

enum ReturnType {
    NoneError   = 0,
    UnknownError = -1,
    TaskStopped  = -2,  // 任务停止
    TaskWaiting  = -3   // 任务等待
};

static const char MetaData_NULL[] = "";
static const char MetaData_Define[] = "Define";
static const char MetaData_YT_Data[] = "YT_Data";

static const char MetaData_Title[] = "Title";
static const char MetaData_Album[] = "Album";
static const char MetaData_Artist[] = "Artist";
static const char MetaData_Lyrics[] = "Lyrics";

static const uint8_t DataPointerCount = AV_NUM_DATA_POINTERS;
}

class AudioFormat
{
public:
    AVCodecID codec_id = AVCodecID::AV_CODEC_ID_NONE;
    int samples = 0;
    int sample_rate = 0;
    AVChannelLayout ch_layout;
    AVSampleFormat format = AVSampleFormat::AV_SAMPLE_FMT_NONE;

    AVCodecParameters *operator=(AVCodecParameters *par)
    {
        codec_id = par->codec_id;
        sample_rate = par->sample_rate;
        ch_layout = par->ch_layout;
        format = (AVSampleFormat)par->format;
        return par;
    }

    friend std::ostream &operator<<(std::ostream &out, const AudioFormat &obj)
    {
        out << "id: " << avcodec_get_name(obj.codec_id)
        << " samples: " << obj.samples
        << " sample_rate: " << obj.sample_rate
        << " channels: " << obj.ch_layout.nb_channels
        << " format: " << av_get_sample_fmt_name(obj.format) << std::endl;
        return out;
    }
};

class VideoFormat
{
public:
    AVCodecID codec_id = AVCodecID::AV_CODEC_ID_NONE;
    int width = 0;
    int height = 0;
    int bit_rate = 0;
    AVRational framerate = {1, 0};
    AVPixelFormat format = AVPixelFormat::AV_PIX_FMT_NONE;

    AVCodecParameters *operator=(AVCodecParameters *par)
    {
        codec_id = par->codec_id;
        width = par->width;
        height = par->height;
        bit_rate = par->bit_rate;
        framerate = par->framerate;
        format = (AVPixelFormat)par->format;
        return par;
    }

    friend std::ostream &operator<<(std::ostream &out, const VideoFormat &obj)
    {
        out << "id: " << avcodec_get_name(obj.codec_id)
        << " width: " << obj.width
        << " height: " << obj.height
        << " bit_rate: " << obj.bit_rate
        << " frequent: " << obj.framerate.den << "/" << obj.framerate.num
        << " format: " << obj.format << std::endl;
        return out;
    }
};

#endif // MEDIAFORMAT_H
