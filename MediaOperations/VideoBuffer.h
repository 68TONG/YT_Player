#ifndef VIDEOBUFFER_H
#define VIDEOBUFFER_H

#include <VoidBuffer.h>
extern "C"
{
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class VideoBuffer : public VoidBuffer
{
public:
    explicit VideoBuffer(const char* url, bool is_image = false);
    explicit VideoBuffer(MediaDecoder* decoder, int index = -1, bool is_image = false);
    virtual ~VideoBuffer();

    void initFormat();
    VideoFormat inFormat();
    VideoFormat& outFormat();
    void setOutFormat(VideoFormat& format);
    void setOutFormat(AVCodecParameters* info);

    int getBuffer(uint8_t** buf_data);
protected:
    int setMediaDecoder(MediaDecoder* decoder, int index = -1, bool is_image = false);
    int initConvertData();

    int allocBuffer(int pix_fmt, int width, int height, int align);
    void freeBuffer();

    int writeBuffer(AVFrame** frame_ptr);
    int writeConvertBuffer(AVFrame** frame_ptr);
protected:
    SwsContext* sws_ctx = nullptr;
    VideoFormat in_format, out_format;
};

#endif // VIDEOBUFFER_H
