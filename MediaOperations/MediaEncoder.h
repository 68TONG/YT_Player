#ifndef MEDIAENCODER_H
#define MEDIAENCODER_H

#include <ThreadPool.h>
#include <AudioBuffer.h>
#include <VideoBuffer.h>
#include <MediaDecoder.h>

class MediaEncoder
{
public:
    class EncoderInfo
    {
    public:
        EncoderInfo(int index, MediaDecoder *decoder);
        ~EncoderInfo();

        enum ENCODER_INFO_TYPE{
            NONE_TYPE = 0,
            IS_ENCODE = 1 << 0,
            IMAGE_TYPE = 1 << 1,
            AUDIO_TYPE = 1 << 2,
            VIDEO_TYPE = 1 << 3,
            IS_ABORT = 1 << 4,
        };

        ENCODER_INFO_TYPE type = ENCODER_INFO_TYPE::NONE_TYPE;

        int64_t record_pts = 0;
        AVCodecContext *codec_ctx = NULL;
        MutexQueue<AVPacket *> *packet_que = NULL;

        int stream_index = -1;
        VoidBuffer *in_buffer = NULL;
        MediaDecoder *media_decoder = NULL;
    };

    MediaEncoder();
    ~MediaEncoder();

    int setMediaPath(const char *url);

    int addStream(int stream_index, MediaDecoder *decoder);
    int addStream(int stream_index, MediaDecoder *decoder, AudioFormat audio_fmt);
    int addStream(int stream_index, MediaDecoder *decoder, VideoFormat video_fmt);

    int addImageStream(const char *image_path);
    int addImageStream(int stream_index, MediaDecoder *decoder);

    int removeStream(int stream_index);
    int removeStream(AVStream *stream);

    void start();
    void stop();
    void release();
    std::set<MediaDecoder *> getMediaDecoderList();

    void encodecRun();

    double progress = 0;
    int64_t duration = 0, max_pts = 0;
    AVFormatContext *format_ctx = NULL;
private:
    int setMediaHeader();
    int setMediaTrailer();
    AVCodecContext *openEncoder(AVStream *stream, AVDictionary *opts = NULL);

    int writePacket(int stream_index);
    int encodecAudioPacket(int stream_index);
    int encodecVideoPacket(int stream_index);

    void clearEncoderInfo(std::map<int, EncoderInfo *> &que);

    char error[256];
    bool is_abort = false;
    AVRational time_base = {1, 1000000};

    std::set<int> codec_ids;
    std::map<int, EncoderInfo *> encoder_info_que;
};

#endif // MEDIAENCODER_H
