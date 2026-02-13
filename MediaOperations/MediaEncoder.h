#ifndef MEDIAENCODER_H
#define MEDIAENCODER_H

#include <ThreadPool.h>
#include <AudioBuffer.h>
#include <VideoBuffer.h>
#include <MediaDecoder.h>

class MediaEncoder
{
public:
    class EncodeInfo
    {
    public:
        EncodeInfo(int en_index, int de_index, MediaDecoder *media_decoder);
        ~EncodeInfo();

        bool is_encode = false;
        bool is_stopped = false;
        MediaType type = MediaType::None;

        int64_t record_pts = 0;
        AVCodecContext *codec_ctx = nullptr;

        int en_stream_index = -1;
        int de_stream_index = -1;
        VoidBuffer *media_buffer = nullptr;
        MediaDecoder *media_decoder = nullptr;
    };

    MediaEncoder();
    ~MediaEncoder();

    int setMediaPath(const char *url);

    int addStream(VoidBuffer *buffer);
    int addStream(AudioBuffer *buffer);
    int addStream(VideoBuffer *buffer);
    int addStream(int stream_index, MediaDecoder *decoder);
    int addStream(int stream_index, MediaDecoder *decoder, AudioFormat audio_fmt);
    int addStream(int stream_index, MediaDecoder *decoder, VideoFormat video_fmt);

    int addCoverStream(const char *image_path);
    int addCoverStream(int stream_index, MediaDecoder *decoder);
    int addLyricsStream(const char *data);

    int removeStream(int stream_index);
    int removeStream(AVStream *stream);

    void start();
    void stop();
    void release();

    int encodeRun();

    double progress = 0;
    int64_t duration = 0, max_pts = 0;
    AVFormatContext *format_ctx = nullptr;

private:
    int setMediaHeader();
    int setMediaTrailer();
    AVStream *addStream(AVStream *stream);
    AVCodecContext *openEncoder(AVStream *stream, AVDictionary *opts = nullptr);

    int writePacket(int stream_index);
    int encodeAudioPacket(int stream_index);
    int encodeVideoPacket(int stream_index);
    int encodeImagePacket(int stream_index);

    int writePacket(AVPacket **packet_ptr);
    int writePacket(int stream_index, AVFrame **frame_ptr, AVCodecContext *codec_ctx);

    char error[256];
    AVRational time_base = {1, 1000000};
    CodecState cur_state = CodecState::Invalid;

    std::set<int> codec_ids;
    std::set<MediaDecoder *> media_decoders;
    std::map<int, EncodeInfo *> encode_infos;
};

#endif // MEDIAENCODER_H
