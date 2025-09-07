#ifndef MUSIC_DATAINFO_H
#define MUSIC_DATAINFO_H

#include <YT_DataBase.h>

#define PROPERTY_MusicData "MusicData"
#define PROPERTY_MusicHeader "MusicHeader"

class Music_ItemInfo : public YT_ItemInfo {
public:
    Music_ItemInfo();
    Music_ItemInfo(int ID, QString Path);

    virtual QJsonObject toJsonObject() override;
    virtual void fromJsonObject(const QJsonObject &json_obj) override;
    int fromMusicFile(const QString &path);

    QByteArray getMusicLyrics() const;
    static QByteArray getMusicLyrics(const QString &path);

    QJsonObject musicData;
};

class Music_ListInfo : public YT_ListInfo {
public:
    Music_ListInfo();
    Music_ListInfo(int ID, QString Name, QList<int> ID_List);

    virtual QJsonObject toJsonObject() override;
    virtual void fromJsonObject(const QJsonObject &json_obj) override;

    QList<QString> musicHeader;
};

#endif // MUSIC_DATAINFO_H
