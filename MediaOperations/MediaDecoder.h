#ifndef MEDIADECODER_H
#define MEDIADECODER_H

#include <MediaFormat.h>
#include <MutexContainer.h>

class MediaDecoder
{
public:
    MediaDecoder();
    virtual ~MediaDecoder();

    int initMediaPath(const char * url);
    bool addStreams_ID(std::set<int> list_id, std::set<int> *list_is = nullptr);
    bool setStreams_ID(std::set<int> list_id, std::set<int> *list_is = nullptr);
    bool setStreams_Type(std::set<AVMediaType> list_type, std::map<int, int> *list_is = nullptr);

    void clearDecoders();
    bool initDecoders(std::set<int> *list_is = nullptr);
    bool initDecoders(std::set<int> list_id, std::set<int> *list_is = nullptr);

    AVStream *getStream(int index);
    AVFormatContext *getFormat_ctx();
    MutexQueue<AVPacket *> *getPacket_que(int index);
    MutexQueue<AVFrame *> *getFrame_que(int index);

    void start();
    void stop();
    void seek(int64_t seek_ts);
    void release();

    void decodecRun();
    void decodecRun_TimeEnd(int64_t end_time);
    void decodecRun_Play(int wait_count = 36, int wait_stream_index = -1);
    void decodecRun_CountWait(int wait_count = 36, int wait_stream_index = -1);
protected:
    std::set<int> codec_ids;
    AVFormatContext *format_ctx = nullptr;

    std::map<int, MutexQueue<AVPacket *>> packet_ques;
    std::map<int, MutexQueue<AVFrame *>> frame_ques;
    std::map<int, AVCodecContext *> codec_ctxs;
private:
    AVCodecContext *openDecoder(int id);
    int readPacket(AVPacket **packet_ptr);
    int decodecPacket(AVPacket **packet_ptr);

    template<typename T>
    void abortQues(std::map<int, MutexQueue<T>> &ques, bool is_abort);
    void clearPacketQue(MutexQueue<AVPacket *> &que);
    void clearFrameQue (MutexQueue<AVFrame *> &que);

    char error[256];
    bool is_seek = false;
    bool is_valid = false;
    bool is_abort = true;
};

#endif // MEDIADECODER_H
