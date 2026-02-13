#include "MusicDataInfo.h"

#include "MusicDatabase.h"

MusicFileInfo::MusicFileInfo()
{
}

MusicFileInfo::MusicFileInfo(QString Path)
    : DB_FileInfo(Path)
{
    getMusicMetaData(Path, Tags);
}

MusicFileInfo::MusicFileInfo(QSqlQuery& query)
{
    MusicFileInfo::fromSqlData(query);
}

QString MusicFileInfo::sqlQuery()
{
    return QString("INSERT INTO %1 (%2, %3) VALUES (?, ?)")
        .arg(MusicDatabase::obj().fileInfoTableName)
        .arg(DB::I_FileInfo_Path)
        .arg(DB::I_FileInfo_Tags);
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
}

MusicListInfo::MusicListInfo(QString Name)
    : DB_ListInfo(Name)
{
}

MusicListInfo::MusicListInfo(QSqlQuery& query)
{
    MusicListInfo::fromSqlData(query);
}

QString MusicListInfo::sqlQuery()
{
    return QString("INSERT INTO %1 (%2, %3, %4, %5) VALUES (?, ?, ?, ?)")
        .arg(MusicDatabase::obj().listInfoTableName)
        .arg(DB::I_ListInfo_Name)
        .arg(DB::I_ListInfo_FileInfoIds)
        .arg(DB::I_ListInfo_SortTag)
        .arg(DB::I_ListInfo_SelectTag);
}
