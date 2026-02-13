#include "MusicImageModel.h"

#include "Music_DataBase.h"
#include "MediaDecoder.h"

MusicImageModel::MusicImageModel()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage MusicImageModel::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    const auto path = Music_DataBase::getObject().getItemInfo(id.toInt()).Path;
    if (path.isEmpty())
        return {};

    // TimeDebugger d(TimeDebugger::Seconds);
    VideoBuffer buffer(path.toUtf8());
    if (buffer.isValid() == false)
        return {};
    // d.elapsedSeconds("create");

    uint8_t* bufs[Media::DataPointerCount] = { nullptr };
    buffer.outFormat().format = AV_PIX_FMT_RGB24;
    buffer.initFormat();
    buffer.mediaDecoder()->decodeRun();
    // d.elapsedSeconds("decode");
    int sic = buffer.getBuffer(bufs);
    if (sic < 0)
        return {};

    return QImage((const uchar*)(bufs[0]), buffer.outFormat().width, buffer.outFormat().height, QImage::Format_RGB888).copy();
}
