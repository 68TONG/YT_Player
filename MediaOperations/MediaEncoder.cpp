#include "MediaEncoder.h"

struct FLACStreamInfo
{
    uint16_t minBlockSize = 4096; // 16位
    uint16_t maxBlockSize = 4096; // 16位
    uint32_t minFrameSize = 0;    // 24位
    uint32_t maxFrameSize;        // 24位
    uint32_t sampleRate;          // 20位
    uint8_t channels;             // 3位
    uint8_t bitsPerSample;        // 5位
    uint64_t sampleCount;         // 36位
    uint8_t md5Signature[16];     // 128位

    void initData(const uint8_t* buffer)
    {
        minBlockSize = (buffer[0] << 8) | buffer[1];
        maxBlockSize = (buffer[2] << 8) | buffer[3];
        minFrameSize = (buffer[4] << 16) | (buffer[5] << 8) | buffer[6];
        maxFrameSize = (buffer[7] << 16) | (buffer[8] << 8) | buffer[9];
        sampleRate = (buffer[10] << 12) | (buffer[11] << 4) | (buffer[12] >> 4);
        channels = ((buffer[12] >> 1) & 0x07);
        bitsPerSample = ((buffer[12] & 0x01) << 4) | ((buffer[13] & 0xf0) >> 4);
        sampleCount = ((uint64_t)(buffer[13] & 0x0f) << 32) | (buffer[14] << 24) | (buffer[15] << 16) | (buffer[16] << 8) | (buffer[17]);
        memcpy(md5Signature, (buffer + 18), 16);
    }

    void toData(uint8_t* buffer)
    {
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

        memcpy((buffer + 18), md5Signature, 16);
    }

    void coutData() const
    {
        std::cout << "Min Block Size: " << minBlockSize << std::endl;
        std::cout << "Max Block Size: " << maxBlockSize << std::endl;
        std::cout << "Min Frame Size: " << minFrameSize << std::endl;
        std::cout << "Max Frame Size: " << maxFrameSize << std::endl;
        std::cout << "Sample Rate: " << sampleRate << std::endl;
        std::cout << "Channels: " << static_cast<int>(channels) << std::endl;
        std::cout << "Bits Per Sample: " << static_cast<int>(bitsPerSample) << std::endl;
        std::cout << "Sample Count: " << sampleCount << std::endl
            << std::endl;
    }
};

static const std::set<std::string> cover_image_formats = { "mp3", "flac", "mp4", "mkv" };
static const std::set<int> image_stream_type_list = {
    AVCodecID::AV_CODEC_ID_PNG,
    AVCodecID::AV_CODEC_ID_MJPEG };

MediaEncoder::MediaEncoder() {}

MediaEncoder::~MediaEncoder()
{
    release();
}

int MediaEncoder::setMediaPath(const char* url)
{
    release();

    int sic = avformat_alloc_output_context2(&format_ctx, NULL, NULL, url);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << url << " MediaEncoder setMediaPath avformat_alloc_output_context2 " << error << std::endl;
        return sic;
    }

    cur_state = CodecState::Ready;
    return Media::NoneError;
}

int MediaEncoder::addStream(VoidBuffer* buffer)
{
    return addStream(buffer->stream_index, buffer->mediaDecoder());
}

int MediaEncoder::addStream(AudioBuffer* buffer)
{
    return addStream(buffer->stream_index, buffer->mediaDecoder(), buffer->outFormat());
}

int MediaEncoder::addStream(VideoBuffer* buffer)
{
    return addStream(buffer->stream_index, buffer->mediaDecoder());
}

int MediaEncoder::addStream(int stream_index, MediaDecoder* decoder)
{
    if (decoder == nullptr)
        return Media::UnknownError;
    auto source_stream = decoder->getStream(stream_index);
    if (source_stream->disposition & AV_DISPOSITION_ATTACHED_PIC)
    {
        return addCoverStream(stream_index, decoder);
    }
    else if (cover_image_formats.count(format_ctx->oformat->name) &&
        image_stream_type_list.count(source_stream->codecpar->codec_id))
    {
        return addCoverStream(stream_index, decoder);
    }

    // int sic = avcodec_parameters_copy(stream->codecpar, source_stream->codecpar);
    // if (sic < 0)
    // {
    //     av_strerror(sic, error, sizeof(error));
    //     std::cout << format_ctx->url << " MediaEncoder addStream avcodec_parameters_copy " << error << std::endl;
    //     return sic;
    // }

    auto stream = addStream(source_stream);
    if (stream == nullptr)
        return Media::UnknownError;

    stream->index = format_ctx->nb_streams - 1;
    stream->duration = source_stream->duration;
    stream->time_base = source_stream->time_base;

    auto info = new EncodeInfo(stream->index, source_stream->index, decoder);
    if (stream->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO)
        info->type = MediaType::Audio;
    else if (stream->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO)
        info->type = MediaType::Video;

    codec_ids.insert(stream->index);
    encode_infos[stream->index] = info;

    int64_t scale_duration = av_rescale_q(source_stream->duration, source_stream->time_base, this->time_base);
    if (scale_duration > duration)
        duration = scale_duration;
    format_ctx->duration = duration;

    return stream->index;
}

int MediaEncoder::addStream(int stream_index, MediaDecoder* decoder, AudioFormat audio_fmt)
{
    if (decoder == nullptr)
        return Media::UnknownError;
    auto source_stream = decoder->getStream(stream_index);
    auto source_codec_par = decoder->getStream(stream_index)->codecpar;
    if ((source_codec_par->codec_id == audio_fmt.codec_id) &&
        (source_codec_par->format == audio_fmt.format) &&
        (source_codec_par->sample_rate == audio_fmt.sample_rate) &&
        (source_codec_par->ch_layout.nb_channels == audio_fmt.ch_layout.nb_channels))
    {
        return addStream(stream_index, decoder);
    }

    auto stream = addStream(source_stream);
    if (stream == nullptr)
        return Media::UnknownError;

    auto codec_par = stream->codecpar;
    codec_par->codec_id = audio_fmt.codec_id;
    codec_par->codec_type = AVMediaType::AVMEDIA_TYPE_AUDIO;
    codec_par->sample_rate = audio_fmt.sample_rate;
    codec_par->ch_layout = audio_fmt.ch_layout;
    codec_par->format = audio_fmt.format;

    stream->time_base = { 1, codec_par->sample_rate };
    stream->duration = av_rescale_q(source_stream->duration, source_stream->time_base, stream->time_base);
    stream->index = format_ctx->nb_streams - 1;

    auto codec_ctx = openEncoder(stream);
    if (codec_ctx == nullptr)
    {
        std::cout << format_ctx->url << " MediaEncoder addStream openEncoder return nullptr" << std::endl;
        return Media::UnknownError;
    }
    if (avcodec_parameters_from_context(codec_par, codec_ctx) < 0)
    {
        std::cout << format_ctx->url << " MediaEncoder addStream avcodec_parameters_from_context error" << std::endl;
        return Media::UnknownError;
    }
    if (codec_par->codec_id == AV_CODEC_ID_FLAC)
    {
        FLACStreamInfo flac_info;
        flac_info.initData(codec_par->extradata);

        flac_info.sampleCount = stream->duration;
        flac_info.toData(codec_par->extradata);
    }

    auto info = new EncodeInfo(stream->index, source_stream->index, decoder);
    info->type = MediaType::Audio;
    info->is_encode = true;
    info->codec_ctx = codec_ctx;

    auto media_buffer = static_cast<AudioBuffer*>(info->media_buffer);
    media_buffer->setOutFormat(audio_fmt);

    codec_ids.insert(stream->index);
    encode_infos[stream->index] = info;

    int64_t scale_duration = av_rescale_q(source_stream->duration, source_stream->time_base, this->time_base);
    if (scale_duration > duration)
        duration = scale_duration;
    format_ctx->duration = duration;

    return stream->index;
}

int MediaEncoder::addCoverStream(const char* image_path)
{
    auto decoder = new MediaDecoder();
    media_decoders.insert(decoder);

    int sic = decoder->initMediaPath(image_path);
    if (sic < 0)
    {
        std::cout << format_ctx->url << " MediaEncoder addCoverStream initMediaPath return < 0" << std::endl;
        return sic;
    }

    // auto de_format_ctx = decoder->getFormat_ctx();
    // int index = av_find_best_stream(de_format_ctx, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    // if (index < 0)
    // {
    //     av_strerror(index, error, sizeof(error));
    //     std::cout << format_ctx->url << " MediaEncoder addCoverStream av_find_best_stream " << error << std::endl;
    // }

    return addCoverStream(0, decoder);
}

int MediaEncoder::addCoverStream(int stream_index, MediaDecoder* decoder)
{
    auto de_format_ctx = decoder->getFormat_ctx();
    auto source_stream = de_format_ctx->streams[stream_index];

    auto stream = addStream(source_stream);
    if (stream == nullptr)
        return Media::UnknownError;

    stream->index = format_ctx->nb_streams - 1;
    stream->disposition = (stream->disposition | AV_DISPOSITION_ATTACHED_PIC);

    auto info = new EncodeInfo(stream->index, source_stream->index, decoder);
    info->type = MediaType::Image;

    codec_ids.insert(stream->index);
    encode_infos[stream->index] = info;
    return stream->index;
}

int MediaEncoder::addLyricsStream(const char* data)
{
    int sic = av_dict_set(&format_ctx->metadata, Media::MetaData_Lyrics, data, AV_DICT_APPEND);
    if (sic < 0)
    {
        char error[256];
        av_strerror(sic, error, sizeof(error));
        std::cout << "MediaEncoder addLyricsStream av_dict_set " << error << std::endl;
        return Media::UnknownError;
    }
    return Media::NoneError;
}

void MediaEncoder::start()
{
    if (cur_state == CodecState::Invalid || cur_state == CodecState::Running)
        return;

    cur_state = CodecState::Running;
}

void MediaEncoder::stop()
{
    if (cur_state == CodecState::Invalid || cur_state == CodecState::Stopped)
        return;

    cur_state = CodecState::Stopped;
}

void MediaEncoder::release()
{
    stop();
    if (format_ctx)
    {
        avformat_close_input(&format_ctx);
        format_ctx = nullptr;
    }

    codec_ids.clear();
    for (auto it : encode_infos)
    {
        delete it.second;
    }
    encode_infos.clear();

    for (auto it : media_decoders)
    {
        delete it;
    }
    media_decoders.clear();

    duration = progress = max_pts = 0;
    cur_state = CodecState::Invalid;
}

int MediaEncoder::encodeRun()
{
    if (cur_state != CodecState::Running)
        start();
    if (cur_state != CodecState::Running)
        return Media::UnknownError;

    {
        std::set<MediaDecoder*> media_decoder_list;
        for (auto it : encode_infos)
        {
            auto info = it.second;
            if (info->type == MediaType::Image)
                continue;
            media_decoder_list.insert(info->media_decoder);
        }

        for (auto it : media_decoder_list)
        {
            it->seek(0);
            it->setStreams_ID({});
            it->clearDecoders();

            auto de_format_ctx = it->getFormat_ctx();
            int sic = av_dict_copy(&format_ctx->metadata, de_format_ctx->metadata, AV_DICT_DONT_OVERWRITE);
            if (sic < 0)
            {
                char error[256];
                av_strerror(sic, error, sizeof(error));
                std::cout << "MediaEncoder encodeRun av_dict_copy " << error << std::endl;
            }
            av_dict_set(&format_ctx->metadata, "User_Data", nullptr, 0);
        }
    }

    {
        std::set<MediaDecoder*> media_decoder_list;
        for (auto it : encode_infos)
        {
            auto info = it.second;
            info->record_pts = 0;

            auto media_decoder = info->media_decoder;
            media_decoder->addStreams_ID({ info->de_stream_index });
            if (info->is_encode)
            {
                media_decoder->initDecoders({ info->de_stream_index });
            }
            media_decoder_list.insert(media_decoder);
        }

        int task_id = ThreadPool::getObject().getTaskID(media_decoder_list.size());
        for (auto it : media_decoder_list)
        {
            ThreadPool::getObject().addTask(task_id, [](MediaDecoder* media_decoder) {
                media_decoder->decodeRun_CountWait();
                }, it);
        }
    }

    if (setMediaHeader() < 0)
    {
        stop();
        return Media::UnknownError;
    }

    for (auto index : codec_ids)
    {
        auto info = encode_infos[index];
        if (info->type != MediaType::Image)
            continue;
        encodeImagePacket(index);
        info->is_stopped = true;
    }

    while (cur_state == CodecState::Running && duration > max_pts)
    {
        progress = progress + 0.005;
        max_pts = duration * progress;

        // std::cout << progress << "  " << duration << "   " << max_pts << std::endl;

        for (auto index : codec_ids)
        {
            int sic = 0;
            auto info = encode_infos[index];
            if (info->is_stopped)
                continue;

            if (info->is_encode == false)
                sic = writePacket(index);
            else if (info->type == MediaType::Audio)
                sic = encodeAudioPacket(index);
            else if (info->type == MediaType::Video)
                sic = encodeVideoPacket(index);
            else
                continue;

            if (sic == Media::TaskStopped)
            {
                info->is_stopped = true;
                std::cout << format_ctx->url << " MediaEncoder encodeRun TaskStopped stream_index = " << index << std::endl;
            }
            else if (sic < 0)
            {
                stop();
                return Media::UnknownError;
            }
        }
    }

    if (setMediaTrailer() < 0)
    {
        stop();
        return Media::UnknownError;
    }

    if (duration > max_pts) {
        stop();
        return Media::UnknownError;
    }

    std::cout << format_ctx->url << " MediaEncoder encodeRun End" << std::endl;
    stop();
    return Media::NoneError;

}

int MediaEncoder::setMediaHeader()
{
    if (format_ctx == NULL)
    {
        std::cout << " MediaEncoder setMediaHeader format_ctx = NULL " << std::endl;
        return Media::UnknownError;
    }

    int sic = avio_open(&format_ctx->pb, format_ctx->url, AVIO_FLAG_WRITE);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder setMediaHeader avio_open " << error << std::endl;
        return sic;
    }

    sic = avformat_write_header(format_ctx, NULL);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder setMediaHeader avformat_write_header " << error << std::endl;
        return sic;
    }

    return Media::NoneError;
}

int MediaEncoder::setMediaTrailer()
{
    int sic = av_write_trailer(format_ctx);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder setMediaTrailer av_write_trailer " << error << std::endl;
        return sic;
    }
    return Media::NoneError;
}

AVStream* MediaEncoder::addStream(AVStream* source_stream)
{
    // auto stream = avformat_new_stream(format_ctx, avcodec_find_encoder(source_stream->codecpar->codec_id));
    // if (stream == nullptr)
    // {
    //     std::cout << format_ctx->url << " MediaEncoder addStream avformat_new_stream return nullptr" << std::endl;
    //     return nullptr;
    // }
    // return stream;

    auto stream = avformat_new_stream(format_ctx, nullptr);
    if (stream == nullptr)
    {
        std::cout << format_ctx->url << " MediaEncoder addStream avformat_new_stream return nullptr" << std::endl;
        return nullptr;
    }

    int sic = avcodec_parameters_copy(stream->codecpar, source_stream->codecpar);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder addCoverStream avcodec_parameters_copy " << error << std::endl;
        return nullptr;
    }
    return stream;
}

AVCodecContext* MediaEncoder::openEncoder(AVStream* stream, AVDictionary* opts)
{
    AVCodecParameters* codec_par = stream->codecpar;
    const AVCodec* codec = avcodec_find_encoder(codec_par->codec_id);
    if (codec == NULL)
    {
        std::cout << format_ctx->url << " MediaEncoder openEncoder avcodec_find_encoder codec = NULL" << std::endl;
        return NULL;
    }

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (codec_ctx == NULL)
    {
        std::cout << format_ctx->url << " MediaEncoder openEncoder avcodec_alloc_context3 codec_ctx = NULL" << std::endl;
        return NULL;
    }

    int sic = avcodec_parameters_to_context(codec_ctx, codec_par);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder openEncoder avcodec_parameters_to_context " << error << std::endl;
        return NULL;
    }
    codec_ctx->time_base = stream->time_base;

    sic = avcodec_open2(codec_ctx, codec, &opts);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder openEncoder avcodec_open2 " << error << std::endl;
        return NULL;
    }

    return codec_ctx;
}

int MediaEncoder::writePacket(int stream_index)
{
    int64_t pts = 0;
    AVPacket* packet = nullptr;
    auto info = encode_infos[stream_index];
    auto stream = format_ctx->streams[stream_index];
    auto packet_que = info->media_decoder->getPacket_que(info->de_stream_index);
    while (max_pts > pts)
    {
        if (packet_que->pop(packet, 10) < 0)
        {
            if (info->media_decoder->state() == CodecState::Stopped && packet_que->size() == 0)
            {
                return Media::TaskStopped;
            }
            continue;
        }

        packet->stream_index = stream_index;
        pts = av_rescale_q(packet->pts, stream->time_base, this->time_base);
        int sic = writePacket(&packet);
        if (sic < 0)
            return Media::UnknownError;
    }
    return Media::NoneError;
}

int MediaEncoder::encodeAudioPacket(int stream_index)
{
    int64_t pts = 0;
    AVPacket* packet = NULL;
    auto info = encode_infos[stream_index];
    auto stream = format_ctx->streams[stream_index];
    auto media_buffer = static_cast<AudioBuffer*>(info->media_buffer);
    auto codec_ctx = info->codec_ctx;

    while (max_pts > pts)
    {
        AVFrame* frame = av_frame_alloc();
        frame->format = codec_ctx->sample_fmt;
        frame->sample_rate = codec_ctx->sample_rate;
        frame->nb_samples = codec_ctx->frame_size;
        frame->ch_layout = codec_ctx->ch_layout;
        frame->time_base = codec_ctx->time_base;

        info->record_pts += frame->nb_samples;
        frame->pts = info->record_pts;
        pts = av_rescale_q(info->record_pts, codec_ctx->time_base, this->time_base);

        int sic = av_frame_get_buffer(frame, 0);
        if (sic < 0)
        {
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder encodeAudioPacket av_frame_get_buffer " << error << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        sic = av_frame_make_writable(frame);
        if (sic < 0)
        {
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder encodeAudioPacket av_frame_make_writable " << error << std::endl;
            av_frame_free(&frame);
            return Media::UnknownError;
        }

        sic = media_buffer->fillFrame(frame);
        if (sic == Media::TaskStopped)
        {
            av_frame_free(&frame);
            return sic;
        }
        else if (sic < 0)
        {
            std::cout << format_ctx->url << " MediaEncoder encodeAudioPacket fillFrame Error" << std::endl;
            av_frame_free(&frame);
            return sic;
        }

        sic = writePacket(stream_index, &frame, codec_ctx);
        if (sic < 0)
            return sic;
    }

    return Media::NoneError;
}

int MediaEncoder::encodeVideoPacket(int stream_index)
{
    return Media::NoneError;
}

int MediaEncoder::encodeImagePacket(int stream_index)
{
    auto info = encode_infos[stream_index];
    auto stream = format_ctx->streams[stream_index];
    auto source_stream = info->media_decoder->getStream(info->de_stream_index);

    AVPacket* packet = nullptr;
    if (source_stream->disposition & AV_DISPOSITION_ATTACHED_PIC)
    {
        packet = av_packet_alloc();
        av_packet_ref(packet, &source_stream->attached_pic);
    }
    else
    {
        auto packet_que = info->media_decoder->getPacket_que(info->de_stream_index);
        while (info->media_decoder->state() != CodecState::Stopped || packet_que->size() > 0)
        {
            if (packet_que->pop(packet, 10) >= 0)
            {
                break;
            }
        }
        if (packet == nullptr)
        {
            std::cout << format_ctx->url << " MediaEncoder encodeImagePacket packet == nullptr stream = " << stream_index << std::endl;
            return Media::UnknownError;
        }
    }

    packet->pts = packet->dts = 0;
    packet->stream_index = stream_index;
    int sic = writePacket(&packet);
    if (sic < 0)
    {
        return sic;
    }
    return Media::NoneError;
}

int MediaEncoder::writePacket(AVPacket** packet_ptr)
{
    // int sic = av_write_frame(format_ctx, *packet_ptr);
    int sic = av_interleaved_write_frame(format_ctx, *packet_ptr);
    av_packet_free(packet_ptr);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder writePacket av_interleaved_write_frame " << error << std::endl;
        return sic;
    }
    return Media::NoneError;
}

int MediaEncoder::writePacket(int stream_index, AVFrame** frame_ptr, AVCodecContext* codec_ctx)
{
    int sic = avcodec_send_frame(codec_ctx, *frame_ptr);
    av_frame_free(frame_ptr);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaEncoder writePacket avcodec_send_frame " << error << std::endl;
        return Media::UnknownError;
    }

    while (true)
    {
        AVPacket* packet = av_packet_alloc();
        sic = avcodec_receive_packet(codec_ctx, packet);
        if (sic == 0)
        {
            packet->stream_index = stream_index;
            sic = writePacket(&packet);
            if (sic < 0)
                return Media::UnknownError;
        }
        else if (sic == AVERROR(EAGAIN))
        {
            av_packet_free(&packet);
            break;
        }
        else if (sic < 0)
        {
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaEncoder encodeAudioPacket avcodec_receive_packet " << error << std::endl;
            av_packet_free(&packet);
            return Media::UnknownError;
        }
    }
    return Media::NoneError;
}

MediaEncoder::EncodeInfo::EncodeInfo(int en_index, int de_index, MediaDecoder* media_decoder)
{
    this->en_stream_index = en_index;
    this->de_stream_index = de_index;
    this->media_decoder = media_decoder;
    this->media_buffer = media_decoder->getCreateBuffer(de_index);
}

MediaEncoder::EncodeInfo::~EncodeInfo()
{
    if (codec_ctx)
    {
        avcodec_free_context(&codec_ctx);
        codec_ctx = NULL;
    }
}
