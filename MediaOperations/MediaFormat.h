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

    template <typename... Args>
    void coutError_(int err, std::string str, Args&&... args) {
        char buf[AV_ERROR_MAX_STRING_SIZE] = { 0 };
        av_strerror(err, buf, sizeof(buf));

        // 用 ostringstream 拼接待打印的通用信息
        std::ostringstream oss;
        ((oss << std::forward<Args>(args)), ...);   // C++17 折叠表达式
        std::cout << str << oss.str() << buf << std::endl;
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
#include <set>
#include <string>

    /**
     * 检查指定封装格式和编码器是否支持附加封面图片流
     *
     * @param format_name 封装格式名称 (如 "mp4", "flac", "mp3" 等)
     * @param codec_id    编码器ID (如 AV_CODEC_ID_MJPEG, AV_CODEC_ID_PNG 等)
     * @return 支持封面流返回 0, 不支持返回 -1
     */
    inline int checkAttachedPicSupport(const char* format_name, enum AVCodecID codec_id)
    {
        if (!format_name) {
            return -1;
        }

        /* 支持的图片编码器白名单 */
        int is_image_codec = 0;
        switch (codec_id) {
        case AV_CODEC_ID_MJPEG:
        case AV_CODEC_ID_PNG:
        case AV_CODEC_ID_BMP:
        case AV_CODEC_ID_GIF:
        case AV_CODEC_ID_TIFF:
        case AV_CODEC_ID_WEBP:
            is_image_codec = 1;
            break;
        default:
            break;
        }

        if (!is_image_codec) {
            return Media::UnknownError;  /* 非图片编码器，不能作为封面 */
        }

        /* 支持封面嵌入的格式集合 */
        static const std::set<std::string> supported_formats = {
            /* 音频格式（最常用封面嵌入场景） */
            "flac", "ogg", "opus",
            "mp3", "mp2", "mp1",
            "aac", "m4a", "m4b", "m4p", "m4r",
            "mp4", "mov", "3gp", "3g2",
            "wma", "asf", "wmv",
            "aiff", "aif", "aifc",
            "wav", "wave",
            "wv", "ape",
            "mpc", "mp+", "mpp",
            "dsf", "dff",
            "tak", "ofr", "ofs",
            "spx", "tta",
            "ac3", "eac3", "dts",
            "thd", "mlp", "dtsma", "dsd",
            /* Matroska 系列（通用容器，支持附件） */
            "matroska", "mkv", "mka", "mk3d", "webm",
            /* 其他 */
            "dsdiff", "sacd",
            "caf", "sox", "ircam", "w64",
            "nist", "sds", "avr", "voc",
            "paf", "fap", "gsm",
            "xwma", "oma",
            "aa", "aax"
        };

        /* 不支持封面嵌入的格式集合（显式排除） */
        static const std::set<std::string> unsupported_formats = {
            "raw", "data", "pcm",
            "s16le", "s16be", "s24le", "s24be", "s32le", "s32be",
            "u8", "f32le", "f32be", "f64le", "f64be",
            "alaw", "mulaw",
            "adx", "adx2",
            "bfstm", "brstm", "bcstm", "bwav", "bfwav", "vgmstream",
            "minipsf", "psf", "psf2", "ssf", "qsf", "gsf", "usf",
            "2sf", "ncsf", "snsf",
            "s3m", "xm", "it", "mod", "mtm", "669",
            "med", "okt", "far", "ult", "stm",
            "midi", "mid", "rmi", "kar", "smf",
            "m3u", "m3u8", "pls", "cue"
        };

        std::string fmt_lower = format_name;
        for (auto& c : fmt_lower) {
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        }

        if (supported_formats.count(fmt_lower)) {
            return Media::NoneError;
        }

        if (unsupported_formats.count(fmt_lower)) {
            return Media::UnknownError;
        }
        return Media::UnknownError;
    }

    // inline int checkAttachedPicSupport(const char* format_name, enum AVCodecID codec_id) {
    //     auto fmt = av_guess_format(format_name, nullptr, nullptr);
    //     if (fmt == nullptr) {
    //         return Media::UnknownError;
    //     }

    //     int ret = avformat_query_codec(fmt, codec_id, FF_COMPLIANCE_NORMAL);
    //     if (ret == MKTAG('A', 'P', 'I', 'C')) {
    //         return 1;  // 支持 attached_pic
    //     }
    //     else if (ret == 1) {
    //         return 0;  // 支持普通流，但不一定是 attached_pic
    //     }
    //     else if (ret == 0) {
    //         return -1;
    //     }
    //     return Media::UnknownError;
    // }
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
