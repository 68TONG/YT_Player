#ifndef MUSIC_DATAINFO_H
#define MUSIC_DATAINFO_H

#include <YT_DataBase.h>
#include <MediaDecoder.h>

class Music_ItemInfo : public YT_ItemInfo
{
public:
    Music_ItemInfo();
    Music_ItemInfo(int ID, QString Path);

    virtual QJsonObject toJsonObject() override;
    virtual void fromJsonObject(const QJsonObject &json_obj) override;

    void fromMusicFile(const QString &path);
    static void getMusicMetaData(const QString &path, QJsonObject &data);
    static void getMusicMetaData(MediaDecoder &decoder, QJsonObject &data);

    QByteArray getMusicLyrics() const;
    static QByteArray getMusicLyrics(const QString &path);
};

class Music_ListInfo : public YT_ListInfo
{
public:
    Music_ListInfo();
    Music_ListInfo(QString Name, QList<int> ID_List);

    virtual QJsonObject toJsonObject() override;
    virtual void fromJsonObject(const QJsonObject &json_obj) override;
};

#endif // MUSIC_DATAINFO_H
