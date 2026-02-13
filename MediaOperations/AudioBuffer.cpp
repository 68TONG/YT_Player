#include "AudioBuffer.h"

#include "MediaDecoder.h"

AudioBuffer::AudioBuffer(const char* url)
    : VoidBuffer(url)
{
    if (is_valid == false) return;
    int sic = setMediaDecoder(media_decoder);
    if (sic < 0) {
        is_valid = false;
        return;
    }

    sic = media_decoder->setStreams_ID({ stream_index });
    if (sic == false) {
        is_valid = false;
        return;
    }

    sic = media_decoder->initDecoders();
    if (sic == false) {
        is_valid = false;
        return;
    }
    is_valid = true;
}

AudioBuffer::AudioBuffer(MediaDecoder* decoder, int index)
    : VoidBuffer(decoder)
{
    int sic = setMediaDecoder(media_decoder, index);
    if (sic < 0) {
        is_valid = false;
        return;
    }
    is_valid = true;
}

AudioBuffer::~AudioBuffer()
{
    freeBuffer();
    in_format = out_format = AudioFormat();
    if (swr_ctx)
    {
        swr_free(&swr_ctx);
    }
}

AudioFormat AudioBuffer::inFormat()
{
    return in_format;
}

AudioFormat& AudioBuffer::outFormat()
{
    return out_format;
}

void AudioBuffer::setOutFormat(AudioFormat& format)
{
    out_format = format;
    if (in_format.isConvert(out_format) == false)
        return;

    freeBuffer();
    initConvertData();
}

void AudioBuffer::setOutFormat(AVCodecParameters* info)
{
    AudioFormat data;
    data = info;
    setOutFormat(data);
}

int AudioBuffer::getBuffer(int get_samples, uint8_t** buf_data)
{
    if (sample_index >= sample_count)
    {
        AVFrame* frame = nullptr;
        int sic = buffer_que->pop(frame, 10);
        if (sic < 0)
        {
            if (media_decoder->state() == CodecState::Stopped && buffer_que->size() == 0)
            {
                std::cout << name << " AudioBuffer getBuffer Media::TaskStopped" << std::endl;
                return Media::TaskStopped;
            }
            return Media::TaskWaiting;
        }

        sample_count = swr_ctx == nullptr ? writeBuffer(&frame) : writeConvertBuffer(&frame);
        if (sample_count < 0)
            return sample_count;
        sample_index = 0;
    }

    if ((sample_count - sample_index) < get_samples)
    {
        get_samples = (sample_count - sample_index);
    }

    int buf_index = sample_index * av_get_bytes_per_sample(out_format.format);
    if (av_sample_fmt_is_planar(out_format.format) == false)
    {
        buf_index = buf_index * out_format.ch_layout.nb_channels;
    }

    for (int i = 0; i < Media::DataPointerCount; i++)
    {
        buf_data[i] = buffer_data[i];
        if (buf_data[i] == nullptr)
            continue;

        buf_data[i] = (buf_data[i] + buf_index);
    }

    sample_index += get_samples;
    return get_samples;
}

int AudioBuffer::fillFrame(AVFrame* frame)
{
    int index = 0;
    while (index < frame->nb_samples)
    {
        uint8_t* bufs[Media::DataPointerCount] = { nullptr };
        int in_samples = getBuffer(frame->nb_samples - index, bufs);
        if (in_samples == Media::TaskWaiting)
            continue;
        else if (in_samples == Media::TaskStopped)
            return in_samples;

        int sic = av_samples_copy(frame->data, bufs, index, 0, in_samples, frame->ch_layout.nb_channels, (AVSampleFormat)frame->format);
        if (sic < 0)
        {
            std::cout << name << " fillFrame av_samples_copy failed" << std::endl;
            return Media::UnknownError;
        }

        index += in_samples;
    }
    return Media::NoneError;
}

int AudioBuffer::setMediaDecoder(MediaDecoder* decoder, int index)
{
    AVStream* stream = nullptr;
    AVFormatContext* format = decoder->getFormat_ctx();

    if (index < 0 || index >= (int)format->nb_streams)
    {
        index = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
        if (index < 0)
        {
            char error[256];
            av_strerror(index, error, sizeof(error));
            std::cout << "AudioBuffer setMediaDecoder av_find_best_stream " << error << std::endl;
            return Media::UnknownError;
        }
    }

    stream = format->streams[index];
    if (stream->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
    {
        std::cout << "AudioBuffer setMediaDecoder codec_type != AVMEDIA_TYPE_AUDIO" << std::endl;
        return Media::UnknownError;
    }

    in_format = out_format = stream->codecpar;
    setDuration(0, stream->duration);

    name = format->url;
    stream_index = stream->index;
    buffer_que = decoder->getFrame_que(stream->index);

    return stream_index;
}

int AudioBuffer::initConvertData()
{
    if (swr_ctx)
    {
        swr_free(&swr_ctx);
        swr_ctx = nullptr;
    }
    int sic = swr_alloc_set_opts2(&swr_ctx,
        &out_format.ch_layout, out_format.format, out_format.sample_rate,
        &in_format.ch_layout, in_format.format, in_format.sample_rate,
        0, nullptr);
    if (sic < 0 || swr_init(swr_ctx) < 0)
    {
        std::cout << name << " initConvertData swr_alloc_set_opts2 failed" << std::endl;
        swr_free(&swr_ctx);
        return Media::UnknownError;
    }
    return Media::NoneError;
}

int AudioBuffer::allocBuffer(int* linesize, int nb_channels, int nb_samples, int sample_fmt, int align)
{
    buffer_size = av_samples_get_buffer_size(linesize, nb_channels, nb_samples, (AVSampleFormat)sample_fmt, align);
    if (buffer_size < 0)
    {
        return Media::UnknownError;
    }

    if (buffer_max_size < buffer_size)
    {
        freeBuffer();
        buffer_max_size = av_samples_alloc(buffer_data, linesize, nb_channels, nb_samples, (AVSampleFormat)sample_fmt, align);
        if (buffer_max_size < 0)
        {
            return Media::UnknownError;
        }
    }
    return Media::NoneError;
}

void AudioBuffer::freeBuffer()
{
    // av_freep(*buffer_data);
    av_freep(&buffer_data[0]);
    for (int i = 0; i < Media::DataPointerCount; i++)
    {
        buffer_line_size[i] = 0;
    }
    buffer_size = buffer_max_size = 0;
    sample_index = sample_count = 0;
}

int AudioBuffer::writeBuffer(AVFrame** frame_ptr)
{
    auto frame = *frame_ptr;
    int sic = allocBuffer(buffer_line_size, frame->ch_layout.nb_channels, frame->nb_samples, frame->format, 0);
    if (sic < 0)
    {
        std::cout << name << " AudioBuffer writeBuffer allocBuffer failed" << std::endl;
        av_frame_free(frame_ptr);
        return Media::UnknownError;
    }

    sic = av_samples_copy(buffer_data, frame->data, 0, 0, frame->nb_samples, frame->ch_layout.nb_channels, (AVSampleFormat)frame->format);
    if (sic < 0)
    {
        std::cout << name << " AudioBuffer writeBuffer av_samples_copy failed" << std::endl;
        av_frame_free(frame_ptr);
        return Media::UnknownError;
    }

    av_frame_free(frame_ptr);
    return frame->nb_samples;
}

int AudioBuffer::writeConvertBuffer(AVFrame** frame_ptr)
{
    auto frame = *frame_ptr;
    int64_t nb_samples = (int64_t)frame->nb_samples * out_format.sample_rate / frame->sample_rate + 256;
    int sic = allocBuffer(buffer_line_size, out_format.ch_layout.nb_channels, nb_samples, out_format.format, 0);
    if (sic < 0)
    {
        std::cout << name << " AudioBuffer writeConvertBuffer av_samples_get_buffer_size failed" << std::endl;
        av_frame_free(frame_ptr);
        return Media::UnknownError;
    }

    nb_samples = swr_convert(swr_ctx, buffer_data, nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
    if (nb_samples < 0)
    {
        std::cout << name << " AudioBuffer writeConvertBuffer swr_convert failed" << std::endl;
        av_frame_free(frame_ptr);
        return Media::UnknownError;
    }

    av_frame_free(frame_ptr);
    return nb_samples;
}
