#include "Music_DataBase.h"

#include "YT_AudioOutput.h"
#include "NE_MediaOutput_Model.h"

Music_DataBase::Music_DataBase()
    : YT_DataBase<Music_ItemInfo, Music_ListInfo>("./YT_PlayerData/Music_ItemInfo.json", "./YT_PlayerData/Music_ListInfo.json")
{
    if (listInfo_Hash.isEmpty())
        listInfo_Hash.insert(defaultListInfoID, Music_ListInfo{ defaultListInfoID, QList<int>() });
    if (listInfo_Hash.find(playListInfoID) == listInfo_Hash.end())
        listInfo_Hash.insert(playListInfoID, Music_ListInfo{ playListInfoID, QList<int>() });
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, qApp, []()
        { Music_DataBase::getObject().updateItemInfo(); }, Qt::DirectConnection);
}

Music_DataBase::~Music_DataBase()
{
}

Music_DataBase& Music_DataBase::getObject()
{
    static Music_DataBase obj;
    return obj;
}

Music_ListInfo& Music_DataBase::getPlayListInfo()
{
    return listInfo_Hash[playListInfoID];
}

int Music_DataBase::updateItemInfo()
{
    YT_AudioOutput::getObject().setSource({});
    for (auto it : updateFileInfo_Future)
    {
        if (it == nullptr)
            continue;
        it->waitForFinished();
    }
    for (auto it = updateFileInfo.begin(); it != updateFileInfo.end(); ++it)
    {
        int info_id = it.key();
        const auto file_path = it.value();
        if (file_path.isEmpty())
            continue;

        qDebug() << "updateFileInfo start" + file_path + " and " + itemInfo_Hash[info_id].Path;
        if (moveFile(file_path, itemInfo_Hash[info_id].Path, true).isEmpty())
        {
            qDebug() << "updateFileInfo failed" + file_path + " and " + itemInfo_Hash[info_id].Path;
        }
    }
    updateFileInfo_Future.clear();
    updateFileInfo.clear();
    return 0;
}

int Music_DataBase::updateItemInfo(int info_id)
{
    static bool is_TestUpdateInfo = false;
    if (listInfo_Hash.find("TestUpdateInfo") != listInfo_Hash.end() && is_TestUpdateInfo)
    {
        is_TestUpdateInfo = false;
        auto ID_List = listInfo_Hash["TestUpdateInfo"].ID_List;
        for (auto it : ID_List)
        {
            updateItemInfo(it);
        }
    }

    if (itemInfo_Hash.find(info_id) == itemInfo_Hash.end())
        return -1;
    if (updateFileInfo.find(info_id) != updateFileInfo.end())
        return -2;

    auto& info = itemInfo_Hash[info_id];
    auto watcher = new QFutureWatcher<QJsonObject>();

    updateFileInfo[info_id] = "";
    updateFileInfo_Future[info_id] = watcher;
    QObject::connect(watcher, &QFutureWatcher<QJsonObject>::finished, watcher, [this, watcher, &info]() {
        auto ret = watcher->result();
        if (info.Tags != ret) info.Tags = ret;
        updateFileInfo_Future[info.ID] = nullptr;
        watcher->deleteLater();
        });
    auto future = QtConcurrent::run([this, info]() -> QJsonObject {
        const auto info_path = info.Path;
        Music_ItemInfo::YT_InfoTag info_tags;
        Music_ItemInfo::getMusicMetaData(info_path, info_tags);

        auto json_list = YT_Func::getTH_Wiki("https://thwiki.cc/歌词:" + info_tags.firstTag(Media::MetaData_Title));
        if (json_list.isEmpty()) {
            json_list = YT_Func::getTH_Wiki("https://thwiki.cc/" + info_tags.firstTag(Media::MetaData_Album));
        }
        if (json_list.isEmpty()) {
            qDebug() << info_path << "Music_DataBase::updateItemInfo json_list.isEmpty()";
            qDebug().noquote() << "\n=== info_tags ===\n" << QJsonDocument(info_tags).toJson(QJsonDocument::Indented);
            return info_tags;
        }

        int index = YT_Func::getBestMusicTag(json_list, info_tags, QFileInfo(info_path).absoluteDir().dirName());
        if (index == INT_MIN) {
            qDebug() << info_path << "Music_DataBase::updateItemInfo best_json duplicate";
            qDebug().noquote() << "\n=== info_tags ===\n" << QJsonDocument(info_tags).toJson(QJsonDocument::Indented);
            return info_tags;
        }
        if (index < 0) {
            qDebug() << info_path << "Music_DataBase::updateItemInfo best_json nullptr";
            qDebug().noquote() << "\n=== info_tags ===\n" << QJsonDocument(info_tags).toJson(QJsonDocument::Indented);
            return info_tags;
        }
        if (info_tags == json_list[index]) {
            qDebug() << info_path << "Music_DataBase::updateItemInfo best_json = info.Tags";
            return info_tags;
        }

        MediaEncoder media_encoder;
        QFileInfo file_info(info_path);
        const QString output_path = "./YT_PlayerOutput/" + QString::number(info.ID) + '.' + file_info.suffix();

        int sic = media_encoder.setMediaPath(output_path.toStdString().c_str());
        if (sic < 0) {
            qDebug() << info_path << "Music_DataBase::updateItemInfo media_encoder setMediaPath error";
            return info_tags;
        }

        AudioBuffer audio_buffer(info_path.toUtf8());
        if (audio_buffer.isValid() == false) {
            qDebug() << info_path << "Music_DataBase::updateItemInfo audio_buffer setMediaPath error";
            return info_tags;
        }

        sic = media_encoder.addStream(&audio_buffer);
        if (sic < 0) {
            qDebug() << info_path << "Music_DataBase::updateItemInfo media_encoder addStream error";
            return info_tags;
        }
        
        YT_Func::MediaEncoder_Write_YT_Data(media_encoder, QString(QJsonDocument(json_list[index]).toJson()));
        sic = media_encoder.encodeRun();
        if (sic < 0) {
            qDebug() << info_path << "Music_DataBase::updateItemInfo media_encoder encodeRun error";
            return info_tags;
        }

        updateFileInfo[info.ID] = output_path;
        qDebug() << info_path << "Music_DataBase::updateItemInfo media_encoder encodeRun finished";
        // qDebug().noquote() << "\n=== info_tags ===\n" << QJsonDocument(info_tags).toJson(QJsonDocument::Indented);
        // qDebug().noquote() << "\n=== best_json ===\n" << QJsonDocument(*best_json).toJson(QJsonDocument::Indented);
        return json_list[index];
        });

    watcher->setFuture(future);
    return 0;
}

int Music_DataBase::updateItemInfo(QString path)
{
    return updateItemInfo(findItemInfo(path));
}

int Music_DataBase::addItemInfo(QString path)
{
    int info_id = YT_DataBase::addItemInfo(path);
    itemInfo_Hash[info_id].fromMusicFile(path);
    return info_id;
}

int Music_DataBase::eraseListInfo(QString info_id)
{
    if (info_id == playListInfoID)
        return -2;
    if (info_id == defaultListInfoID)
        return -1;

    int sic = YT_DataBase::eraseListInfo(info_id);
    if (listInfo_Hash.count() == 1)
    {
        listInfo_Hash.insert(defaultListInfoID, Music_ListInfo{ defaultListInfoID, QList<int>() });
    }
    return sic;
}

// QList<int> Music_DataBase::getItemInfo_KMP(const QString &kmp_data, const QList<QString> &kmp_indexs)
// {
//     QList<int> list_;
//     for(const auto &info : itemInfo_Hash){
//         for(const auto &kmp_index : kmp_indexs){
//             const auto &array = info.Tags[kmp_index].toArray();
//             for(const auto &array_it : array){
//                 if(KMP_String(array_it.toString(""), kmp_data) < 0) continue;
//                 list_.append(info.ID);
//             }
//         }
//     }
//     return list_;
// }

// QList<int> Music_DataBase::getItemInfo_KMP(const QString &kmp_data, const QList<QString> &kmp_indexs, int info_id)
// {
//     if (isListInfo(info_id) == false) return {};

//     QList<int> list_;
//     const auto &ID_List = listInfo_Hash[info_id].ID_List;
//     for(const auto &id : ID_List){
//         const auto &info = itemInfo_Hash[id];
//         for(const auto &kmp_index : kmp_indexs){
//             const auto &array = info.Tags[kmp_index].toArray();
//             for(const auto &array_it : array){
//                 if(KMP_String(array_it.toString(""), kmp_data) < 0) continue;
//                 list_.append(info.ID);
//             }
//         }
//     }
//     return list_;
// }

// QList<int> Music_DataBase::getItemInfo_Filter(const QString &data, const QList<QString> &indexs, const QList<int> &ID_List)
// {
//     QList<int> list_;
//     for (const auto &id : ID_List) {
//         const auto &info = itemInfo_Hash[id];
//         for (const auto &index : indexs) {
//             const auto &array = info.Tags[index].toArray();
//             for (const auto &array_it : array) {
//                 if(KMP_String(array_it.toString(""), data) < 0) continue;
//                 // if (array_it.toString("") != data) continue;
//                 list_.append(info.ID);
//             }
//         }
//     }
//     return list_;
// }
