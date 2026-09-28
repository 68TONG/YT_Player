#pragma once

#include <DB_Info.h>
#include <DB_DataBase.h>
#include <MediaFormat.h>

class MusicFileInfo : public DB_FileInfo
{
public:
    MusicFileInfo();
    MusicFileInfo(QString filePath);

    static void getMusicMetaData(const QString& path, QJsonObject& data);
    static void getMusicMetaData(AVDictionary* metadata, QJsonObject& data);

    QJsonObject musicTag = {};
    int starLevel = 0;
    int playCount = 0;
    int addPlayCount = 0;

    inline static const QString I_MusicTag = "musicTag";
    inline static const QString I_StarLevel = "starLevel";
    inline static const QString I_PlayCount = "playCount";
    inline static const QString I_AddPlayCount = "addPlayCount";
};

class MusicListInfo : public DB_ListInfo
{
public:
    MusicListInfo();
    MusicListInfo(QString name);
};