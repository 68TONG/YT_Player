#include "MusicDatabase.h"

static const QStringList MusicDatabase_isMusicInfoIndex = {
    MusicFileInfo::I_MusicTag,
    MusicFileInfo::I_StarLevel,
    MusicFileInfo::I_PlayCount,
    MusicFileInfo::I_AddPlayCount
};

MusicDatabase::MusicDatabase()
    : DB_Database<MusicFileInfo, MusicListInfo>()
{
    if (getListInfo(playListInfo_Name, MusicListInfo::I_IsValid).toBool() == false) {
        addListInfo(MusicListInfo(playListInfo_Name));
    }
}

MusicDatabase::MusicDatabase(QThread* thread)
    : DB_Database<MusicFileInfo, MusicListInfo>(thread)
{}

MusicDatabase::~MusicDatabase()
{
    {}
}

void MusicDatabase::initMusicFileInfo(MusicFileInfo& info, QSqlQuery& query)
{
    info.musicTag = QJsonDocument::fromJson(query.value(MusicFileInfo::I_MusicTag).toByteArray()).object();
    info.starLevel = query.value(MusicFileInfo::I_StarLevel).toInt();
    info.playCount = query.value(MusicFileInfo::I_PlayCount).toInt();
    info.addPlayCount = query.value(MusicFileInfo::I_AddPlayCount).toInt();
}

QList<QString> MusicDatabase::getMusicFileInfo_TagKeys(QString list_id)
{
    auto&& info_ids = getListInfo_FileIds(list_id);
    return getMusicFileInfo_TagKeys(info_ids);
}

QList<QString> MusicDatabase::getMusicFileInfo_TagKeys(const QList<int>& info_ids)
{
    TempIdsTable temp_table(db, info_ids);
    QString sql = QString(R"(
            SELECT DISTINCT json_each.key
            FROM %1 AS MF_Info
            JOIN %2 AS T_Ids ON MF_Info.Id = T_Ids.Id
            CROSS JOIN json_each(MF_Info.musicTag)
        )").arg(DB::MusicFileInfo).arg(temp_table.tableName);

    QSqlQuery query(db);
    query.prepare(sql);
    if (query.exec() == false) {
        qDebug() << "DB_Database getFileInfo_TagKeys failed - info_ids:" << info_ids
            << "error:" << query.lastError().text();
        return {};
    }

    QList<QString> ret_datas;
    while (query.next()) {
        ret_datas.append(query.value(0).toString());
    }
    return ret_datas;
}

QList<QString> MusicDatabase::getMusicFileInfo_TagValues(QString list_id, QString json_path)
{
    auto&& info_ids = getListInfo_FileIds(list_id);
    return getMusicFileInfo_TagValues(info_ids, json_path);
}

QList<QString> MusicDatabase::getMusicFileInfo_TagValues(const QList<int>& info_ids, QString json_path)
{
    TempIdsTable temp_table(db, info_ids);
    QString sql = QString(R"(
            SELECT DISTINCT MF_Tag.value
            FROM %1 AS MF_Info
            CROSS JOIN %2 AS T_Ids ON MF_Info.Id = T_Ids.Id
            CROSS JOIN json_tree(MF_Info.musicTag, :json_path) AS MF_Tag ON MF_Tag.type = 'text'
        )").arg(DB::MusicFileInfo).arg(temp_table.tableName);

    QSqlQuery query(db);
    query.prepare(sql);
    query.bindValue(":json_path", json_path.isEmpty() ? "$" : "$." + json_path);
    if (query.exec() == false) {
        qDebug() << "DB_Database getFileInfo_TagValues failed - info_ids:" << info_ids
            << "error:" << query.lastError().text();
        return {};
    }

    QList<QString> ret_datas;
    while (query.next()) {
        ret_datas.append(query.value(0).toString());
    }
    return ret_datas;
}

QList<int> MusicDatabase::searchMusicFileInfo(QString list_id, QString search_tag, QString json_path)
{
    auto&& info_ids = getListInfo_FileIds(list_id);
    return searchMusicFileInfo(info_ids, search_tag, json_path);
}

QList<int> MusicDatabase::searchMusicFileInfo(const QList<int>& info_ids, QString search_tag, QString json_path)
{
    TempIdsTable temp_table(db, info_ids);
    QString sql = QString(R"(
            SELECT DISTINCT MF_Info.Id AS Id
            FROM %1 AS MF_Info
            CROSS JOIN %2 AS T_Ids ON MF_Info.Id = T_Ids.Id
            CROSS JOIN json_tree(MF_Info.musicTag, :json_path) AS MF_Tag ON MF_Tag.type = 'text'
            WHERE MF_Tag.value GLOB :search_tag
        )").arg(DB::MusicFileInfo).arg(temp_table.tableName);

    // QString sql = QString(R"(
    //     SELECT DISTINCT F_Info.Id AS Id
    //     FROM %1 AS F_Info
    //         JOIN
    //         %2 AS T_Ids ON F_Info.Id = T_Ids.Id
    //         CROSS JOIN
    //         json_tree(F_Info.fileTag, :json_path) AS F_Tag ON F_Tag.type = 'text'
    //         CROSS JOIN
    //         json_each(:search_tag) AS S_Tag
    //     WHERE F_Tag.value GLOB S_Tag.value;
    // )").arg(DB::FileInfo).arg(temp_table.tableName);

    QSqlQuery query(db);
    query.prepare(sql);
    query.bindValue(":search_tag", search_tag);
    query.bindValue(":json_path", json_path.isEmpty() ? "$" : "$." + json_path);
    if (query.exec() == false) {
        qDebug() << "MusicDatabase searchMusicFileInfo failed - search_tag:" << search_tag << " json_path: " << json_path
            << "error:" << query.lastError().text();
        return {};
    }

    QList<int> result_info_ids;
    while (query.next()) {
        result_info_ids.append(query.value("Id").toInt());
    }
    return result_info_ids;
}

MusicFileInfo MusicDatabase::getFileInfo(int info_id)
{
    auto info = DB_Database<MusicFileInfo, MusicListInfo>::getFileInfo(info_id);
    if (info.Id < 0) return {};

    QString sql = QString("SELECT musicTag, starLevel, playCount, addPlayCount FROM %1 WHERE Id = ?")
        .arg(DB::MusicFileInfo);

    QSqlQuery query(db);
    query.prepare(sql);
    query.addBindValue(info_id);
    if (query.exec() == false || query.next() == false) {
        qDebug() << "DB_Database getFileInfo failed - info_id:" << info_id
            << "error:" << query.lastError().text();
        return {};
    }

    initMusicFileInfo(info, query);
    return info;
}

QVariant MusicDatabase::getFileInfo(int info_id, QString index)
{
    if (MusicDatabase_isMusicInfoIndex.contains(index) == false) {
        return DB_Database<MusicFileInfo, MusicListInfo>::getFileInfo(info_id, index);
    }

    QString sql = QString("SELECT %1 FROM %2 WHERE Id = ?")
        .arg(index)
        .arg(DB::MusicFileInfo);

    QSqlQuery query(db);
    query.prepare(sql);
    query.addBindValue(info_id);
    if (query.exec() == false || query.next() == false) {
        qDebug() << "DB_Database getFileInfo failed - info_id:" << info_id
            << "index:" << index << "error:" << query.lastError().text();
        return {};
    }
    return query.value(0);
}

QHash<int, MusicFileInfo> MusicDatabase::getFileInfo(const QList<int>& info_ids)
{
    auto info_list = DB_Database<MusicFileInfo, MusicListInfo>::getFileInfo(info_ids);
    for (auto& info : info_list) {
        if (info.Id < 0) continue;

        QString sql = QString("SELECT musicTag, starLevel, playCount, addPlayCount FROM %1 WHERE Id = ?")
            .arg(DB::MusicFileInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(info.Id);
        if (query.exec() == false || query.next() == false) {
            qDebug() << "DB_Database getFileInfo failed - info_id:" << info.Id
                << "error:" << query.lastError().text();
            continue;
        }

        initMusicFileInfo(info, query);
    }
    return info_list;
}

int MusicDatabase::addFileInfo(const MusicFileInfo& info)
{
    int info_id = DB_Database<MusicFileInfo, MusicListInfo>::addFileInfo(info);
    if (info_id < 0) return info_id;

    QString sql = QString("INSERT INTO %1 (Id, musicTag, starLevel, playCount, addPlayCount) VALUES (?, ?, ?, ?, ?)")
        .arg(DB::MusicFileInfo);

    QSqlQuery query(db);
    query.prepare(sql);
    query.addBindValue(info_id);
    query.addBindValue(QJsonDocument(info.musicTag).toJson(QJsonDocument::Indented));
    query.addBindValue(info.starLevel);
    query.addBindValue(info.playCount);
    query.addBindValue(info.addPlayCount);
    if (query.exec() == false) {
        qDebug() << "MusicDatabase addFileInfo failed - path:" << info.filePath
            << "error:" << query.lastError().text();
        return info_id;
    }
    return info_id;
}

int MusicDatabase::setFileInfo(int info_id, QString index, const QVariant& data)
{
    if (MusicDatabase_isMusicInfoIndex.contains(index) == false) {
        return DB_Database<MusicFileInfo, MusicListInfo>::setFileInfo(info_id, index, data);
    }

    QString sql = QString("UPDATE %1 SET %2 = :data WHERE Id = :info_id")
        .arg(DB::MusicFileInfo)
        .arg(index);

    QSqlQuery query(db);
    query.prepare(sql);
    query.bindValue(":data", data);
    query.bindValue(":info_id", info_id);
    if (query.exec() == false) {
        qDebug() << "MusicDatabase setMusicFileInfo failed - info_id:" << info_id
            << "index:" << index << "data:" << data << "error:" << query.lastError().text();
        return -1;
    }
    return 0;
}

int MusicDatabase::sortFileInfo(QList<int>& info_ids, QString sort_tag)
{
    if (sort_tag == DB_ListInfo::I_ReverseSortTag) {
        std::reverse(info_ids.begin(), info_ids.end());
        return 0;
    }

    TempIdsTable temp_table(db, info_ids);
    QString sql = QString(R"(
        SELECT 
            MF_Info.Id AS Id,
            CASE WHEN MF_Tag.type = 'object'
                THEN '' 
                WHEN MF_Tag.type = 'array'
                THEN json_extract(MF_Tag.value, '$[0]')
                ELSE MF_Tag.value 
            END AS TagValue
        FROM %1 AS MF_Info
        CROSS JOIN %2 AS T_Ids ON MF_Info.Id = T_Ids.Id
        CROSS JOIN json_each(MF_Info.musicTag) AS MF_Tag
        WHERE MF_Tag.key = :sort_tag
    )").arg(DB::MusicFileInfo).arg(temp_table.tableName);

    QSqlQuery query(db);
    query.prepare(sql);
    query.bindValue(":sort_tag", sort_tag);
    if (query.exec() == false) {
        qDebug() << "MusicDatabase sortFileInfo failed - sort_tag:" << sort_tag
            << "error:" << query.lastError().text();
        return -1;
    }

    QHash<int, QString> sort_container;
    while (query.next()) {
        auto data = YT_Func::getObject().getChinesePinYin(query.value("TagValue").toString());
        sort_container.insert(query.value("Id").toInt(), data);
    }

    std::stable_sort(info_ids.begin(), info_ids.end(), [this, sort_container](const int& v1, const int& v2) -> bool
        { return QString::compare(sort_container[v1], sort_container[v2], Qt::CaseInsensitive) < 0; });
    return 0;
}

QList<QString> MusicDatabase::getListInfo_NameAll()
{
    return DB_Database<MusicFileInfo, MusicListInfo>::getListInfo_NameAll(DB_ListInfo::I_Type_Music);
}

MusicListInfo MusicDatabase::getPlayListInfo()
{
    return getListInfo(playListInfo_Name);
}
