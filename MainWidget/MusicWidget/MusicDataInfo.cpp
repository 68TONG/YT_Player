#include "MusicDataInfo.h"

MusicFileInfo::MusicFileInfo()
    :DB_FileInfo()
{
    {}
}

MusicFileInfo::MusicFileInfo(QString filePath)
    : DB_FileInfo(filePath)
{
    getMusicMetaData(filePath, musicTag);
    fileTitle = musicTag[Media::MetaData_Title].toString();
}

void MusicFileInfo::getMusicMetaData(const QString& path, QJsonObject& data)
{
    MediaDecoder decoder;
    int sic = decoder.initMediaPath(path.toStdString().c_str());
    if (sic < 0)
        return;

    getMusicMetaData(decoder.getFormat_ctx()->metadata, data);
}

void MusicFileInfo::getMusicMetaData(AVDictionary* metadata, QJsonObject& data)
{
    AVDictionaryEntry* dictionary = nullptr;

    data = QJsonObject();
    dictionary = av_dict_get(metadata, Media::MetaData_YT_Data, nullptr, AV_DICT_IGNORE_SUFFIX);
    if (dictionary != nullptr)
    {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(dictionary->value);
        if (jsonDoc.isObject())
        {
            data = jsonDoc.object();
        }
    }

    if (data.find(Media::MetaData_Title) == data.end() || data[Media::MetaData_Title].isNull())
    {
        dictionary = av_dict_get(metadata, Media::MetaData_Title, nullptr, AV_DICT_IGNORE_SUFFIX);
        if (dictionary != nullptr)
        {
            data[Media::MetaData_Title] = dictionary->value;
        }
    }
    if (data.find(Media::MetaData_Album) == data.end() || data[Media::MetaData_Album].isNull())
    {
        dictionary = av_dict_get(metadata, Media::MetaData_Album, nullptr, AV_DICT_IGNORE_SUFFIX);
        if (dictionary != nullptr)
        {
            data[Media::MetaData_Album] = dictionary->value;
        }
    }
    if (data.find(Media::MetaData_Artist) == data.end() || data[Media::MetaData_Artist].isNull())
    {
        dictionary = av_dict_get(metadata, Media::MetaData_Artist, nullptr, AV_DICT_IGNORE_SUFFIX);
        if (dictionary != nullptr)
        {
            data[Media::MetaData_Artist] = QJsonArray({ dictionary->value });
        }
    }
}

MusicListInfo::MusicListInfo()
{
    {}
}

MusicListInfo::MusicListInfo(QString name)
    : DB_ListInfo(name, I_Type_Music)
{
    {}
}
