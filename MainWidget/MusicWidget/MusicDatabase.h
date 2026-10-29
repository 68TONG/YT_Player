#pragma once

#include "DB_DataBase.h"
#include "MusicDataInfo.h"

class MusicDatabase : public DB_Database<MusicFileInfo, MusicListInfo>
{
    friend MusicDatabase& DB::object<MusicDatabase>();
    friend MusicDatabase& DB::objectForThread<MusicDatabase>();
    
    MusicDatabase();
    explicit MusicDatabase(QThread* thread);
public:
    virtual ~MusicDatabase();

    void initMusicFileInfo(MusicFileInfo& info, QSqlQuery& query);

    QList<QString> getMusicFileInfo_TagKeys(QString list_id);
    QList<QString> getMusicFileInfo_TagKeys(const QList<int>& info_ids);
    QList<QString> getMusicFileInfo_TagValues(QString list_id, QString json_path = "");
    QList<QString> getMusicFileInfo_TagValues(const QList<int>& info_ids, QString json_path = "");

    QList<int> searchMusicFileInfo(QString list_id, QString search_tag, QString json_path = "");
    QList<int> searchMusicFileInfo(const QList<int>& info_ids, QString search_tag, QString json_path = "");

    using DB_Database::getFileInfo;
    virtual MusicFileInfo getFileInfo(int info_id) override;
    virtual QVariant getFileInfo(int info_id, QString index) override;
    virtual QHash<int, MusicFileInfo> getFileInfo(const QList<int>& info_ids) override;

    using DB_Database::addFileInfo;
    virtual int addFileInfo(const MusicFileInfo& info) override;

    using DB_Database::setFileInfo;
    virtual int setFileInfo(int info_id, QString index, const QVariant& data) override;

    using DB_Database::sortFileInfo;
    virtual int sortFileInfo(QList<int>& info_ids, QString sort_tag) override;

    virtual QList<QString> getListInfo_NameAll();

    MusicListInfo getPlayListInfo();
    inline static const QString playListInfo_Name = "音乐播放列表";
};
