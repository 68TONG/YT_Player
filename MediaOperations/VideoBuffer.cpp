#include "VideoBuffer.h"

VideoBuffer::~VideoBuffer()
{
    VideoBuffer::resetThis();
}

int VideoBuffer::setMediaPath(const char *url)
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

int VideoBuffer::setMediaDecoder(MediaDecoder *decoder, int index)
{
    AVStream *stream = NULL;
    AVFormatContext *format = decoder->getFormat_ctx();

    if (index < 0 || index >= (int)format->nb_streams) {
        index = av_find_best_stream(format, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        if (index < 0) {
            char error[256];
            av_strerror(index, error, sizeof(error));
            std::cout << "VideoBuferr setMediaDecoder av_find_best_stream " << error << std::endl;
            return Media::UnknownError;
        }
    }
    stream = format->streams[index];
    if (stream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO) {
        std::cout << "VideoBuferr setMediaDecoder codec_type != AVMEDIA_TYPE_VIDEO" << std::endl;
        return Media::UnknownError;
    }

    resetThis();
    setOutInfo(stream->codecpar);
    setDuration(0, stream->duration);
    out_format.framerate = in_format.framerate = stream->avg_frame_rate;

    name = format->url;
    media_decoder = decoder;
    stream_index = stream->index;
    buffer_que = decoder->getFrame_que(stream->index);

    return stream_index;
}

void VideoBuffer::setOutInfo(VideoFormat &format)
{
    if (format.format != out_format.format || format.width != out_format.width || format.height != out_format.height) {
        clearBuffer_data();
    }
    if (sws_ctx) {
        sws_freeContext(sws_ctx);
        sws_ctx = NULL;
    }
    in_format = out_format = format;
}

void VideoBuffer::setOutInfo(AVCodecParameters *info)
{
    if (info->format != out_format.format || info->width != out_format.width || info->height != out_format.height) {
        clearBuffer_data();
    }
    if (sws_ctx) {
        sws_freeContext(sws_ctx);
        sws_ctx = NULL;
    }
    in_format = out_format = info;
}

void VideoBuffer::resetThis()
{
    clearBuffer_data();
    VoidBuffer::resetThis();

    in_format = out_format = VideoFormat();
    if (sws_ctx) {
        sws_freeContext(sws_ctx);
        sws_ctx = NULL;
    }
}

void VideoBuffer::clearBuffer_data()
{
    clearBuffer_data(buffer_data, buffer_line_size);

    is_buffer_data = false;
    buffer_index = buffer_size = buffer_max_size = 0;
}

void VideoBuffer::clearBuffer_data(uint8_t **data, int *line_size)
{
    if(data != NULL && data[0] != NULL){
        av_freep(&data[0]);
    }
    for(int i = 0;i < Media::DataPointerCount;i++){
        data[i] = NULL;
        line_size[i] = 0;
    }
}

int VideoBuffer::updateBuffer()
{
    is_buffer_data = false;

    AVFrame *frame = NULL;
    if(buffer_que == NULL){
        std::cout << name << " updateBuffer buffer_que = NULL" << std::endl;
        return Media::UnknownError;
    }

    int sic = buffer_que->pop(frame, 10);
    if(sic < 0){
        if(buffer_que->isAbort() == true && buffer_que->size() == 0){
            // std::cout << name << " updateBuffer buffer_que.abort = true & buffer_que.size = "  << buffer_que->size() << std::endl;
            return Media::TaskStopped;
        }
        return Media::TaskWaiting;
    }

    if (frame->width != in_format.width || frame->height != in_format.height || frame->format != in_format.format) {
        if (sws_ctx) sws_freeContext(sws_ctx);
        sws_ctx = sws_getContext(frame->width, frame->height, (AVPixelFormat)frame->format,
                                 out_format.width, out_format.height,  out_format.format, SWS_FAST_BILINEAR, NULL, NULL, NULL);

        if (sws_ctx == NULL) {
            std::cout << name << " updateBuffer sws_getContext failed" << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        in_format.width = frame->width;
        in_format.height = frame->height;
        in_format.format = (AVPixelFormat)frame->format;
    }

    if (sws_ctx != NULL) {
        int64_t image_size = av_image_get_buffer_size(out_format.format, out_format.width, out_format.height, 1);
        if (image_size < 0) {
            std::cout << name << " updateBuffer av_image_get_buffer_size failed" << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        if (buffer_max_size < image_size) {
            clearBuffer_data();
            buffer_max_size = av_image_alloc(buffer_data, buffer_line_size, out_format.width, out_format.height, out_format.format, 1);
            if (buffer_max_size < 0) {
                std::cout << name << " updateBuffer av_image_get_buffer_size failed" << std::endl;
                av_frame_free(&frame);
                return Media::UnknownError;
            }
        }

        sic = sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, buffer_data, buffer_line_size);
        if (sic < 0) {
            std::cout << name << " updateBuffer sws_scale failed" << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        buffer_size = image_size;
    }
    else {
        buffer_size = av_image_get_buffer_size((AVPixelFormat)frame->format, frame->width, frame->height, 1);
        if(buffer_size < 0){
            std::cout << name << " not sws updateBuffer av_image_get_buffer_size failed" << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        if (buffer_max_size < buffer_size) {
            clearBuffer_data();
            buffer_max_size = av_image_alloc(buffer_data, buffer_line_size, frame->width, frame->height, (AVPixelFormat)frame->format, 1);
            if(buffer_max_size < 0){
                std::cout << name << " not sws updateBuffer av_image_alloc failed" << std::endl;
                av_frame_free(&frame);
                return Media::UnknownError;
            }
        }

        av_image_copy(buffer_data, buffer_line_size, frame->data, frame->linesize, (AVPixelFormat)frame->format, frame->width, frame->height);
    }

    is_buffer_data = true;
    av_frame_free(&frame);
    return buffer_size;
}

int VideoBuffer::updateBuffer(const VideoFormat &format)
{
    is_buffer_data = false;
    if (format.width == out_format.width && format.height == out_format.height && format.format == out_format.format) {
        is_buffer_data = true;
        return Media::UnknownError;
    }

    SwsContext *sws_ctx = sws_getContext(out_format.width, out_format.height, out_format.format,
                                         format.width, format.height,  format.format,
                                         SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if (sws_ctx == NULL) {
        std::cout << name << " updateBuffer sws_getContext failed" << std::endl;
        return Media::UnknownError;
    }

    int image_line_size[Media::DataPointerCount] = {0};
    uint8_t *image_buffer[Media::DataPointerCount] = {NULL};
    int64_t image_size = av_image_alloc(image_buffer, image_line_size, out_format.width, out_format.height, out_format.format, 1);
    if (image_size < 0) {
        std::cout << name << " updateBuffer av_image_alloc failed " << image_size << std::endl;
        return Media::UnknownError;
    }

    av_image_copy(image_buffer, image_line_size, buffer_data, buffer_line_size, out_format.format, out_format.width, out_format.height);

    clearBuffer_data();
    image_size = av_image_alloc(buffer_data, buffer_line_size, format.width, format.height, format.format, 1);
    if(image_size < 0){
        std::cout << name << " updateBuffer av_image_alloc failed " << image_size << std::endl;
        return Media::UnknownError;
    }

    int sic = sws_scale(sws_ctx, image_buffer, image_line_size, 0, out_format.height, buffer_data, buffer_line_size);
    if(sic < 0){
        std::cout << name << " updateBuffer sws_scale failed" << std::endl;
        return Media::UnknownError;
    }
    sws_freeContext(sws_ctx);

    is_buffer_data = true;
    in_format = out_format = format;
    buffer_max_size = buffer_size = image_size;
    clearBuffer_data(image_buffer, image_line_size);
    return Media::NoneError;
}

uint8_t **VideoBuffer::getBuffers()
{
    if(is_buffer_data == false) return NULL;
    return buffer_data;
}

