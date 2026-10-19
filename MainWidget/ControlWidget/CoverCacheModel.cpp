#include "CoverCacheModel.h"

#include <QDir>
#include <QPixmap>
#include "YT_Func.h"

CoverCacheModel::CoverCacheModel()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    {}
}

QPixmap CoverCacheModel::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    const QString cover_hash = id.size() == 32 ? id : YT_Func::outputFileCover(id);
    QPixmap target_data;

    if (cover_hash.isEmpty() == false) {
        const QDir cache_dir("./YT_PlayerCache");
        const auto cache_files = cache_dir.entryList({ cover_hash + ".*" }, QDir::Files, QDir::Name);
        if (cache_files.isEmpty() == false) {
            target_data.load(cache_dir.filePath(cache_files.first()));
        }
    }
    if (target_data.isNull()) {
        QIcon::setThemeName("Material Symbols Rounded");
        target_data = QIcon::fromTheme("broken_image").pixmap(QSize(300, 300));

        // target_data.load(":/Ui_Image/failed_image.png");
    }
    if (target_data.isNull()) {
        return QPixmap();
    }

    if (size) {
        *size = target_data.size();
    }
    if (requestedSize.isValid() == false) {
        return target_data;
    }
    return target_data.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
