#include "Music_DataInfo.h"

#include "MediaDecoder.h"

Music_ItemInfo::Music_ItemInfo()
{

}

Music_ItemInfo::Music_ItemInfo(int ID, QString Path)
{
    this->ID = ID;
    fromMusicFile(Path);
}

QJsonObject Music_ItemInfo::toJsonObject()
{
    auto &&json = YT_ItemInfo::toJsonObject();
    json.insert(PROPERTY_MusicData, musicData);
    return json;
}

void Music_ItemInfo::fromJsonObject(const QJsonObject &json)
{
    YT_ItemInfo::fromJsonObject(json);
    musicData = json.value(PROPERTY_MusicData).toObject();
}

int Music_ItemInfo::fromMusicFile(const QString &path)
{
    MediaDecoder decoder;
    int sic = decoder.initMediaPath(path.toStdString().c_str());
    if (sic < 0) return sic;

    AVDictionaryEntry *dictionary = NULL;
    AVFormatContext *format_ctx = decoder.getFormat_ctx();

    dictionary = av_dict_get(format_ctx->metadata, Media::MetaData_YT_Data, NULL, AV_DICT_IGNORE_SUFFIX);
    if (dictionary != NULL) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(dictionary->value);
        if (jsonDoc.isObject()) {
            musicData = jsonDoc.object();
        }
    }

    // while ((dictionary = av_dict_get(format_ctx->metadata, "", dictionary, AV_DICT_IGNORE_SUFFIX)))
    // {
    //     qDebug() << dictionary->key << dictionary->value;
    // }

    // if (musicData.isEmpty()) {
    //     dictionary = av_dict_get(format_ctx->metadata, Media::MetaData_YT_Data, NULL, AV_DICT_IGNORE_SUFFIX);
    //     if (dictionary != NULL) {
    //         QJsonDocument jsonDoc = QJsonDocument::fromJson(dictionary->value);
    //         if (jsonDoc.isObject()) {
    //             musicData = jsonDoc.object();
    //         }
    //     }
    // }

    if (musicData.find(Media::MetaData_Title) == musicData.end() || musicData[Media::MetaData_Title].isNull()) {
        dictionary = av_dict_get(format_ctx->metadata, Media::MetaData_Title, NULL, AV_DICT_IGNORE_SUFFIX);
        if (dictionary != NULL) {
            musicData[Media::MetaData_Title] = QJsonArray({dictionary->value});
        }
    }
    if (musicData.find(Media::MetaData_Album) == musicData.end() || musicData[Media::MetaData_Album].isNull()) {
        dictionary = av_dict_get(format_ctx->metadata, Media::MetaData_Album, NULL, AV_DICT_IGNORE_SUFFIX);
        if (dictionary != NULL) {
            musicData[Media::MetaData_Album] = QJsonArray({dictionary->value});
        }
    }
    if (musicData.find(Media::MetaData_Artist) == musicData.end() || musicData[Media::MetaData_Artist].isNull()) {
        dictionary = av_dict_get(format_ctx->metadata, Media::MetaData_Artist, NULL, AV_DICT_IGNORE_SUFFIX);
        if (dictionary != NULL) {
            musicData[Media::MetaData_Artist] = QJsonArray({dictionary->value});
        }
    }

    Path = path;
    // this->duration = format->duration;

    return av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
}

QByteArray Music_ItemInfo::getMusicLyrics() const
{
    return getMusicLyrics(Path);
}

QByteArray Music_ItemInfo::getMusicLyrics(const QString &path)
{
    // if(path.isEmpty()) return QString();

    // MediaDecoder decoder;
    // int sic = decoder.initMediaPath(path.toStdString().c_str());
    // if(sic < 0) return QString();

    // AVDictionaryEntry *dictionary = NULL;
    // AVFormatContext *format = decoder.getFormat_ctx();
    // dictionary = av_dict_get(format->metadata, METADATA_LYRICS, NULL, AV_DICT_IGNORE_SUFFIX);
    // if(dictionary == NULL) return QString();
    // return dictionary->value;
    return "";
}

Music_ListInfo::Music_ListInfo()
{

}

Music_ListInfo::Music_ListInfo(int ID, QString Name, QList<int> ID_List)
    : YT_ListInfo(ID, Name, ID_List)
{

}

QJsonObject Music_ListInfo::toJsonObject()
{
    auto &&json = YT_ListInfo::toJsonObject();

    QJsonArray json_musicHeader;
    for (auto &&it : musicHeader) json_musicHeader.push_back(QJsonValue(it));
    json.insert(PROPERTY_MusicHeader, json_musicHeader);
    return json;
}

void Music_ListInfo::fromJsonObject(const QJsonObject &json)
{
    YT_ListInfo::fromJsonObject(json);

    QJsonArray json_musicHeader = json.value(PROPERTY_MusicHeader).toArray();
    for(auto &&it : json_musicHeader) musicHeader.push_back(it.toString());
}
