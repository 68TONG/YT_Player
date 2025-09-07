#include "VoidBuffer.h"

VoidBuffer::~VoidBuffer()
{
    VoidBuffer::resetThis();
}

int VoidBuffer::setMediaPath(const char *url)
{
    is_media_decoder = true;
    media_decoder = new MediaDecoder();

    int sic = media_decoder->initMediaPath(url);
    if(sic < 0) return sic;

    return Media::NoneError;
}

void VoidBuffer::resetThis()
{
    name = "";
    stream_index = -1;
    media_decoder = NULL;
    begin_pts = end_pts = -1;
    time_base = AV_TIME_BASE_Q;

    if(is_media_decoder) delete media_decoder;
    is_media_decoder = false;
    media_decoder = NULL;
}

void VoidBuffer::setDuration(int64_t begin, int64_t end)
{
    end_pts = end;
    begin_pts = begin;
}
