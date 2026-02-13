#include "VoidBuffer.h"

#include "MediaDecoder.h"

VoidBuffer::VoidBuffer(const char* url)
    : is_media_decoder(true)
    , media_decoder(new MediaDecoder())
{
    int sic = media_decoder->initMediaPath(url);
    if (sic < 0) is_valid = false;
}

VoidBuffer::VoidBuffer(MediaDecoder* decoder)
    : is_media_decoder(false)
    , media_decoder(decoder)
{}

VoidBuffer::~VoidBuffer()
{
    name = "";
    stream_index = -1;
    begin_pts = end_pts = -1;
    time_base = AV_TIME_BASE_Q;

    if (is_media_decoder) delete media_decoder;
}

bool VoidBuffer::isValid() const
{
    return is_valid;
}

MediaDecoder* VoidBuffer::mediaDecoder() const
{
    return media_decoder;
}

void VoidBuffer::setDuration(int64_t begin, int64_t end)
{
    end_pts = end;
    begin_pts = begin;
}
