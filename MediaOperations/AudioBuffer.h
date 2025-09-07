#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <VoidBuffer.h>

class AudioBuffer : public VoidBuffer
{
public:
    virtual ~AudioBuffer();

    virtual int setMediaPath(const char *url);
    virtual int setMediaDecoder(MediaDecoder *decoder, int index = -1);
    virtual void setOutInfo(AudioFormat &format);
    virtual void setOutInfo(AVCodecParameters *info);

    virtual void resetThis();
    void clearBuffer_data();
    void clearBuffer_data(uint8_t **data, int *line_size);

    int updateBuffer();
    int getBuffer(int buf_size, uint8_t **buf_data);

    int fillFrame(AVFrame *frame);
public:
    SwrContext *swr_ctx = NULL;
    AudioFormat in_format, out_format;
};
#endif // AUDIOBUFFER_H
