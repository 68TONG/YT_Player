#ifndef VOIDBUFFER_H
#define VOIDBUFFER_H

#include <MediaFormat.h>
#include <MutexContainer.h>

class MediaDecoder;
class VoidBuffer
{
protected:
    explicit VoidBuffer(const char* url);
    explicit VoidBuffer(MediaDecoder* decoder);
public:
    virtual ~VoidBuffer();
    bool isValid() const;
    MediaDecoder* mediaDecoder() const;
    void setDuration(int64_t begin, int64_t end);

    std::string name = "";
    int stream_index = -1;

    int64_t begin_pts = -1, end_pts = -1;
    AVRational time_base = AV_TIME_BASE_Q;
protected:
    bool is_valid = true;
    int buffer_line_size[Media::DataPointerCount] = { 0 };
    uint8_t* buffer_data[Media::DataPointerCount] = { nullptr };
    int buffer_size = 0;
    int buffer_max_size = 0;
protected:
    const bool is_media_decoder;
    MediaDecoder* const media_decoder;
    MutexQueue<AVFrame*>* buffer_que = nullptr;
};

#endif // VOIDBUFFER_H
