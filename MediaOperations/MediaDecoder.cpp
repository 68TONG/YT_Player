#include "MediaDecoder.h"

MediaDecoder::MediaDecoder()
{
}

MediaDecoder::~MediaDecoder()
{
    release();
}

int MediaDecoder::initMediaPath(const char *url)
{
    release();

    format_ctx = avformat_alloc_context();
    int sic = avformat_open_input(&format_ctx, url, NULL, NULL);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << url << " MediaDecoder initMediaPath avformat_open_input " << error << std::endl;
        return sic;
    }

    sic = avformat_find_stream_info(format_ctx, NULL);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << url << " MediaDecoder initMediaPath avformat_find_stream_info " << error << std::endl;
        return sic;
    }

    cur_state = CodecState::Ready;
    return 0;
}

bool MediaDecoder::addStreams_ID(std::set<int> list_id, std::set<int> *list_is)
{
    if (cur_state != CodecState::Ready)
        return false;

    bool ret = true;
    for (auto id : list_id)
    {
        if (id < 0 || id >= (int)format_ctx->nb_streams)
        {
            ret = false;
            continue;
        }
        if (list_is)
            list_is->insert(id);
        codec_ids.insert(id);
    }
    return ret;
}

bool MediaDecoder::setStreams_ID(std::set<int> list_id, std::set<int> *list_is)
{
    if (cur_state != CodecState::Ready)
        return false;

    codec_ids.clear();
    return addStreams_ID(list_id, list_is);
}

bool MediaDecoder::setStreams_Type(std::set<AVMediaType> list_type, std::map<int, int> *list_is)
{
    if (cur_state != CodecState::Ready)
        return false;

    bool ret = true;
    codec_ids.clear();
    for (auto it : list_type)
    {
        int index = av_find_best_stream(format_ctx, it, -1, -1, NULL, 0);
        if (index < 0)
        {
            ret = false;
            av_strerror(index, error, sizeof(error));
            std::cout << format_ctx->url << " MediaDecoder setStreams av_find_best_stream " << error << std::endl;
            continue;
        }
        if (list_is)
            list_is->insert({it, index});
        codec_ids.insert(index);
    }
    return ret;
}

void MediaDecoder::clearDecoders()
{
    for (auto it = codec_ctxs.begin(); it != codec_ctxs.end(); it++)
    {
        avcodec_free_context(&(it->second));
    }
    codec_ctxs.clear();
}

bool MediaDecoder::initDecoders(std::set<int> *list_is)
{
    return initDecoders(codec_ids, list_is);
}

bool MediaDecoder::initDecoders(std::set<int> list_id, std::set<int> *list_is)
{
    if (cur_state != CodecState::Ready)
        return false;

    bool ret = true;
    for (auto id : list_id)
    {
        if (codec_ctxs.find(id) != codec_ctxs.end())
        {
            avcodec_free_context(&(codec_ctxs[id]));
            codec_ctxs.erase(id);
        }

        AVCodecContext *codec_ctx = openDecoder(id);
        if (codec_ctx == NULL)
        {
            ret = false;
            continue;
        }

        if (list_is)
            list_is->insert(id);
        codec_ctxs[id] = codec_ctx;
    }
    return ret;
}

AVStream *MediaDecoder::getStream(int index)
{
    if (index < 0 || index >= format_ctx->nb_streams)
        return nullptr;
    return format_ctx->streams[index];
}

AVFormatContext *MediaDecoder::getFormat_ctx()
{
    return format_ctx;
}

MutexQueue<AVPacket *> *MediaDecoder::getPacket_que(int index)
{
    if (cur_state == CodecState::Invalid)
        return nullptr;
    if (index < 0 || index >= format_ctx->nb_streams)
        return nullptr;
    return &packet_ques[index];
}

MutexQueue<AVFrame *> *MediaDecoder::getFrame_que(int index)
{
    if (cur_state == CodecState::Invalid)
        return nullptr;
    if (index < 0 || index >= format_ctx->nb_streams)
        return nullptr;
    return &frame_ques[index];
}

VoidBuffer *MediaDecoder::getCreateBuffer(int index)
{
    if (cur_state == CodecState::Invalid)
        return nullptr;
    if (index < 0 || index >= format_ctx->nb_streams)
        return nullptr;

    if (create_buffers.find(index) != create_buffers.end())
    {
        return create_buffers[index];
    }

    const auto codec_type = format_ctx->streams[index]->codecpar->codec_type;
    if (codec_type == AVMEDIA_TYPE_AUDIO)
    {
        create_buffers[index] = new AudioBuffer(this, index);
    }
    else if (codec_type == AVMEDIA_TYPE_VIDEO)
    {
        create_buffers[index] = new VideoBuffer(this, index);
    }
    else
    {
        return nullptr;
    }
    // create_buffers[index] =  ->setMediaDecoder(this, index);
    return create_buffers[index];
}

AVPacket *MediaDecoder::getPacket()
{
    auto packet = av_packet_alloc();
    while (true)
    {
        int sic = av_read_frame(format_ctx, packet);
        if (sic < 0)
        {
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaDecoder getPacket av_read_frame " << error << std::endl;
            av_packet_free(&packet);
            return nullptr;
        }

        int index = packet->stream_index;
        if (codec_ids.find(index) == codec_ids.end())
        {
            av_packet_unref(packet);
            continue;
        }
    }
    return packet;
}

AVFrame *MediaDecoder::getFrame()
{
    if (frame_que.empty() == false)
    {
        auto ret = frame_que.front();
        frame_que.pop();
        return ret;
    }

    auto packet = getPacket();
    if (packet == nullptr)
        return nullptr;

    int index = packet->stream_index;
    auto codec_ctx = codec_ctxs[index];
    int sic = avcodec_send_packet(codec_ctx, packet);
    av_packet_free(&packet);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaDecoder getFrame avcodec_send_packet " << error << std::endl;
        return nullptr;
    }

    while (true)
    {
        auto frame = av_frame_alloc();
        sic = avcodec_receive_frame(codec_ctx, frame);
        if (sic == 0)
        {
            frame_que.push(frame);
        }
        else if (sic == AVERROR(EAGAIN))
        {
            av_frame_free(&frame);
            break;
        }
        else if (sic < 0)
        {
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaDecoder decodecPacket avcodec_receive_frame " << error << std::endl;
            av_frame_free(&frame);
            return nullptr;
        }
    }
    return getFrame();
}

void MediaDecoder::start()
{
    if (cur_state == CodecState::Invalid || cur_state == CodecState::Running)
        return;

    cur_state = CodecState::Running;
    // for (auto it = packet_ques.begin(); it != packet_ques.end(); it++)
    // {
    //     it->second.Abort(false);
    // }
    // for (auto &it : frame_ques)
    // {
    //     it.second.Abort(false);
    // }
}

void MediaDecoder::stop()
{
    if (cur_state == CodecState::Invalid || cur_state == CodecState::Stopped)
        return;

    cur_state = CodecState::Stopped;
    // for (auto it = packet_ques.begin(); it != packet_ques.end(); it++)
    // {
    //     it->second.Abort(true);
    // }
    // for (auto it = frame_ques.begin(); it != frame_ques.end(); it++)
    // {
    //     it->second.Abort(true);
    // }
}

void MediaDecoder::seek(int64_t seek_ts)
{
    is_seek = true;

    // for (auto it = packet_ques.begin(); it != packet_ques.end(); it++)
    // {
    //     it->second.Abort(false);
    // }
    // for (auto it = frame_ques.begin(); it != frame_ques.end(); it++)
    // {
    //     it->second.Abort(false);
    // }

    int sic = avformat_seek_file(format_ctx, -1, INT64_MIN, seek_ts, INT64_MAX, AVSEEK_FLAG_ANY);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaDecoder seek avformat_seek_file " << error << std::endl;
        return;
    }

    for (auto it = packet_ques.begin(); it != packet_ques.end(); it++)
    {
        clearPacketQue(it->second);
    }
    for (auto it = frame_ques.begin(); it != frame_ques.end(); it++)
    {
        clearFrameQue(it->second);
    }
    for (auto it = codec_ctxs.begin(); it != codec_ctxs.end(); it++)
    {
        avcodec_flush_buffers(it->second);
    }

    is_seek = false;
}

void MediaDecoder::release()
{
    stop();
    codec_ids.clear();

    if (format_ctx)
    {
        avformat_close_input(&format_ctx);
    }

    for (auto it = packet_ques.begin(); it != packet_ques.end(); it++)
    {
        clearPacketQue(it->second);
    }
    packet_ques.clear();
    for (auto it = frame_ques.begin(); it != frame_ques.end(); it++)
    {
        clearFrameQue(it->second);
    }
    frame_ques.clear();

    clearDecoders();
    clearCreateBuffers();

    cur_state = CodecState::Invalid;
}

CodecState MediaDecoder::state() const
{
    return cur_state;
}

void MediaDecoder::decodeRun()
{
    if (cur_state != CodecState::Running)
        start();
    while (cur_state == CodecState::Running)
    {
        AVPacket *packet = nullptr;
        int sic = readPacket(&packet);
        if (sic < 0)
            return stop();
        if (packet == NULL || sic == false)
            continue;

        sic = decodePacket(&packet);
        if (sic < 0)
            return stop();
    }
    stop();
}

void MediaDecoder::decodeRun_TimeEnd(int64_t end_time)
{
    if (cur_state != CodecState::Running)
        start();
    while (cur_state == CodecState::Running)
    {
        AVPacket *packet = NULL;
        int sic = readPacket(&packet);
        if (sic < 0)
            return stop();
        if (packet == NULL)
            continue;

        AVCodecContext *codec_ctx = codec_ctxs[packet->stream_index];
        int64_t pts = av_rescale_q(packet->pts, codec_ctx->time_base, {1, 1000000});
        if (pts >= end_time)
        {
            av_packet_free(&packet);
            return stop();
        }
        if (sic == false)
            continue;

        sic = decodePacket(&packet);
        if (sic < 0)
            return stop();
    }
    return stop();
}

void MediaDecoder::decodeRun_CountWait(int wait_count, int wait_stream_index)
{
    if (cur_state != CodecState::Running)
        start();
    if (wait_stream_index < 0)
    {
        int64_t max_duration = INT64_MIN;
        for (auto it : codec_ids)
        {
            auto stream = format_ctx->streams[it];
            if (max_duration >= stream->duration)
                continue;

            max_duration = stream->duration;
            wait_stream_index = it;
        }
        if (wait_stream_index < 0)
            wait_stream_index = *codec_ids.begin();
    }
    wait_stream_index = *codec_ids.begin();
    while (cur_state == CodecState::Running)
    {
        int cur_count = std::max(packet_ques[wait_stream_index].size(), frame_ques[wait_stream_index].size());
        if (cur_count > wait_count)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        AVPacket *packet = nullptr;
        int sic = readPacket(&packet);
        if (sic < 0)
            return stop();
        if (packet == nullptr || sic == 0)
            continue;

        sic = decodePacket(&packet);
        if (sic < 0)
            return stop();
    }
    return stop();
}

void MediaDecoder::decodeRun_Play(int wait_count, int wait_stream_index)
{
    if (cur_state != CodecState::Running)
        start();
    if (wait_stream_index < 0)
        wait_stream_index = *codec_ids.begin();
    while (cur_state == CodecState::Running)
    {
        int cur_count = std::max(packet_ques[wait_stream_index].size(), frame_ques[wait_stream_index].size());
        if (cur_count > wait_count || is_seek)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        AVPacket *packet = NULL;
        int sic = readPacket(&packet);
        if (sic == AVERROR_EOF)
        {
            // for (auto it = packet_ques.begin(); it != packet_ques.end(); it++)
            // {
            //     it->second.Abort(true);
            // }
            // for (auto it = frame_ques.begin(); it != frame_ques.end(); it++)
            // {
            //     it->second.Abort(true);
            // }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        if (sic < 0)
            return stop();
        if (packet == NULL || sic == false)
            continue;

        sic = decodePacket(&packet);
        if (sic < 0)
            return stop();
    }
    return stop();
}

AVCodecContext *MediaDecoder::openDecoder(int id)
{
    AVCodecParameters *codec_par = format_ctx->streams[id]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codec_par->codec_id);
    if (codec == NULL)
    {
        std::cout << format_ctx->url << " openDecoder avcodec_find_decoder " << "codec = NULL" << std::endl;
        return NULL;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    if (codec_ctx == NULL)
    {
        std::cout << format_ctx->url << " openDecoder avcodec_alloc_context3 " << "codec_ctx = NULL" << std::endl;
        return NULL;
    }

    int sic = avcodec_parameters_to_context(codec_ctx, codec_par);
    if (sic < 0)
    {
        avcodec_free_context(&codec_ctx);

        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " openDecoder avcodec_parameters_to_context " << error << std::endl;
        return NULL;
    }

    sic = avcodec_open2(codec_ctx, codec, NULL);
    if (sic < 0)
    {
        avcodec_free_context(&codec_ctx);

        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " openDecoder avcodec_open2 " << error << std::endl;
        return NULL;
    }
    return codec_ctx;
}

int MediaDecoder::readPacket(AVPacket **packet_ptr)
{
    AVPacket *packet = av_packet_alloc();
    int sic = av_read_frame(format_ctx, packet);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaDecoder readPacket av_read_frame " << error << std::endl;
        av_packet_free(&packet);
        return sic;
    }

    int index = packet->stream_index;
    if (codec_ids.find(index) == codec_ids.end())
    {
        av_packet_free(&packet);
        return 0;
    }

    *packet_ptr = packet;
    if (codec_ctxs.find(index) == codec_ctxs.end())
    {
        packet_ques[index].push(packet);
        return 0;
    }

    return +true;
}

int MediaDecoder::decodePacket(AVPacket **packet_ptr)
{
    auto packet = *packet_ptr;
    *packet_ptr = nullptr;

    int index = packet->stream_index;
    AVCodecContext *codec_ctx = codec_ctxs[index];
    int sic = avcodec_send_packet(codec_ctx, packet);
    if (sic < 0)
    {
        av_strerror(sic, error, sizeof(error));
        std::cout << format_ctx->url << " MediaDecoder decodePacket avcodec_send_packet " << error << std::endl;
        av_packet_free(&packet);
        return sic;
    }
    av_packet_free(&packet);

    while (true)
    {
        AVFrame *frame = av_frame_alloc();
        sic = avcodec_receive_frame(codec_ctx, frame);
        if (sic == 0)
        {
            frame_ques[index].push(frame);
        }
        else if (sic == AVERROR(EAGAIN))
        {
            av_frame_free(&frame);
            break;
        }
        else if (sic < 0)
        {
            av_strerror(sic, error, sizeof(error));
            std::cout << format_ctx->url << " MediaDecoder decodePacket avcodec_receive_frame " << error << std::endl;
            av_frame_free(&frame);
            return sic;
        }
    }
    return Media::NoneError;
}

template <typename T>
void MediaDecoder::abortQues(std::map<int, MutexQueue<T>> &ques, bool is_abort)
{
    for (auto it = ques.begin(); it != ques.end(); it++)
    {
        it->second.Abort(is_abort);
    }
}

void MediaDecoder::clearPacketQue(MutexQueue<AVPacket *> &que)
{
    std::queue<AVPacket *> read_que;
    que.pop(read_que);

    for (; read_que.empty() == false; read_que.pop())
    {
        AVPacket *val = read_que.front();
        av_packet_free(&val);
    }
}

void MediaDecoder::clearFrameQue(MutexQueue<AVFrame *> &que)
{
    std::queue<AVFrame *> read_que;
    que.pop(read_que);

    for (; read_que.empty() == false; read_que.pop())
    {
        AVFrame *val = read_que.front();
        av_frame_free(&val);
    }
}

void MediaDecoder::clearCreateBuffers()
{
    for (auto &&it : create_buffers)
    {
        delete it.second;
    }
    create_buffers.clear();
}
