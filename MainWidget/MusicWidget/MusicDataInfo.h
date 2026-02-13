#pragma once

#include <DB_DataBase.h>
#include <MediaFormat.h>

class MusicFileInfo : public DB_FileInfo
{
public:
    MusicFileInfo();
    MusicFileInfo(QString Path);
    MusicFileInfo(QSqlQuery& query);

    virtual QString sqlQuery() override;
    // virtual QList<QVariant> toSqlData() override;
    // virtual void fromSqlData(QSqlQuery& query) override;

    static void getMusicMetaData(const QString& path, QJsonObject& data);
    static void getMusicMetaData(AVDictionary *metadata, QJsonObject& data);
};

class MusicListInfo : public DB_ListInfo
{
public:
    MusicListInfo();
    MusicListInfo(QString Name);
    MusicListInfo(QSqlQuery& query);

    virtual QString sqlQuery() override;
};