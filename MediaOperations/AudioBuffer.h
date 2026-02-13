#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <VoidBuffer.h>
extern "C"
{
#include <libswresample/swresample.h>
}

class AudioBuffer : public VoidBuffer
{
public:
    explicit AudioBuffer(const char* url);
    explicit AudioBuffer(MediaDecoder* decoder, int index = -1);
    virtual ~AudioBuffer();

    AudioFormat inFormat();
    AudioFormat& outFormat();
    void setOutFormat(AudioFormat& format);
    void setOutFormat(AVCodecParameters* info);

    int getBuffer(int get_samples, uint8_t** buf_data);
    int fillFrame(AVFrame* frame);
private:
    int setMediaDecoder(MediaDecoder* decoder, int index = -1);
    int initConvertData();

    int allocBuffer(int* linesize, int nb_channels, int nb_samples, int sample_fmt, int align);
    void freeBuffer();

    int writeBuffer(AVFrame** frame_ptr);
    int writeConvertBuffer(AVFrame** frame_ptr);

    int sample_index = 0;
    int sample_count = 0;

private:
    SwrContext* swr_ctx = nullptr;
    AudioFormat in_format, out_format;
};
#endif // AUDIOBUFFER_H
