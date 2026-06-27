#pragma once

#include <DB_DataBase.h>
#include <MusicDataInfo.h>

class MusicDatabase : public DB_Database<MusicFileInfo, MusicListInfo>
{
    MusicDatabase();
    ~MusicDatabase();
public:
    static MusicDatabase& obj();

    inline static const QString playListInfo_Name = "播放列表";
    MusicListInfo getPlayListInfo();
};
