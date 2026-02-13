#pragma once

#include <DB_DataBase.h>
#include <MusicDataInfo.h>

class MusicDatabase : public DB_Database<MusicFileInfo, MusicListInfo>
{
    MusicDatabase();
    ~MusicDatabase();
public:
    static MusicDatabase& obj();

    int updateFileInfo();
    int updateFileInfo(int info_id);
    int updateFileInfo(QString path);

    QList<int> playFileInfoIds = {};
    const QString playFileInfoIdsName = "播放列表";
protected:
    QHash<int, QString> updateFileInfo_Path = {};
    QHash<int, QFutureWatcher<QJsonObject>*> updateFileInfo_Future = {};
};
