#include "AudioBuffer.h"

AudioBuffer::~AudioBuffer()
{
    AudioBuffer::resetThis();
}

int AudioBuffer::setMediaPath(const char *url)
{
    int sic = VoidBuffer::setMediaPath(url);
    if(sic < 0) return sic;

    sic = setMediaDecoder(media_decoder);
    if(sic < 0) return sic;

    sic = media_decoder->setStreams_ID({stream_index});
    if(sic == false) return Media::UnknownError;

    sic = media_decoder->initDecoders();
    if(sic == false) return Media::UnknownError;

    return Media::NoneError;
}

int AudioBuffer::setMediaDecoder(MediaDecoder *decoder, int index)
{
    AVStream *stream = NULL;
    AVFormatContext *format = decoder->getFormat_ctx();

    if (index < 0 || index >= (int)format->nb_streams) {
        index = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
        if(index < 0){
            char error[256];
            av_strerror(index, error, sizeof(error));
            std::cout << "AudioBuffer setMediaDecoder av_find_best_stream " << error << std::endl;
            return Media::UnknownError;
        }
    }

    stream = format->streams[index];
    if (stream->codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
        std::cout << "AudioBuffer setMediaDecoder codec_type != AVMEDIA_TYPE_AUDIO" << std::endl;
        return Media::UnknownError;
    }

    resetThis();
    setOutInfo(stream->codecpar);
    setDuration(0, stream->duration);

    name = format->url;
    media_decoder = decoder;
    stream_index = stream->index;
    buffer_que = decoder->getFrame_que(stream->index);

    return stream_index;
}

void AudioBuffer::setOutInfo(AudioFormat &format)
{
    if (format.format != out_format.format || format.ch_layout.nb_channels != out_format.ch_layout.nb_channels) {
        clearBuffer_data();
    }
    if (swr_ctx) {
        swr_free(&swr_ctx);
        swr_ctx = NULL;
    }
    in_format = out_format = format;
}

void AudioBuffer::setOutInfo(AVCodecParameters *info)
{
    if (info->format != out_format.format || info->ch_layout.nb_channels != out_format.ch_layout.nb_channels) {
        clearBuffer_data();
    }
    if (swr_ctx) {
        swr_free(&swr_ctx);
        swr_ctx = NULL;
    }
    in_format = out_format = info;
}

void AudioBuffer::resetThis()
{
    clearBuffer_data();
    VoidBuffer::resetThis();

    in_format = out_format = AudioFormat();
    if (swr_ctx) {
        swr_free(&swr_ctx);
        swr_ctx = NULL;
    }
}

void AudioBuffer::clearBuffer_data()
{
    clearBuffer_data(buffer_data, buffer_line_size);

    is_buffer_data = false;
    buffer_index = buffer_size = buffer_max_size = 0;
}

void AudioBuffer::clearBuffer_data(uint8_t **data, int *line_size)
{
    for(int i = 0;i < Media::DataPointerCount;i++){
        if(data[i] != NULL){
            av_freep(&data[i]);
            data[i] = NULL;
        }
        line_size[i] = 0;
    }
}

int AudioBuffer::updateBuffer()
{
    if (buffer_index < buffer_size) return buffer_size - buffer_index;
    is_buffer_data = false;

    AVFrame *frame = NULL;
    if (buffer_que == NULL) {
        std::cout << name << " updateBuffer buffer_que = NULL" << std::endl;
        return Media::UnknownError;
    }

    int sic = buffer_que->pop(frame, 10);
    if (sic < 0) {
        if(buffer_que->isAbort() == true && buffer_que->size() == 0){
            std::cout << name << " updateBuffer buffer_que.abort = true & buffer_que.size = "  << buffer_que->size() << std::endl;
            return Media::TaskStopped;
        }
        return Media::TaskWaiting;
    }

    if(frame->sample_rate != in_format.sample_rate ||
        frame->format != in_format.format ||
        frame->ch_layout.nb_channels != in_format.ch_layout.nb_channels) {

        if (swr_ctx) swr_free(&swr_ctx);
        // swr_ctx = swr_alloc_set_opts(NULL, out_type.channel_layout, out_type.format, out_type.frequent,
        //                              frame->channel_layout, (AVSampleFormat)frame->format, frame->sample_rate, 0, NULL);

        int sic = swr_alloc_set_opts2(&swr_ctx,
                                      &out_format.ch_layout, out_format.format, out_format.sample_rate,
                                      &in_format.ch_layout,  in_format.format,  in_format.sample_rate,
                                      0, nullptr);
        if (sic < 0 || swr_init(swr_ctx) < 0) {
            std::cout << name << " updateBuffer swr_alloc_set_opts2 failed" << std::endl;
            swr_free(&swr_ctx);
            swr_ctx = NULL;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        in_format.samples = frame->nb_samples;
        in_format.sample_rate = frame->sample_rate;
        in_format.ch_layout = frame->ch_layout;
        in_format.format = (AVSampleFormat)frame->format;
    }

    if(swr_ctx != NULL){
        int64_t samples_count = (int64_t)frame->nb_samples * out_format.sample_rate / frame->sample_rate + 256;
        int64_t samples_size = av_samples_get_buffer_size(NULL, out_format.ch_layout.nb_channels, samples_count, out_format.format, 0);
        if(samples_size < 0){
            std::cout << name << " updateBuffer av_samples_get_buffer_size failed" << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        if(buffer_max_size < samples_size){
            clearBuffer_data();
            buffer_max_size = av_samples_alloc(buffer_data, buffer_line_size, out_format.ch_layout.nb_channels, samples_count, out_format.format, 0);
            if(buffer_max_size < 0){
                std::cout << name << " updateBuffer av_samples_alloc failed" << std::endl;
                av_frame_free(&frame);
                return Media::UnknownError;
            }
        }

        samples_count = swr_convert(swr_ctx, buffer_data, samples_count, (const uint8_t **)frame->data, frame->nb_samples);
        if(samples_count < 0){
            std::cout << name << " updateBuffer swr_convert failed" << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        buffer_size = av_samples_get_buffer_size(NULL, out_format.ch_layout.nb_channels, samples_count, out_format.format, 1);
    }
    else {
        buffer_size = av_samples_get_buffer_size(NULL, frame->ch_layout.nb_channels, frame->nb_samples, (AVSampleFormat)frame->format, 1);
        if(buffer_size < 0){
            std::cout << name << " not swr updateBuffer av_samples_get_buffer_size failed" << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        if(buffer_max_size < buffer_size){
            clearBuffer_data();
            buffer_max_size = av_samples_alloc(buffer_data, buffer_line_size, frame->ch_layout.nb_channels, frame->nb_samples, (AVSampleFormat)frame->format, 0);
            if(buffer_max_size < 0){
                std::cout << name << " not swr updateBuffer av_samples_alloc failed" << std::endl;
                av_frame_free(&frame);
                return Media::UnknownError;
            }
        }

        sic = av_samples_copy(buffer_data, frame->data, 0, 0, frame->nb_samples, frame->ch_layout.nb_channels, (AVSampleFormat)frame->format);
        if(sic < 0){
            std::cout << name << " not swr updateBuffer av_samples_copy failed" << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }
    }

    buffer_index = 0;
    is_buffer_data = true;
    av_frame_free(&frame);
    return buffer_size - buffer_index;
}

int AudioBuffer::getBuffer(int buf_size, uint8_t **buf_data)
{
    if(is_buffer_data == false) return Media::UnknownError;
    if(buf_size % out_format.ch_layout.nb_channels != 0) return Media::UnknownError;
    if((buffer_size - buffer_index) < buf_size) buf_size = (buffer_size - buffer_index);

    int buf_index = buffer_index;
    if(av_sample_fmt_is_planar(out_format.format)){
        buf_index = buffer_index / out_format.ch_layout.nb_channels;
    }

    for(int i = 0;i < Media::DataPointerCount;i++){
        buf_data[i] = buffer_data[i];
        if(buf_data[i] == NULL) continue;

        buf_data[i] = (buf_data[i] + buf_index);
    }
    buffer_index += buf_size;
    return buf_size;
}

int AudioBuffer::fillFrame(AVFrame *frame)
{
    int index = 0;
    int out_size = av_samples_get_buffer_size(NULL, frame->ch_layout.nb_channels, frame->nb_samples, (AVSampleFormat)frame->format, 0);
    if(out_size < 0){
        std::cout << name << " fillFrame av_samples_get_buffer_size buf_size = " << out_size << std::endl;
        return Media::UnknownError;
    }

    while (index < out_size) {
        int in_size = updateBuffer();
        if (in_size == Media::TaskWaiting) {
            continue;
        } else if(in_size == Media::TaskStopped) {
            return in_size;
        }
        if(in_size > (out_size - index)){
            in_size = out_size - index;
        }

        uint8_t *bufs[Media::DataPointerCount] = {NULL};
        in_size = getBuffer(in_size, bufs);
        if(in_size < 0){
            std::cout << name << " fillFrame getBuffer bufs = NULL" << std::endl;
            return Media::UnknownError;
        }

        int out_buf_index = index;
        int fill_size = in_size;
        if(av_sample_fmt_is_planar(out_format.format)){
            out_buf_index = index / out_format.ch_layout.nb_channels;
            fill_size = in_size / out_format.ch_layout.nb_channels;
        }
        for(int i = 0;i < Media::DataPointerCount;i++){
            uint8_t *in_buf = bufs[i], *out_buf = (frame->data[i] + out_buf_index);
            if(in_buf == NULL || out_buf == NULL){
                break;
            }
            memcpy(out_buf, in_buf, fill_size);
        }

        index += in_size;
    }
    return Media::UnknownError;
}

