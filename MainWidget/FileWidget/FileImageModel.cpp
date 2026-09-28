#include "FileImageModel.h"

#include "YT_Info.h"
#include "YT_Func.h"

FileImageModel::FileImageModel()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    {}
}

QPixmap FileImageModel::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    // if (YT_Info::getObject().aboutToQuit) return {};

    const QString cover_hash = id.size() == 32 ? id : YT_Func::outputFileCover(id);
    QPixmap target_data("./YT_PlayerCache/" + cover_hash);
    if (target_data.isNull()) {
        // target_data = fileIconProvider.icon(QFileInfo(id)).pixmap(requestedSize.isValid() ? requestedSize : QSize(300, 300));
    }
    if (target_data.isNull()) {
        target_data.load(":/Ui_Image/failed_image.png");
    }
    if (target_data.isNull()) {
        return {};
    }

    if (size) *size = QSize(target_data.width(), target_data.height());
    const int target_width = requestedSize.width() > 0 ? requestedSize.width() : target_data.width();
    const int target_height = requestedSize.height() > 0 ? requestedSize.height() : target_data.height();

    if (*size == requestedSize) return target_data;
    if (requestedSize.isValid() == false) return target_data;
    return target_data.scaled(target_width, target_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
