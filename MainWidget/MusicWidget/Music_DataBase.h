#ifndef MUSIC_DATABASE_H
#define MUSIC_DATABASE_H

#include <Music_DataInfo.h>
#include <QFuture>
#include <QFutureWatcher>

class Music_DataBase : public YT_DataBase<Music_ItemInfo, Music_ListInfo>
{
    Music_DataBase();
    ~Music_DataBase();
public:
    static Music_DataBase& getObject();
    Music_ListInfo& getPlayListInfo();

    int updateItemInfo();
    int updateItemInfo(int info_id);
    int updateItemInfo(QString path);

    virtual int addItemInfo(QString path) override;
    virtual int eraseListInfo(QString info_id) override;

    // QList<int> getItemInfo_KMP(const QString &kmp_data, const QList<QString> &kmp_indexs);
    // QList<int> getItemInfo_KMP(const QString &kmp_data, const QList<QString> &kmp_indexs, int info_id);
    // QList<int> getItemInfo_Filter(const QString &data, const QList<QString> &indexs, const QList<int> &ID_List);
    QHash<int, QString> updateFileInfo = {};
    QHash<int, QFutureWatcher<QJsonObject>*> updateFileInfo_Future = {};
    const QString playListInfoID = "播放列表";
    const QString defaultListInfoID = "默认列表";
};

#endif // MUSIC_DATABASE_H
