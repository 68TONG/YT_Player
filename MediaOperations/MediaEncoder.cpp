#include "MediaEncoder.h"

struct FLACStreamInfo {
    uint16_t minBlockSize = 4096;          // 16位
    uint16_t maxBlockSize = 4096;          // 16位
    uint32_t minFrameSize = 0;          // 24位
    uint32_t maxFrameSize;          // 24位
    uint32_t sampleRate;           // 20位
    uint8_t channels;               // 3位
    uint8_t bitsPerSample;          // 5位
    uint64_t sampleCount;        // 36位
    uint8_t md5Signature[16];       // 128位

    void initData(const uint8_t* buffer) {
        minBlockSize = (buffer[0] << 8) | buffer[1];
        maxBlockSize = (buffer[2] << 8) | buffer[3];
        minFrameSize = (buffer[4] << 16) | (buffer[5] << 8) | buffer[6];
        maxFrameSize = (buffer[7] << 16) | (buffer[8] << 8) | buffer[9];
        sampleRate = (buffer[10] << 12) | (buffer[11] << 4) | (buffer[12] >> 4);
        channels = ((buffer[12] >> 1) & 0x07);
        bitsPerSample = ((buffer[12] & 0x01) << 4) | ((buffer[13] & 0xf0) >> 4);
        sampleCount = ((uint64_t)(buffer[13] & 0x0f) << 32) | (buffer[14] << 24) | (buffer[15] << 16) | (buffer[16] << 8) | (buffer[17]);
        memcpy(md5Signature, (buffer+18), 16);
    }

    void toData(uint8_t* buffer) {
        buffer[0] = (minBlockSize >> 8);
        buffer[1] = (minBlockSize >> 0);

        buffer[2] = (maxBlockSize >> 8);
        buffer[3] = (maxBlockSize >> 0);

        buffer[4] = (minFrameSize >> 16);
        buffer[5] = (minFrameSize >> 8);
        buffer[6] = (minFrameSize >> 0);

        buffer[7] = (maxFrameSize >> 16);
        buffer[8] = (maxFrameSize >> 8);
        buffer[9] = (maxFrameSize >> 0);

        buffer[10] = (sampleRate >> 12);
        buffer[11] = (sampleRate >> 4);
        buffer[12] = (sampleRate << 4) | ((channels & 0x07) << 1) | ((bitsPerSample >> 4) & 0x01);

        buffer[13] = (bitsPerSample << 4) | (sampleCount >> 32);
        buffer[14] = (sampleCount >> 24);
        buffer[15] = (sampleCount >> 16);
        buffer[16] = (sampleCount >> 8);
        buffer[17] = (sampleCount >> 0);

        memcpy((buffer+18), md5Signature, 16);
    }

    void coutData() const {
        std::cout << "Min Block Size: " << minBlockSize << std::endl;
        std::cout << "Max Block Size: " << maxBlockSize << std::endl;
        std::cout << "Min Frame Size: " << minFrameSize << std::endl;
        std::cout << "Max Frame Size: " << maxFrameSize << std::endl;
        std::cout << "Sample Rate: " << sampleRate << std::endl;
        std::cout << "Channels: " << static_cast<int>(channels) << std::endl;
        std::cout << "Bits Per Sample: " << static_cast<int>(bitsPerSample) << std::endl;
        std::cout << "Sample Count: " << sampleCount << std::endl << std::endl;
    }
};

static const std::set<std::string> cover_image_formats = {"mp3", "flac", "mp4", "mkv"};
static const std::set<int> image_stream_type_list = {
    AVCodecID::AV_CODEC_ID_PNG,
    AVCodecID::AV_CODEC_ID_MJPEG
};

MediaEncoder::MediaEncoder() {}

MediaEncoder::~MediaEncoder()
{
    release();
}

int MediaEncoder::setMediaPath(const char *url)
{
    release();

    int sic = avformat_alloc_output_context2(&format_ctx, NULL, NULL, url);
    if(sic < 0){
        av_strerror(sic, error, sizeof(error));
        std::cout << url << " MediaEncoder setMediaPath avformat_alloc_output_context2 " << error << std::endl;
        return sic;
    }

    return Media::NoneError;
}

int MediaEncoder::addStream(int stream_index, MediaDecoder *decoder)
{
    AVStream *source_stream = decoder->getStream(stream_index);
    if (source_stream->disposition & AV_DISPOSITION_ATTACHED_PIC) {
        return addImageStream(stream_index, decoder);
    } else if(cover_image_formats.count(format_ctx->oformat->name) &&
               image_stream_type_list.count(source_stream->codecpar->codec_id)){
        return addImageStream(stream_index, decoder);
    }

    AVStream *stream = avformat_new_stream(format_ctx, NULL);
    if (stream == NULL) {
        std::cout << format_ctx->url << " MediaEncoder addStream avformat_new_stream return NULL" << std::endl;
        return Media::UnknownError;
    }

    int sic = avcodec_parameters_copy(stream->codecpar, source_stream->codecpar);
    if (sic < 0) {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder addStream avcodec_parameters_copy " << error << std::endl;
        return sic;
    }

    stream->duration = source_stream->duration;
    stream->time_base = source_stream->time_base;

    int type = EncoderInfo::NONE_TYPE;
    if (stream->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
        type = EncoderInfo::AUDIO_TYPE;
    } else if (stream->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO) {
        type = EncoderInfo::VIDEO_TYPE;
    }

    EncoderInfo *encoder_info = new EncoderInfo(stream_index, decoder);
    encoder_info->type = (EncoderInfo::ENCODER_INFO_TYPE)type;
    encoder_info->packet_que = decoder->getPacket_que(stream_index);

    stream->index = format_ctx->nb_streams - 1;
    codec_ids.insert(stream->index);
    encoder_info_que[stream->index] = encoder_info;

    int64_t scale_duration = av_rescale_q(source_stream->duration, source_stream->time_base, this->time_base);
    if(scale_duration > duration) duration = scale_duration;
    format_ctx->duration = duration;

    return stream->index;
}

int MediaEncoder::addStream(int stream_index, MediaDecoder *decoder, AudioFormat audio_fmt)
{
    AVStream *source_stream = decoder->getStream(stream_index);
    auto source_codec_par = decoder->getStream(stream_index)->codecpar;
    int is_encodec = (source_codec_par->codec_id != audio_fmt.codec_id) ||
                     (source_codec_par->sample_rate != audio_fmt.sample_rate) ||
                     (source_codec_par->format != audio_fmt.format);
    if(is_encodec == false){
        return addStream(stream_index, decoder);
    }

    AVStream *stream = avformat_new_stream(format_ctx, NULL);
    if(stream == NULL){
        std::cout << format_ctx->url << " MediaEncoder addStream avformat_new_stream " << std::endl;
        return Media::UnknownError;
    }

    AVCodecParameters *codec_par = stream->codecpar;
    codec_par->codec_id = audio_fmt.codec_id;
    codec_par->codec_type = AVMediaType::AVMEDIA_TYPE_AUDIO;
    codec_par->sample_rate = audio_fmt.sample_rate;
    codec_par->ch_layout = audio_fmt.ch_layout;
    codec_par->format = audio_fmt.format;

    stream->time_base = {1, codec_par->sample_rate};
    stream->duration = av_rescale_q(source_stream->duration, source_stream->time_base, stream->time_base);

    AVCodecContext *codec_ctx = openEncoder(stream);
    if(codec_ctx == NULL){
        std::cout << format_ctx->url << " MediaEncoder addStream openEncoder return NULL" << std::endl;
        return Media::UnknownError;
    }
    if(avcodec_parameters_from_context(codec_par, codec_ctx) < 0){
        std::cout << format_ctx->url << " MediaEncoder addStream avcodec_parameters_from_context error" << std::endl;
        return Media::UnknownError;
    }
    if(codec_par->codec_id == AV_CODEC_ID_FLAC){
        FLACStreamInfo flac_info;
        flac_info.initData(codec_par->extradata);

        flac_info.sampleCount = stream->duration;
        flac_info.toData(codec_par->extradata);
    }

    AudioBuffer *buffer = new AudioBuffer();
    buffer->name = format_ctx->url;
    buffer->setMediaDecoder(decoder, stream_index);
    buffer->setOutInfo(codec_par);

    EncoderInfo *encoder_info = new EncoderInfo(stream_index, decoder);
    encoder_info->type = (EncoderInfo::ENCODER_INFO_TYPE)(EncoderInfo::IS_ENCODE | EncoderInfo::AUDIO_TYPE);
    encoder_info->codec_ctx = codec_ctx;
    encoder_info->packet_que = new MutexQueue<AVPacket *>();
    encoder_info->in_buffer = buffer;

    stream->index = format_ctx->nb_streams - 1;
    codec_ids.insert(stream->index);
    encoder_info_que[stream->index] = encoder_info;

    int64_t scale_duration = av_rescale_q(source_stream->duration, source_stream->time_base, this->time_base);
    if(scale_duration > duration) duration = scale_duration;
    format_ctx->duration = duration;

    return stream->index;
}

int MediaEncoder::addImageStream(const char *image_path)
{
    MediaDecoder decoder;
    int sic = decoder.initMediaPath(image_path);
    if(sic < 0){
        std::cout << format_ctx->url << " MediaEncoder addImageStream initMediaPath return < 0" << std::endl;
        return sic;
    }

    auto decoder_format = decoder.getFormat_ctx();
    int index = av_find_best_stream(decoder_format, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if(index < 0){
        av_strerror(index, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder addImageStream av_find_best_stream " << error << std::endl;
    }

    AVStream *stream = decoder.getStream(index);
    sic = addImageStream(stream->index, &decoder);
    if(sic < 0) return sic;

    return Media::NoneError;
}

int MediaEncoder::addImageStream(int stream_index, MediaDecoder *decoder)
{
    auto decoder_format = decoder->getFormat_ctx();
    AVStream *source_stream = decoder_format->streams[stream_index];

    AVStream *stream = avformat_new_stream(format_ctx, NULL);
    if(stream == NULL){
        std::cout << format_ctx->url << " MediaEncoder addImageStream avformat_new_stream return NULL" << std::endl;
        return Media::UnknownError;
    }

    int sic = avcodec_parameters_copy(stream->codecpar, source_stream->codecpar);
    if(sic < 0){
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder addImageStream avcodec_parameters_copy " << error << std::endl;
        return sic;
    }
    stream->index = format_ctx->nb_streams - 1;
    stream->disposition = (stream->disposition | AV_DISPOSITION_ATTACHED_PIC);

    AVPacket *packet = av_packet_alloc();

    if(image_stream_type_list.count(source_stream->codecpar->codec_id)){
        sic = av_read_frame(decoder_format, packet);
        if(sic < 0){
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder addImageStream av_read_frame " << error << std::endl;
            av_packet_free(&packet);
            return sic;
        }
    } else if(source_stream->disposition & AV_DISPOSITION_ATTACHED_PIC){
        sic = av_packet_ref(packet, &source_stream->attached_pic);
        if(sic < 0){
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder addImageStream av_packet_ref " << error << std::endl;
            av_packet_free(&packet);
            return sic;
        }
    } else {
        std::cout << format_ctx->url << " MediaEncoder addImageStream source_stream != image " << std::endl;
        return Media::UnknownError;
    }

    packet->stream_index = stream->index;
    EncoderInfo *encoder_info = new EncoderInfo(stream_index, NULL);
    encoder_info->packet_que = new MutexQueue<AVPacket *>();
    encoder_info->type = EncoderInfo::IMAGE_TYPE;
    sic = encoder_info->packet_que->push(packet);
    if(sic < 0){
        delete encoder_info;
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder addImageStream packet_que.push return < 0" << std::endl;
        return sic;
    }

    codec_ids.insert(stream->index);
    encoder_info_que[stream->index] = encoder_info;
    return Media::NoneError;
}

void MediaEncoder::start()
{
    if(is_abort == false) return ;
    is_abort = false;
}

void MediaEncoder::stop()
{
    if(is_abort == true) return ;
    is_abort = true;
}

void MediaEncoder::release()
{
    stop();
    if(format_ctx){
        avformat_close_input(&format_ctx);
        format_ctx = NULL;
    }
    clearEncoderInfo(encoder_info_que);
    duration = progress = max_pts = 0;
}

std::set<MediaDecoder *> MediaEncoder::getMediaDecoderList()
{
    std::set<MediaDecoder *> media_decoder_list;
    for(auto it : encoder_info_que){
        auto info = it.second;
        if(info->media_decoder == NULL) continue;

        media_decoder_list.insert(info->media_decoder);
    }

    for(auto it : media_decoder_list){
        it->seek(0);
        it->setStreams_ID({});
        it->clearDecoders();
    }

    for(auto it : encoder_info_que){
        auto info = it.second;
        auto decoder = info->media_decoder;
        if(info->type & EncoderInfo::IMAGE_TYPE) continue;

        decoder->addStreams_ID({info->stream_index});
        if(info->type & EncoderInfo::IS_ENCODE){
            decoder->initDecoders({info->stream_index});
        }
    }

    return media_decoder_list;
}

void MediaEncoder::encodecRun()
{
    start();
    if(setMediaHeader() < 0) return stop();
    while(is_abort == false && duration > max_pts){
        progress = progress + 0.005;
        max_pts = duration * progress;

        // std::cout << progress << "  " << duration << "   " << max_pts << std::endl;

        for(auto index : codec_ids){
            int sic = 0;
            EncoderInfo *info = encoder_info_que[index];
            if((info->type & EncoderInfo::IS_ABORT) || (info->type & EncoderInfo::IMAGE_TYPE)){
                continue;
            }

            if((info->type & EncoderInfo::IS_ENCODE) == false){
                sic = writePacket(index);
            } else if((info->type & EncoderInfo::IS_ENCODE) && (info->type & EncoderInfo::AUDIO_TYPE)){
                sic = encodecAudioPacket(index);
            } else if((info->type & EncoderInfo::IS_ENCODE) && (info->type & EncoderInfo::VIDEO_TYPE)){
                sic = encodecVideoPacket(index);
            }

            if(sic == Media::TaskStopped){
                info->type = (EncoderInfo::ENCODER_INFO_TYPE)(info->type | EncoderInfo::IS_ABORT);
            } else if(sic < 0) {
                return stop();
            }
        }
    }
    if(setMediaTrailer() < 0) return stop();
    return stop();
}

int MediaEncoder::setMediaHeader()
{
    if(format_ctx == NULL){
        std::cout << " MediaEncoder setMediaHeader format_ctx = NULL " << std::endl;
        return Media::UnknownError;
    }

    int sic = avio_open(&format_ctx->pb, format_ctx->url, AVIO_FLAG_WRITE);
    if(sic < 0){
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder setMediaHeader avio_open " << error << std::endl;
        return sic;
    }

    sic = avformat_write_header(format_ctx, NULL);
    if(sic < 0){
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder setMediaHeader avformat_write_header " << error << std::endl;
        return sic;
    }

    for(auto it : codec_ids){
        EncoderInfo *info = encoder_info_que[it];
        if((info->type & EncoderInfo::IMAGE_TYPE) == 0) continue;

        AVPacket *packet = NULL;
        sic = info->packet_que->pop(packet, 10);
        if(sic < 0){
            std::cout << format_ctx->url << " MediaEncoder setMediaHeader packet_que->pop return < 0 " << info->packet_que->size() << std::endl;
            return sic;
        }

        av_write_frame(format_ctx, packet);
        av_packet_free(&packet);
    }

    return Media::NoneError;
}

int MediaEncoder::setMediaTrailer()
{
    int sic = av_write_trailer(format_ctx);
    if(sic < 0){
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder setMediaTrailer av_write_trailer " << error << std::endl;
        return sic;
    }
    return Media::NoneError;
}

AVCodecContext *MediaEncoder::openEncoder(AVStream *stream, AVDictionary *opts)
{
    AVCodecParameters *codec_par = stream->codecpar;
    const AVCodec *codec = avcodec_find_encoder(codec_par->codec_id);
    if(codec == NULL){
        std::cout << format_ctx->url << " MediaEncoder openEncoder avcodec_find_encoder codec = NULL" << std::endl;
        return NULL;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    if(codec_ctx == NULL){
        std::cout << format_ctx->url << " MediaEncoder openEncoder avcodec_alloc_context3 codec_ctx = NULL" << std::endl;
        return NULL;
    }

    int sic = avcodec_parameters_to_context(codec_ctx, codec_par);
    if(sic < 0){
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder openEncoder avcodec_parameters_to_context " << error << std::endl;
        return NULL;
    }
    codec_ctx->time_base = stream->time_base;

    sic = avcodec_open2(codec_ctx, codec, &opts);
    if(sic < 0){
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder openEncoder avcodec_open2 " << error << std::endl;
        return NULL;
    }

    return codec_ctx;
}

int MediaEncoder::writePacket(int stream_index)
{
    int64_t pts = 0;
    AVPacket *packet = NULL;
    AVStream *stream = format_ctx->streams[stream_index];
    EncoderInfo *info = encoder_info_que[stream_index];
    MutexQueue<AVPacket *> *packet_que = info->packet_que;
    while(max_pts > pts){
        if(packet_que->pop(packet, 10) < 0){
            if(packet_que->size() == 0 && packet_que->isAbort() == true){
                std::cout << format_ctx->url << " MediaEncoder writePacket packet_que->abort = true" << std::endl;
                return Media::TaskStopped;
            }
            continue;
        }
        pts = av_rescale_q(packet->pts, stream->time_base, this->time_base);

        packet->stream_index = stream_index;
        int sic = av_write_frame(format_ctx, packet);
        if(sic < 0){
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder writePacket av_write_frame " << error << std::endl;
            av_packet_free(&packet);
            return Media::UnknownError;
        }
        av_packet_free(&packet);
    }
    return Media::NoneError;
}

int MediaEncoder::encodecAudioPacket(int stream_index)
{
    int64_t pts = 0;
    EncoderInfo *info = encoder_info_que[stream_index];
    AVCodecContext *codec_ctx = info->codec_ctx;
    MutexQueue<AVPacket *> *packet_que = info->packet_que;
    AudioBuffer *buffer = static_cast<AudioBuffer *>(info->in_buffer);
    if(buffer == NULL){
        std::cout << format_ctx->url << " MediaEncoder encodecAudioPacket AudioBuffer = NULL" << std::endl;
        return Media::UnknownError;
    }
    while(max_pts > pts){
        AVFrame *frame = av_frame_alloc();
        frame->format = codec_ctx->sample_fmt;
        frame->sample_rate = codec_ctx->sample_rate;
        frame->nb_samples = codec_ctx->frame_size;
        frame->ch_layout = codec_ctx->ch_layout;

        info->record_pts += frame->nb_samples;
        frame->pts = info->record_pts;
        pts = av_rescale_q(info->record_pts, codec_ctx->time_base, this->time_base);

        int sic = av_frame_get_buffer(frame, 0);
        if(sic < 0){
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder encodecAudioPacket av_frame_get_buffer " << error << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        sic = av_frame_make_writable(frame);
        if(sic < 0){
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder encodecAudioPacket av_frame_make_writable " << error << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        sic = buffer->fillFrame(frame);
        if (sic == Media::TaskStopped) {
            av_frame_free(&frame);
            packet_que->Abort(true);
            break;
        } else if(sic < 0) {
            std::cout << format_ctx->url << " MediaEncoder encodecAudioPacket fillFrame Error" << std::endl;
            av_frame_free(&frame);
            return sic;
        }

        sic = avcodec_send_frame(codec_ctx, frame);
        if(sic < 0){
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder encodecAudioPacket avcodec_send_frame " << error << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }
        av_frame_free(&frame);

        while(true){
            AVPacket *packet = av_packet_alloc();
            sic = avcodec_receive_packet(codec_ctx, packet);
            if(sic == 0){
                packet_que->push(packet);
            } else if(sic == AVERROR(EAGAIN)){
                av_packet_free(&packet);
                break;
            } else if(sic < 0){
                av_strerror(sic, error, sizeof (error));
                std::cout << format_ctx->url << " MediaEncoder encodecAudioPacket avcodec_receive_packet " << error << std::endl;
                av_packet_free(&packet);
                return Media::UnknownError;
            }
        }
    }

    while(packet_que->size() != 0){
        AVPacket *packet = NULL;
        int sic = packet_que->pop(packet);
        if(sic < 0) continue;

        packet->stream_index = stream_index;
        sic = av_write_frame(format_ctx, packet);
        if(sic < 0){
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder encodecAudioPacket av_write_frame " << error << std::endl;
            av_packet_free(&packet);
            return Media::UnknownError;
        }
        av_packet_free(&packet);
    }

    if(packet_que->isAbort() == true){
        std::cout << format_ctx->url << " MediaEncoder encodecAudioPacket packet_que->abort = true" << std::endl;
        return Media::TaskStopped;
    }
    return Media::NoneError;
}

int MediaEncoder::encodecVideoPacket(int stream_index)
{
    return Media::NoneError;
}

void MediaEncoder::clearEncoderInfo(std::map<int, EncoderInfo *> &que)
{
    for(auto it : que){
        delete it.second;
    }
    que.clear();
}

MediaEncoder::EncoderInfo::EncoderInfo(int index, MediaDecoder *decoder)
{
    stream_index = index;
    media_decoder = decoder;
}

MediaEncoder::EncoderInfo::~EncoderInfo()
{
    bool is_packet_que = (type & EncoderInfo::IS_ENCODE) || (type & IMAGE_TYPE);
    if(is_packet_que && packet_que != NULL){
        packet_que->Abort(true);
        while(packet_que->size() != 0){
            AVPacket *val = NULL;
            if(packet_que->pop(val) < 0){
                continue;
            }
            av_packet_free(&val);
        }
        delete packet_que;
        packet_que = NULL;
    }
    if(in_buffer){
        delete in_buffer;
        in_buffer = NULL;
    }
    if(codec_ctx){
        avcodec_free_context(&codec_ctx);
        codec_ctx = NULL;
    }
}
