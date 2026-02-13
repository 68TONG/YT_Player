#pragma once

#include <DB_Info.h>

template <typename FileInfo_T = DB_FileInfo, typename ListInfo_T = DB_ListInfo>
class DB_Database
{
    static_assert(std::is_base_of<DB_FileInfo, FileInfo_T>::value, "FileInfo_T must inherit from Base");
    static_assert(std::is_base_of<DB_ListInfo, ListInfo_T>::value, "ListInfo_T must inherit from Base");
protected:
    DB_Database(QString fileInfoTableName, QString listInfoTableName)
        : fileInfoTableName(fileInfoTableName)
        , listInfoTableName(listInfoTableName)
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("C:/TONG/Code/YT_Player/YT_Database/YT_Database.db");
        if (db.open() == false) {
            qDebug() << "DB_Database: connection with database failed";
        }

        // QSqlQuery query(db);
        // query.exec("PRAGMA foreign_keys = ON");
        // query.exec("PRAGMA foreign_keys");
        // if (query.next() && query.value(0).toInt() == 1) {
        //     qDebug() << "外键约束已启用";
        // }
        // else {
        //     qWarning() << "警告：外键约束未能启用";
        // }
    }
    ~DB_Database() {}

    class TempIdsTable
    {
    public:
        TempIdsTable(const QList<int>& ids = {}, QString tableName = "tempIdsTable")
            :tableName(tableName)
        {
            if (QSqlDatabase::database().transaction() == false) {
                qDebug() << "DB_Database TempIdsTable database transaction error:";
            }
            if (query.exec("CREATE TEMP TABLE IF NOT EXISTS " + tableName + " (ID INT PRIMARY KEY)") == false) {
                qDebug() << "DB_Database TempIdsTable CREATE TABLE error:" << query.lastError().text();
            }
            addIds(ids);
        }
        ~TempIdsTable()
        {
            if (query.exec("DROP TABLE IF EXISTS " + tableName) == false) {
                qDebug() << "DB_Database TempIdsTable DROP TABLE error:" << query.lastError().text();
            }
            if (QSqlDatabase::database().commit() == false) {
                qDebug() << "DB_Database TempIdsTable database commit error:";
            }
        }
        void addIds(const QList<int>& ids) {
            query.prepare(QString("INSERT INTO %1 (ID) VALUES (?)").arg(tableName));
            for (int id : ids) {
                query.addBindValue(id);
                query.exec();
            }
        }
        QSqlQuery query;
        const QString tableName;
    };
public:
    virtual FileInfo_T getFileInfo(int info_id)
    {
        QString sql = QString("SELECT * FROM %1 WHERE ID = %2")
            .arg(fileInfoTableName)
            .arg(info_id);

        QSqlQuery query;
        query.prepare(sql);
        if (query.exec() == false || query.next() == false) {
            qDebug() << "DB_Database getFileInfo failed - info_id:" << info_id
                << "error:" << query.lastError().text();
            return {};
        }
        return FileInfo_T(query);
    }

    virtual QVariant getFileInfo(int info_id, QString index)
    {
        QString sql = QString("SELECT %1 FROM %2 WHERE ID = ?")
            .arg(index)
            .arg(fileInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(info_id);
        if (query.exec() == false || query.next() == false) {
            qDebug() << "DB_Database getFileInfo failed - info_id:" << info_id
                << "index:" << index << "error:" << query.lastError().text();
            return {};
        }
        return query.value(0);
    }

    virtual QHash<int, FileInfo_T> getFileInfo(const QList<int>& info_ids)
    {
        TempIdsTable temp_table(info_ids);
        QString sql = QString(R"(
            SELECT * FROM %1 AS F_Info
            JOIN %2 AS T_Ids ON F_Info.ID = T_Ids.ID
        )").arg(fileInfoTableName).arg(temp_table.tableName);

        QSqlQuery query;
        query.prepare(sql);
        if (query.exec() == false) {
            qDebug() << "DB_Database getFileInfo failed - info_ids:" << info_ids
                << "error:" << query.lastError().text();
            return {};
        }

        QHash<int, FileInfo_T> info_list;
        while (query.next()) {
            FileInfo_T info(query);
            info_list.insert(info.ID, info);
        }
        return info_list;
    }

    virtual int getFileInfo_ID(QString path)
    {
        QString sql = QString("SELECT ID FROM %1 WHERE Path = ?")
            .arg(fileInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(path);
        if (query.exec() == false || query.next() == false) {
            // qDebug() << "DB_Database getFileInfo_ID failed - path:" << path << "error:" << query.lastError().text();
            return -1;
        }
        return query.value(0).toInt();
    }

    virtual QList<QString> getFileInfo_TagKeys(QString list_id)
    {
        auto info_ids = getListInfo_FileIds(list_id);
        return getFileInfo_TagKeys(info_ids);
    }

    virtual QList<QString> getFileInfo_TagKeys(const QList<int>& info_ids)
    {
        TempIdsTable temp_table(info_ids);
        QString sql = QString(R"(
            SELECT DISTINCT json_each.key
            FROM %1 AS F_Info
            JOIN %2 AS T_Ids ON F_Info.ID = T_Ids.ID
            CROSS JOIN json_each(F_Info.Tags)
        )").arg(fileInfoTableName).arg(temp_table.tableName);

        QSqlQuery query;
        query.prepare(sql);
        QList<QString> tag_keys;
        if (query.exec() == false) {
            qDebug() << "DB_Database getFileInfo_TagKeys failed - info_ids:" << info_ids
                << "error:" << query.lastError().text();
            return tag_keys;
        }
        while (query.next()) {
            tag_keys.append(query.value(0).toString());
        }
        return tag_keys;
    }

    virtual QList<QString> getFileInfo_TagValues(QString list_id, QString tag_key)
    {
        auto info_ids = getListInfo_FileIds(list_id);
        return getFileInfo_TagValues(info_ids, tag_key);
    }

    virtual QList<QString> getFileInfo_TagValues(const QList<int>& info_ids, QString tag_key)
    {
        TempIdsTable temp_table(info_ids);
        QString sql = QString(R"(
            SELECT DISTINCT json_tree.value
            FROM %1 AS F_Info
            JOIN %2 AS T_Ids ON F_Info.ID = T_Ids.ID
            CROSS JOIN json_tree(F_Info.Tags, :tag_key)
        )").arg(fileInfoTableName).arg(temp_table.tableName);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":tag_key", "$." + tag_key);
        if (query.exec() == false) {
            qDebug() << "DB_Database getFileInfo_TagValues failed - info_ids:" << info_ids
                << "error:" << query.lastError().text();
            return {};
        }

        QList<QString> tag_values;
        while (query.next()) {
            tag_values.append(query.value(0).toString());
        }
        return tag_values;
    }

    virtual int addFileInfo(QString path)
    {
        auto info_path = DB::canonicalPath(path);
        int info_id = getFileInfo_ID(path);
        if (info_id >= 0) return info_id;

        FileInfo_T info(info_path);
        return addFileInfo(info);
    }

    virtual int addFileInfo(FileInfo_T& info)
    {
        QSqlQuery query;
        query.prepare(info.sqlQuery());
        const auto sql_data = info.toSqlData();
        for (const auto& it : sql_data) {
            query.addBindValue(it);
        }

        if (query.exec() == false) {
            qDebug() << "DB_Database addFileInfo failed - path:" << info.Path
                << "error:" << query.lastError().text();
            return -1;
        }
        return query.lastInsertId().toInt();
    }

    virtual int setFileInfo(int info_id, QString index, const QVariant& data)
    {
        QString sql = QString("UPDATE %1 SET %2 = :data WHERE ID = :info_id")
            .arg(fileInfoTableName)
            .arg(index);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":data", data);
        query.bindValue(":info_id", info_id);
        if (query.exec() == false) {
            qDebug() << "DB_Database setFileInfo failed - info_id:" << info_id
                << "index:" << index << "data:" << data << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual int deleteFileInfo(int info_id)
    {
        QString sql = QString("DELETE FROM %1 WHERE ID = %2")
            .arg(fileInfoTableName)
            .arg(info_id);

        QSqlQuery query;
        query.prepare(sql);
        if (query.exec() == false) {
            qDebug() << "DB_Database deleteFileInfo failed - info_id:" << info_id
                << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual void sortFileInfo(QString list_id, QString sort_tag)
    {
        auto info_ids = getListInfo_FileIds(list_id);
        sortFileInfo(info_ids, sort_tag);
        QString sql = QString(R"(
            UPDATE %1 
            SET 
                SortTag = COALESCE(:sort_tag, SortTag),
                ReverseSortTag = 
                    CASE WHEN :reverse_sort_tag = 1 THEN (~ReverseSortTag & 1)
                    ELSE 0 END,
                FileInfoIds = json(:file_ids) 
            WHERE Name = :list_id
        )").arg(listInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":list_id", list_id);
        query.bindValue(":sort_tag", sort_tag != "ID" ? sort_tag : QVariant());
        query.bindValue(":reverse_sort_tag", sort_tag == "ID");
        query.bindValue(":file_ids", DB::listToJsonByte<int>(info_ids));
        if (query.exec() == false)
        {
            qDebug() << "DB_Database sortListInfo_FileIds failed - list_id:" << list_id
                << "error:" << query.lastError().text();
            return;
        }
    }

    virtual void sortFileInfo(QList<int>& info_ids, QString sort_tag)
    {
        if (sort_tag == "ID")
        {
            std::reverse(info_ids.begin(), info_ids.end());
            return;
        }

        TempIdsTable temp_table(info_ids);
        QString sql = QString(R"(
            SELECT 
                F_Info.ID AS ID,
                CASE WHEN json_each.type = 'object'
                THEN '' 
                WHEN json_each.type = 'array'
                THEN json_extract(json_each.value, '$[0]')
                ELSE json_each.value 
                END AS TagValue
            FROM %1 AS F_Info
            JOIN %2 AS T_Ids ON F_Info.ID = T_Ids.ID
            CROSS JOIN json_each(F_Info.Tags)
            WHERE json_each.key = :sort_tag
        )").arg(fileInfoTableName).arg(temp_table.tableName);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":sort_tag", sort_tag);
        if (query.exec() == false) {
            qDebug() << "DB_Database sortListInfo_FileIds failed - sort_tag:" << sort_tag
                << "error:" << query.lastError().text();
            return;
        }

        QHash<int, QString> sort_container;
        while (query.next()) {
            auto data = YT_Func::getObject().getChinesePinYin(query.value("TagValue").toString());;
            sort_container.insert(query.value("ID").toInt(), data);
        }
        std::stable_sort(info_ids.begin(), info_ids.end(), [this, sort_container](const int& v1, const int& v2) -> bool
            { return QString::compare(sort_container[v1], sort_container[v2], Qt::CaseInsensitive) < 0; });
    }

    virtual QList<int> searchFileInfo(QString list_id, QString search_tag)
    {
        auto info_ids = getListInfo_FileIds(list_id);
        return searchFileInfo(info_ids, search_tag);
    }

    virtual QList<int> searchFileInfo(const QList<int>& info_ids, QString search_tag)
    {
        TempIdsTable temp_table(info_ids);
        QString sql = QString(R"(
            SELECT DISTINCT F_Info.ID AS ID
            FROM %1 AS F_Info
            JOIN %2 AS T_Ids ON F_Info.ID = T_Ids.ID
            CROSS JOIN json_tree(F_Info.Tags)
            WHERE json_tree.value = :search_tag
        )").arg(fileInfoTableName).arg(temp_table.tableName);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":search_tag", search_tag);
        QList<int> result_info_ids;
        if (query.exec() == false) {
            qDebug() << "DB_Database searchFileInfo failed - search_tag:" << search_tag
                << "error:" << query.lastError().text();
            return result_info_ids;
        }

        while (query.next()) {
            result_info_ids.append(query.value("ID").toInt());
        }
        return result_info_ids;
    }

    virtual ListInfo_T getListInfo(QString info_id)
    {
        QString sql = QString("SELECT * FROM %1 WHERE Name = ?")
            .arg(listInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(info_id);
        if (query.exec() == false || query.next() == false) {
            qDebug() << "DB_Database getListInfo failed - info_id:" << info_id
                << "error:" << query.lastError().text();
            return {};
        }
        return ListInfo_T(query);
    }

    virtual QVariant getListInfo(QString info_id, QString index)
    {
        QString sql = QString("SELECT %1 FROM %2 WHERE Name = ?")
            .arg(index)
            .arg(listInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(info_id);
        if (query.exec() == false || query.next() == false) {
            qDebug() << "DB_Database getListInfo failed - info_id:" << info_id
                << "index:" << index << "error:" << query.lastError().text();
            return {};
        }
        return query.value(0);
    }

    virtual QList<QString> getListInfo_NameAll()
    {
        QString sql = QString("SELECT Name FROM %1 ORDER BY SortIndex ASC")
            .arg(listInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        QList<QString> info_list;
        if (query.exec() == false) {
            qDebug() << "DB_Database getListInfo_NameAll failed"
                << "error:" << query.lastError().text();
            return info_list;
        }
        while (query.next()) {
            info_list.append(query.value(0).toString());
        }
        return info_list;
    }

    virtual QList<int> getListInfo_FileIds(QString info_id)
    {
        QString sql = QString("SELECT json_each.value FROM %1 AS L_Info, json_each(L_Info.FileInfoIds) WHERE L_Info.Name = ?")
            .arg(listInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(info_id);

        QList<int> info_ids;
        if (query.exec() == false) {
            qDebug() << "DB_Database getListInfo_FileIds failed - info_id:" << info_id
                << "error:" << query.lastError().text();
            return info_ids;
        }
        while (query.next()) {
            info_ids.append(query.value(0).toInt());
        }
        return info_ids;
    }

    virtual int addListInfo(ListInfo_T& info)
    {
        QSqlQuery query;
        query.prepare(info.sqlQuery());
        const auto sql_data = info.toSqlData();
        for (const auto& it : sql_data) {
            query.addBindValue(it);
        }

        if (query.exec() == false) {
            qDebug() << "DB_Database addListInfo failed - name:" << info.Name
                << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual int addListInfo_FileIds(QString list_id, int file_id)
    {
        QString sql = QString(R"(
            UPDATE %1 
            SET FileInfoIds = json_insert(
                COALESCE(FileInfoIds, '[]'), 
                '$[#]', 
                :file_id
            ) 
            WHERE Name = :list_id
            AND NOT EXISTS (
                SELECT 1 
                FROM json_each(FileInfoIds) 
                WHERE json_each.value = :file_id
            )
        )").arg(listInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":list_id", list_id);
        query.bindValue(":file_id", file_id);
        if (query.exec() == false) {
            qDebug() << "DB_Database addListInfo_FileIds failed "
                << "list_id:" << list_id << "file_id:" << file_id
                << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual int addListInfo_FileIds(QString list_id, QString path)
    {
        int file_id = addFileInfo(path);
        if (file_id < 0) return file_id;
        return addListInfo_FileIds(list_id, file_id);
    }

    virtual int setListInfo(QString info_id, QString index, const QVariant& data)
    {
        QString sql = QString("UPDATE %1 SET %2 = :data WHERE Name = :info_id")
            .arg(listInfoTableName)
            .arg(index);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":data", data);
        query.bindValue(":info_id", info_id);
        if (query.exec() == false) {
            qDebug() << "DB_Database setListInfo failed - info_id:" << info_id
                << "index:" << index << "data:" << data << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual int setListInfo_SortIndex(QString info_id, QString target_id)
    {
        int info_sort_index = getListInfo(info_id, "SortIndex").toInt();
        int target_sort_index = getListInfo(target_id, "SortIndex").toInt();

        QSqlQuery query;
        QSqlDatabase db = QSqlDatabase::database();
        if (db.transaction() == false) {
            qDebug() << "DB_Database setListInfo_SortIndex failed - info_id:" << info_id
                << "target_id:" << target_id << "error:" << db.lastError().text();
            return -1;
        }

        try {
            if (info_sort_index > target_sort_index) {
                query.prepare(QString(R"(
                    UPDATE %1 
                    SET SortIndex = SortIndex + 1
                    WHERE SortIndex >= :target AND SortIndex < :current
                )").arg(listInfoTableName));
                query.bindValue(":target", target_sort_index);
                query.bindValue(":current", info_sort_index);
            }
            else {
                query.prepare(QString(R"(
                    UPDATE %1 
                    SET SortIndex = SortIndex - 1
                    WHERE SortIndex > :current AND SortIndex <= :target
                )").arg(listInfoTableName));
                query.bindValue(":current", info_sort_index);
                query.bindValue(":target", target_sort_index);
            }
            if (query.exec() == false) {
                throw QString("更新中间记录失败: %1").arg(query.lastError().text());
            }

            query.prepare(QString("UPDATE %1 SET SortIndex = :idx WHERE Name = :id")
                .arg(listInfoTableName));
            query.bindValue(":idx", target_sort_index);
            query.bindValue(":id", info_id);
            if (query.exec() == false) {
                throw QString("更新目标记录失败: %1").arg(query.lastError().text());
            }

            if (db.commit() == false) {
                throw QString("提交事务失败: %1").arg(db.lastError().text());
            }
            return 0;
        }
        catch (const QString& error) {
            db.rollback();
            qDebug() << "DB_Database setListInfo_SortIndex failed - info_id:" << info_id
                << "target_id:" << target_id << "error:" << error;
            return -1;
        }
        return 0;
    }

    virtual int deleteListInfo(QString info_id)
    {
        QString sql = QString("DELETE FROM %1 WHERE Name = ?")
            .arg(listInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(info_id);
        if (query.exec() == false) {
            qDebug() << "DB_Database deleteListInfo failed - info_id:" << info_id
                << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual int deleteListInfo_FileIds(QString info_id, int index)
    {
        QString sql = QString("UPDATE %1 SET FileInfoIds = json_remove(FileInfoIds, '$[:index]') WHERE Name = :info_id")
            .arg(listInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":index", index);
        query.bindValue(":info_id", info_id);
        if (query.exec() == false)
        {
            qDebug() << "DB_Database deleteListInfo_FileIds failed - info_id:" << info_id
                << "index:" << index
                << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    const QString fileInfoTableName;
    const QString listInfoTableName;
};