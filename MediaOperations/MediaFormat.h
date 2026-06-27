#ifndef MEDIAFORMAT_H
#define MEDIAFORMAT_H

#include <sstream>
#include <ThreadPool.h>
#include <MutexContainer.h>

extern "C"
{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace Media
{
    enum ReturnType
    {
        NoneError = 0,
        UnknownError = -1,
        TaskStopped = -2, // 任务停止
        TaskWaiting = -3  // 任务等待
    };

    static const char MetaData_NULL[] = "";
    static const char MetaData_Define[] = "Define";
    static const char MetaData_YT_Data[] = "YT_Data";
    static const char MetaData_Title[] = "Title";
    static const char MetaData_Album[] = "Album";
    static const char MetaData_Artist[] = "Artist";
    static const char MetaData_Lyrics[] = "Lyrics";

    static const uint8_t DataPointerCount = AV_NUM_DATA_POINTERS;

    template <typename... Args>
    void coutError(int err, Args&&... args) {
        char buf[AV_ERROR_MAX_STRING_SIZE] = { 0 };
        av_strerror(err, buf, sizeof(buf));

        // 用 ostringstream 拼接待打印的通用信息
        std::ostringstream oss;
        ((oss << std::forward<Args>(args)), ...);   // C++17 折叠表达式
        std::cout << oss.str() << "  " << buf << std::endl;
    }

    static const std::set<std::string> cover_image_formats = { "mp3", "flac", "mp4", "mkv" };
    static const std::set<int> image_stream_type_list = {
        AVCodecID::AV_CODEC_ID_PNG,
        AVCodecID::AV_CODEC_ID_MJPEG };

    inline bool checkImageStream(const AVStream* stream) {
        const auto codec_par = stream->codecpar;
        const AVCodecDescriptor* codec_desc = avcodec_descriptor_get(codec_par->codec_id);
        if (codec_desc == nullptr) return false;

        if (codec_desc->type == AVMEDIA_TYPE_VIDEO) {
            // 图片流通常没有有效的帧率
            AVRational frame_rate = stream->avg_frame_rate;
            if (frame_rate.num == 0 || (frame_rate.num == 1 && frame_rate.den == 1)) {
                return true;
            }
            // 进一步检查：图片流通常只有1帧
            if (stream->nb_frames == 1 || stream->nb_frames == 0) {
                return true;
            }
        }

        // 方法2：检查编解码器属性
        if (codec_desc->props & AV_CODEC_PROP_BITMAP_SUB) {
            return true;  // 位图字幕也算图片
        }
        return false;
    }
    inline int checkAttachedPicSupport(const char* format_name, enum AVCodecID codec_id) {
        auto fmt = av_guess_format(format_name, nullptr, nullptr);
        if (fmt == nullptr) {
            return -1;
        }

        int ret = avformat_query_codec(fmt, codec_id, FF_COMPLIANCE_NORMAL);
        if (ret == MKTAG('A', 'P', 'I', 'C')) {
            return 1;  // 支持 attached_pic
        }
        else if (ret == 1) {
            return 0;  // 支持普通流，但不一定是 attached_pic
        }
        else if (ret == 0) {
            return -1;
        }
        else {
            return -1;
        }
        return -1;
    }
}

enum class MediaType
{
    None,
    Image,
    Audio,
    Video
};

enum class CodecState
{
    Invalid,
    Ready,
    Running,
    Paused,
    Stopped,
    Error
};

class AudioFormat
{
public:
    AVCodecID codec_id = AVCodecID::AV_CODEC_ID_NONE;
    // int samples = 0;
    int sample_rate = 0;
    AVChannelLayout ch_layout;
    AVSampleFormat format = AVSampleFormat::AV_SAMPLE_FMT_NONE;

    bool isConvert(const AudioFormat& data)
    {
        return (this->format != data.format ||
            this->sample_rate != data.sample_rate ||
            this->ch_layout.nb_channels != data.ch_layout.nb_channels);
    }

    AVCodecParameters* operator=(AVCodecParameters* par)
    {
        codec_id = par->codec_id;
        sample_rate = par->sample_rate;
        ch_layout = par->ch_layout;
        format = (AVSampleFormat)par->format;
        return par;
    }

    friend std::ostream& operator<<(std::ostream& out, const AudioFormat& obj)
    {
        out << "id: " << avcodec_get_name(obj.codec_id)
            // << " samples: " << obj.samples
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
    AVRational framerate = { 1, 0 };
    AVPixelFormat format = AVPixelFormat::AV_PIX_FMT_NONE;

    bool isConvert(const VideoFormat& data)
    {
        return (this->format != data.format ||
            this->width != data.width ||
            this->height != data.height);
    }

    AVCodecParameters* operator=(AVCodecParameters* par)
    {
        codec_id = par->codec_id;
        width = par->width;
        height = par->height;
        bit_rate = par->bit_rate;
        framerate = par->framerate;
        format = (AVPixelFormat)par->format;
        return par;
    }

    friend std::ostream& operator<<(std::ostream& out, const VideoFormat& obj)
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
