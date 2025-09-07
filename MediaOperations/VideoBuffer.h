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
    ~VideoBuffer();

    virtual int setMediaPath(const char *url);
    virtual int setMediaDecoder(MediaDecoder *decoder, int index = -1);
    virtual void setOutInfo(VideoFormat &format);
    virtual void setOutInfo(AVCodecParameters *info);

    virtual void resetThis();
    void clearBuffer_data();
    void clearBuffer_data(uint8_t **data, int *line_size);

    int updateBuffer();
    int updateBuffer(const VideoFormat &format);

    uint8_t **getBuffers();
public:
    SwsContext *sws_ctx = NULL;
    VideoFormat in_format, out_format;
};


#endif // VIDEOBUFFER_H
