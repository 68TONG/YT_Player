#include "MusicImageModel.h"

#include <QCryptographicHash>
#include "Music_DataBase.h"
#include "MusicDatabase.h"
#include "MediaDecoder.h"

MusicImageModel::MusicImageModel()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    {}
}

QPixmap MusicImageModel::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    // if (YT_Info::getObject().aboutToQuit) return {};

    const QString cover_hash = id.size() == 32 ? id : YT_Func::outputFileCover(id);
    QPixmap target_data("./YT_PlayerCache/" + cover_hash);
    if (target_data.isNull()) {
        target_data.load(":/Ui_Image/failed_image.png");
    }
    if (target_data.isNull()) {
        return {};
    }

    if (size) *size = QSize(target_data.width(), target_data.height());
    const int target_width = requestedSize.width() > 0 ? requestedSize.width() : target_data.width();
    const int target_height = requestedSize.height() > 0 ? requestedSize.height() : target_data.height();
    if (requestedSize.width() < 0 && requestedSize.height() < 0) return target_data;
    return target_data.scaled(target_width, target_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

// QPixmap MusicImageModel::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
// {
//     if (YT_Info::getObject().aboutToQuit) return {};

//     // TimeDebugger debugger(TimeDebugger::TimeUnit::Milli);
//     QString cover_hash = id;
//     if (id.size() != 32) {
//         const auto db_thread = MusicDatabase::obj().db.thread();
//         QMetaObject::invokeMethod(db_thread, [id, &cover_hash]() {
//             cover_hash = MusicDatabase::obj().getFileInfo(id.toInt(), MusicFileInfo::I_CoverHashValue).toString();
//             }, Qt::BlockingQueuedConnection);
//     }

//     QPixmap target_data(cacheRootPath + cover_hash);
//     if (target_data.isNull()) {
//         cover_hash = outputCoverCache(id.toInt());
//     }
//     target_data.load(cacheRootPath + cover_hash);
//     if (target_data.isNull()) {
//         return {};
//     }
//     // debugger.elapsedSinceMilli(cover_hash.toStdString() + " MusicImageModel::requestPixmap: Load Image");

//     if (size) *size = QSize(target_data.width(), target_data.height());
//     const int target_width = requestedSize.width() > 0 ? requestedSize.width() : target_data.width();
//     const int target_height = requestedSize.height() > 0 ? requestedSize.height() : target_data.height();
//     if (requestedSize.width() < 0 && requestedSize.height() < 0) return target_data;
//     return target_data.scaled(target_width, target_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
// }

QString MusicImageModel::outputCoverCache(const int info_id)
{
    QString info_path = "";
    const auto db_thread = MusicDatabase::obj().db.thread();
    QMetaObject::invokeMethod(db_thread, [info_id](QString& ret) {
        ret = MusicDatabase::obj().getFileInfo(info_id, MusicFileInfo::I_FilePath).toString();
        }, Qt::BlockingQueuedConnection
        , info_path);
    if (info_path.isEmpty()) return {};

    const QString target_path = "./YT_PlayerCache/MusicImage" + QString::number(info_id) + ".png";
    if (target_path.isEmpty()) {
        return {};
    }

    MediaEncoder media_encoder;
    int sic = media_encoder.setMediaPath(target_path.toUtf8());
    if (sic < 0) {
        // const QString qDebug_str = QString::number(info_id) + info_path + " MusicImageModel::outputCoverCache media_encoder setMediaPath error";
        // qDebug() << qDebug_str;
        return {};
    }

    VideoBuffer media_buffer(info_path.toUtf8());
    if (media_buffer.isValid() == false) {
        // const QString qDebug_str = QString::number(info_id) + info_path + " MusicImageModel::outputCoverCache media_buffer isValid is false";
        // qDebug() << qDebug_str;
        return {};
    }

    sic = media_encoder.addStream(&media_buffer);
    if (sic < 0) {
        // const QString qDebug_str = QString::number(info_id) + info_path + " MusicImageModel::outputCoverCache media_encoder addStream error";
        // qDebug() << qDebug_str;
        return {};
    }

    sic = media_encoder.encodeRun();
    if (sic < 0) {
        // const QString qDebug_str = QString::number(info_id) + info_path + " MusicImageModel::outputCoverCache media_encoder encodeRun error";
        // qDebug() << qDebug_str;
        return {};
    }
    media_encoder.release();

    QFile file(target_path);
    if (file.exists() == false || file.open(QIODevice::ReadOnly) == false) {
        // qDebug() << info_id << info_path << " MusicImageModel::outputCoverCache file open error";
        return {};
    }

    const QString file_hash = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
    const QString file_path = cacheRootPath + file_hash + '.' + QFileInfo(target_path).suffix();

    if (file.rename(file_path) == false) {
        // qDebug() << info_id << info_path << " MusicImageModel::outputCoverCache file rename error" << file.errorString();
        if (file.remove() == false) {
            // qDebug() << info_id << info_path << " MusicImageModel::outputCoverCache file remove error" << file.errorString();
        }
    }

    QMetaObject::invokeMethod(db_thread, [info_id, file_hash] {
        MusicDatabase::obj().setFileInfo(info_id, MusicFileInfo::I_CoverHashValue, file_hash);
        }, Qt::BlockingQueuedConnection);

    return file_hash;
}
