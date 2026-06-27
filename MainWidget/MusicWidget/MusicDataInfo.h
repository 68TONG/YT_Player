#pragma once

#include <DB_DataBase.h>
#include <MediaFormat.h>

class MusicFileInfo : public DB_FileInfo
{
public:
    MusicFileInfo();
    MusicFileInfo(QString filePath);
    MusicFileInfo(QSqlQuery& query);

    virtual QString sqlQuery(QString table_name) const override;
    virtual QList<QVariant> toSqlData() const override;
    virtual void fromSqlData(QSqlQuery& query) override;

    static void getMusicMetaData(const QString& path, QJsonObject& data);
    static void getMusicMetaData(AVDictionary* metadata, QJsonObject& data);

    int starLevel = 0;
    int playCount = 0;
    int addPlayCount = 0;

    inline static const QString I_StarLevel = "starLevel";
    inline static const QString I_PlayCount = "playCount";
    inline static const QString I_AddPlayCount = "addPlayCount";
    inline static const QString I_CoverHashValue = "CoverHashValue";
};

class MusicListInfo : public DB_ListInfo
{
public:
    MusicListInfo();
    MusicListInfo(QString Name);
    MusicListInfo(QSqlQuery& query);
};