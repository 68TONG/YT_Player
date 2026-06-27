#include "MusicDatabase.h"

MusicDatabase::MusicDatabase()
    : DB_Database<MusicFileInfo, MusicListInfo>("MusicFileInfo", "MusicListInfo")
{
    if (getListInfo(playListInfo_Name, MusicListInfo::I_IsValid).toBool() == false) {
        addListInfo(MusicListInfo(playListInfo_Name));
    }
}

MusicDatabase::~MusicDatabase()
{
}

MusicDatabase& MusicDatabase::obj()
{
    static MusicDatabase obj;
    return obj;
}

MusicListInfo MusicDatabase::getPlayListInfo()
{
    return getListInfo(playListInfo_Name);
}

// int MusicDatabase::updateFileInfo()
// {
//     if (MusicDatabase_EnableUpdateFile == false) return 0;
//     //////////////////////////////////
//     //////////////////////////////////
//     //////////////////////////////////
//     //////////////////////////////////
//     //////////////////////////////////

//     // qDebug() << updateFileInfo_Future;
//     for (auto it : updateFileInfo_Future)
//     {
//         if (it == nullptr)
//             continue;
//         it->waitForFinished();
//     }
//     for (auto it = updateFileInfo_Path.begin(); it != updateFileInfo_Path.end(); ++it)
//     {
//         int info_id = it.key();
//         const auto file_path = it.value();
//         const auto target_path = getFileInfo(info_id, DB_FileInfo::I_FilePath).toString();
//         if (file_path.isEmpty())
//             continue;

//         qDebug() << "updateFileInfo start" + file_path + " and " + target_path;
//         if (moveFile(file_path, target_path, true).isEmpty())
//         {
//             qDebug() << "updateFileInfo failed" + file_path + " and " + target_path;
//         }
//     }
//     updateFileInfo_Future.clear();
//     updateFileInfo_Path.clear();
//     return 0;
// }

// int MusicDatabase::updateFileInfo(int info_id)
// {
//     if (MusicDatabase_EnableUpdateInfo == false) return 0;
//     //////////////////////////////////
//     //////////////////////////////////
//     //////////////////////////////////
//     //////////////////////////////////
//     //////////////////////////////////
//     if (getFileInfo(info_id, DB_FileInfo::I_IsValid).toBool() == false) {
//         qDebug() << "MusicDatabase::updateFileInfo info_id:" << info_id << "is not valid";
//         return -1;
//     }
//     if (updateFileInfo_Path.find(info_id) != updateFileInfo_Path.end()) {
//         qDebug() << "MusicDatabase::updateFileInfo info_id:" << info_id << "is updated";
//         return -2;
//     }

//     auto info = getFileInfo(info_id);
//     auto watcher = new QFutureWatcher<QJsonObject>();
//     updateFileInfo_Path[info_id] = "";
//     updateFileInfo_Future[info_id] = watcher;
//     QObject::connect(watcher, &QFutureWatcher<QJsonObject>::finished, qApp, [this, watcher, info]() {
//         auto ret = watcher->result();
//         if (info.Tags != ret) {
//             setFileInfo(info.ID, DB_FileInfo::I_FileTag, QJsonDocument(ret).toJson());
//         }
//         updateFileInfo_Future[info.ID] = nullptr;
//         watcher->deleteLater();
//         });
//     auto future = QtConcurrent::run([this, info]() -> QJsonObject {
//         QJsonObject info_tags = {};
//         const auto info_path = info.Path;
//         MusicFileInfo::getMusicMetaData(info_path, info_tags);

//         auto json_list = YT_Func::getTH_Wiki("https://thwiki.cc/歌词:" + DB::jsTagFirst(info_tags, Media::MetaData_Title).toString());
//         if (json_list.isEmpty()) {
//             json_list = YT_Func::getTH_Wiki("https://thwiki.cc/" + DB::jsTagFirst(info_tags, Media::MetaData_Album).toString());
//         }
//         if (json_list.isEmpty()) {
//             const QString error = " MusicDatabase::updateFileInfo getTH_Wiki isEmpty";
//             qDebug() << info_path << error;
//             qDebug().noquote() << "=== info_tags ===" << '\n' << QJsonDocument(info_tags).toJson(QJsonDocument::Indented) << '\n';
//             // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
//             return info_tags;
//         }

//         int index = YT_Func::getBestMusicTag(json_list, info_tags, QFileInfo(info_path).absoluteDir().dirName());
//         if (index == INT_MIN) {
//             const QString error = " MusicDatabase::updateFileInfo best_json duplicate";
//             qDebug() << info_path << error;
//             qDebug().noquote() << "=== info_tags ===" << '\n' << QJsonDocument(info_tags).toJson(QJsonDocument::Indented) << '\n';
//             // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
//             return info_tags;
//         }
//         if (index < 0) {
//             const QString error = " MusicDatabase::updateFileInfo best_json nullptr";
//             qDebug() << info_path << error;
//             qDebug().noquote() << "=== info_tags ===" << '\n' << QJsonDocument(info_tags).toJson(QJsonDocument::Indented) << '\n';
//             // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
//             return info_tags;
//         }
//         if (info_tags == json_list[index]) {
//             const QString error = " MusicDatabase::updateFileInfo best_json = info.Tags";
//             qDebug() << info_path << error;
//             // YT_Func::obj().sendMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
//             return info_tags;
//         }

//         MediaEncoder media_encoder;
//         QFileInfo file_info(info_path);
//         const QString output_path = "./YT_PlayerOutput/" + QString::number(info.ID) + '.' + file_info.suffix();

//         int sic = media_encoder.setMediaPath(output_path.toUtf8());
//         if (sic < 0) {
//             const QString error = " MusicDatabase::updateFileInfo media_encoder setMediaPath error";
//             qDebug() << info_path << error;
//             // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
//             return info_tags;
//         }

//         AudioBuffer audio_buffer(info_path.toUtf8());
//         if (audio_buffer.isValid() == false) {
//             const QString error = " MusicDatabase::updateFileInfo audio_buffer setMediaPath error";
//             qDebug() << info_path << error;
//             // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
//             return info_tags;
//         }

//         sic = media_encoder.addStream(&audio_buffer);
//         if (sic < 0) {
//             const QString error = " MusicDatabase::updateFileInfo media_encoder addStream error";
//             qDebug() << info_path << error;
//             // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
//             return info_tags;
//         }

//         sic = YT_Func::MediaEncoder_Write_YT_Data(media_encoder, QJsonDocument(json_list[index]).toJson());
//         if (sic < 0) {
//             const QString error = " MusicDatabase::updateFileInfo MediaEncoder_Write_YT_Data error";
//             qDebug() << info_path << error;
//             // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
//             return info_tags;
//         }

//         sic = media_encoder.encodeRun();
//         if (sic < 0) {
//             const QString error = " MusicDatabase::updateFileInfo media_encoder encodeRun error";
//             qDebug() << info_path << error;
//             // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
//             return info_tags;
//         }

//         updateFileInfo_Path[info.ID] = output_path;
//         qDebug() << info_path << " MusicDatabase::updateFileInfo media_encoder encodeRun finished";
//         YT_Func::obj().sendMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + " MusicDatabase::updateFileInfo info.Tags update finished", 10000);
//         YT_Func::obj().sendMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + " output_path is " + output_path, 10000);
//         // qDebug().noquote() << "\n=== info_tags ===\n" << QJsonDocument(info_tags).toJson(QJsonDocument::Indented);
//         // qDebug().noquote() << "\n=== best_json ===\n" << QJsonDocument(*best_json).toJson(QJsonDocument::Indented);
//         return json_list[index];
//         });

//     watcher->setFuture(future);
//     return 0;
// }

// int MusicDatabase::updateFileInfo(QString path)
// {
//     return updateFileInfo(getFileInfo_ID(path));
// }
