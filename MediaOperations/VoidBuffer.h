#ifndef VOIDBUFFER_H
#define VOIDBUFFER_H

#include <MediaDecoder.h>

class VoidBuffer
{
public:
    virtual ~VoidBuffer();

    virtual int setMediaPath(const char *url);
    virtual int setMediaDecoder(MediaDecoder *decoder, int index = -1) = 0;
    virtual void setOutInfo(AVCodecParameters *info) = 0;

    virtual void resetThis();
    void setDuration(int64_t begin, int64_t end);

    std::string name = "";
    int stream_index = -1;
    MediaDecoder *media_decoder = NULL;

    int64_t begin_pts = -1, end_pts = -1;
    AVRational time_base = AV_TIME_BASE_Q;
protected:
    bool is_buffer_data = false;
    int buffer_line_size[Media::DataPointerCount] = {0};
    uint8_t *buffer_data[Media::DataPointerCount] = {NULL};
    uint32_t buffer_index = 0, buffer_size = 0, buffer_max_size = 0;
protected:
    bool is_media_decoder = false;
    MutexQueue<AVFrame *> *buffer_que = nullptr;
};

#endif // VOIDBUFFER_H
