#include "Music_DataInfo.h"

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
    auto&& json = YT_ItemInfo::toJsonObject();
    return json;
}

void Music_ItemInfo::fromJsonObject(const QJsonObject& json)
{
    YT_ItemInfo::fromJsonObject(json);
}

void Music_ItemInfo::fromMusicFile(const QString& path)
{
    MediaDecoder decoder;
    int sic = decoder.initMediaPath(path.toStdString().c_str());
    if (sic < 0) return;

    getMusicMetaData(decoder, Tags);
    Path = path;
    // this->duration = format->duration;
}

void Music_ItemInfo::getMusicMetaData(const QString& path, QJsonObject& data)
{
    MediaDecoder decoder;
    int sic = decoder.initMediaPath(path.toStdString().c_str());
    if (sic < 0)
        return;

    getMusicMetaData(decoder, data);
}

void Music_ItemInfo::getMusicMetaData(MediaDecoder& decoder, QJsonObject& data)
{
    AVDictionaryEntry* dictionary = NULL;
    AVFormatContext* format_ctx = decoder.getFormat_ctx();

    data = QJsonObject();
    dictionary = av_dict_get(format_ctx->metadata, Media::MetaData_YT_Data, NULL, AV_DICT_IGNORE_SUFFIX);
    if (dictionary != NULL)
    {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(dictionary->value);
        if (jsonDoc.isObject())
        {
            data = jsonDoc.object();
        }
    }

    if (data.find(Media::MetaData_Title) == data.end() || data[Media::MetaData_Title].isNull())
    {
        dictionary = av_dict_get(format_ctx->metadata, Media::MetaData_Title, NULL, AV_DICT_IGNORE_SUFFIX);
        if (dictionary != NULL)
        {
            data[Media::MetaData_Title] = dictionary->value;
        }
    }
    if (data.find(Media::MetaData_Album) == data.end() || data[Media::MetaData_Album].isNull())
    {
        dictionary = av_dict_get(format_ctx->metadata, Media::MetaData_Album, NULL, AV_DICT_IGNORE_SUFFIX);
        if (dictionary != NULL)
        {
            data[Media::MetaData_Album] = dictionary->value;
        }
    }
    if (data.find(Media::MetaData_Artist) == data.end() || data[Media::MetaData_Artist].isNull())
    {
        dictionary = av_dict_get(format_ctx->metadata, Media::MetaData_Artist, NULL, AV_DICT_IGNORE_SUFFIX);
        if (dictionary != NULL)
        {
            data[Media::MetaData_Artist] = QJsonArray({ dictionary->value });
        }
    }
}

QByteArray Music_ItemInfo::getMusicLyrics() const
{
    return getMusicLyrics(Path);
}

QByteArray Music_ItemInfo::getMusicLyrics(const QString& path)
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

Music_ListInfo::Music_ListInfo(QString Name, QList<int> ID_List)
    : YT_ListInfo(Name, ID_List)
{
}

QJsonObject Music_ListInfo::toJsonObject()
{
    auto&& json = YT_ListInfo::toJsonObject();
    return json;
}

void Music_ListInfo::fromJsonObject(const QJsonObject& json)
{
    YT_ListInfo::fromJsonObject(json);
}
