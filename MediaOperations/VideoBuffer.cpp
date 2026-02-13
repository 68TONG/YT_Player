#include "VideoBuffer.h"

#include "MediaDecoder.h"

VideoBuffer::VideoBuffer(const char* url)
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

VideoBuffer::VideoBuffer(MediaDecoder* decoder, int index)
    : VoidBuffer(decoder)
{
    if (is_valid == false) return;
    int sic = setMediaDecoder(media_decoder, index);
    if (sic < 0) {
        is_valid = false;
        return;
    }
    is_valid = true;
}

VideoBuffer::~VideoBuffer()
{
    freeBuffer();
    in_format = out_format = VideoFormat();
    if (sws_ctx)
    {
        sws_freeContext(sws_ctx);
        sws_ctx = nullptr;
    }
}

void VideoBuffer::initFormat()
{
    if (in_format.isConvert(out_format) == false)
        return;

    freeBuffer();
    initConvertData();
}

VideoFormat VideoBuffer::inFormat()
{
    return in_format;
}

VideoFormat& VideoBuffer::outFormat()
{
    return out_format;
}

void VideoBuffer::setOutFormat(VideoFormat& format)
{
    out_format = format;
    if (in_format.isConvert(out_format) == false)
        return;

    freeBuffer();
    initConvertData();
}

void VideoBuffer::setOutFormat(AVCodecParameters* info)
{
    VideoFormat format;
    format = info;
    setOutFormat(format);
}

// int VideoBuffer::updateBuffer(const VideoFormat& format)
// {
//     // is_buffer_data = false;
//     if (format.width == out_format.width && format.height == out_format.height && format.format == out_format.format)
//     {
//         // is_buffer_data = true;
//         return Media::UnknownError;
//     }

//     SwsContext* sws_ctx = sws_getContext(out_format.width, out_format.height, out_format.format,
//         format.width, format.height, format.format,
//         SWS_FAST_BILINEAR, NULL, NULL, NULL);

//     if (sws_ctx == NULL)
//     {
//         std::cout << name << " updateBuffer sws_getContext failed" << std::endl;
//         return Media::UnknownError;
//     }

//     int image_line_size[Media::DataPointerCount] = { 0 };
//     uint8_t* image_buffer[Media::DataPointerCount] = { NULL };
//     int64_t image_size = av_image_alloc(image_buffer, image_line_size, out_format.width, out_format.height, out_format.format, 1);
//     if (image_size < 0)
//     {
//         std::cout << name << " updateBuffer av_image_alloc failed " << image_size << std::endl;
//         return Media::UnknownError;
//     }

//     av_image_copy(image_buffer, image_line_size, buffer_data, buffer_line_size, out_format.format, out_format.width, out_format.height);

//     freeBuffer();
//     image_size = av_image_alloc(buffer_data, buffer_line_size, format.width, format.height, format.format, 1);
//     if (image_size < 0)
//     {
//         std::cout << name << " updateBuffer av_image_alloc failed " << image_size << std::endl;
//         return Media::UnknownError;
//     }

//     int sic = sws_scale(sws_ctx, image_buffer, image_line_size, 0, out_format.height, buffer_data, buffer_line_size);
//     if (sic < 0)
//     {
//         std::cout << name << " updateBuffer sws_scale failed" << std::endl;
//         return Media::UnknownError;
//     }
//     sws_freeContext(sws_ctx);

//     // is_buffer_data = true;
//     in_format = out_format = format;
//     buffer_max_size = buffer_size = image_size;
//     clearBuffer_data(image_buffer, image_line_size);
//     return Media::NoneError;
// }

int VideoBuffer::getBuffer(uint8_t** buf_data)
{
    AVFrame* frame = nullptr;
    if (buffer_que == nullptr)
    {
        std::cout << name << " getBuffer buffer_que = nullptr" << std::endl;
        return Media::UnknownError;
    }

    int sic = buffer_que->pop(frame, 10);
    if (sic < 0)
    {
        if (media_decoder->state() == CodecState::Stopped && buffer_que->size() == 0)
        {
            std::cout << name << " VideoBuffer getBuffer Media::TaskStopped" << std::endl;
            return Media::TaskStopped;
        }
        return Media::TaskWaiting;
    }

    sic = sws_ctx == nullptr ? writeBuffer(&frame) : writeConvertBuffer(&frame);
    if (sic < 0) {
        return sic;
    }

    std::copy(std::begin(buffer_data), std::end(buffer_data), buf_data);
    return Media::NoneError;
}

int VideoBuffer::setMediaDecoder(MediaDecoder* decoder, int index)
{
    AVStream* stream = nullptr;
    AVFormatContext* format = decoder->getFormat_ctx();

    if (index < 0 || index >= (int)format->nb_streams)
    {
        index = av_find_best_stream(format, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        if (index < 0)
        {
            char error[256];
            av_strerror(index, error, sizeof(error));
            std::cout << "VideoBuferr setMediaDecoder av_find_best_stream " << error << std::endl;
            return Media::UnknownError;
        }
    }

    stream = format->streams[index];
    if (stream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
    {
        std::cout << "VideoBuferr setMediaDecoder codec_type != AVMEDIA_TYPE_VIDEO" << std::endl;
        return Media::UnknownError;
    }

    in_format = out_format = stream->codecpar;
    out_format.framerate = in_format.framerate = stream->avg_frame_rate;
    setDuration(0, stream->duration);

    name = format->url;
    stream_index = stream->index;
    buffer_que = decoder->getFrame_que(stream->index);

    return stream_index;
}


int VideoBuffer::initConvertData()
{
    if (sws_ctx)
    {
        sws_freeContext(sws_ctx);
        sws_ctx = nullptr;
    }
    sws_ctx = sws_getContext(in_format.width, in_format.height, in_format.format,
        out_format.width, out_format.height, out_format.format,
        SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

    if (sws_ctx == nullptr)
    {
        std::cout << name << " VideoBuffer initConvertData sws_getContext failed" << std::endl;
        return Media::UnknownError;
    }
    return Media::NoneError;
}

int VideoBuffer::allocBuffer(int pix_fmt, int width, int height, int align)
{
    buffer_size = av_image_get_buffer_size((AVPixelFormat)pix_fmt, width, height, align);
    if (buffer_size < 0)
    {
        return Media::UnknownError;
    }

    if (buffer_max_size < buffer_size)
    {
        freeBuffer();
        buffer_max_size = av_image_alloc(buffer_data, buffer_line_size, width, height, (AVPixelFormat)pix_fmt, align);
        if (buffer_max_size < 0)
        {
            return Media::UnknownError;
        }
    }
    return Media::NoneError;
}

void VideoBuffer::freeBuffer()
{
    // av_freep(*buffer_data);
    av_freep(&buffer_data[0]);
    for (int i = 0; i < Media::DataPointerCount; i++)
    {
        buffer_line_size[i] = 0;
    }
    buffer_size = buffer_max_size = 0;
}

int VideoBuffer::writeBuffer(AVFrame** frame_ptr)
{
    auto frame = *frame_ptr;
    int sic = allocBuffer(frame->format, frame->width, frame->height, 4);
    if (sic < 0)
    {
        std::cout << name << " VideoBuffer writeBuffer allocBuffer failed" << std::endl;
        av_frame_free(frame_ptr);
        return Media::UnknownError;
    }

    av_image_copy(buffer_data, buffer_line_size, frame->data, frame->linesize, (AVPixelFormat)frame->format, frame->width, frame->height);
    av_frame_free(frame_ptr);
    return Media::NoneError;
}

int VideoBuffer::writeConvertBuffer(AVFrame** frame_ptr)
{
    auto frame = *frame_ptr;
    int sic = allocBuffer(out_format.format, out_format.width, out_format.height, 4);
    if (sic < 0)
    {
        std::cout << name << " VideoBuffer writeConvertBuffer allocBuffer failed" << std::endl;
        av_frame_free(frame_ptr);
        return Media::UnknownError;
    }

    int line_size = sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, buffer_data, buffer_line_size);
    if (line_size < 0)
    {
        Media::coutError(line_size, name, " VideoBuffer writeConvertBuffer sws_scale failed");
        av_frame_free(frame_ptr);
        return Media::UnknownError;
    }
    av_frame_free(frame_ptr);
    return Media::NoneError;
}

