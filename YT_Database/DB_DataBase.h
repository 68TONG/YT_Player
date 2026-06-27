#pragma once

#include <DB_Info.h>

template <typename FileInfo_T = DB_FileInfo, typename ListInfo_T = DB_ListInfo>
class DB_Database
{
    static_assert(std::is_base_of<DB_FileInfo, FileInfo_T>::value, "FileInfo_T must inherit from Base");
    static_assert(std::is_base_of<DB_ListInfo, ListInfo_T>::value, "ListInfo_T must inherit from Base");
protected:
    QSqlDatabase db;
    DB_Database(QString fileInfoTableName, QString listInfoTableName)
        : fileInfoTableName(fileInfoTableName)
        , listInfoTableName(listInfoTableName)
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
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
            query.prepare(QString("INSERT INTO %1 (Id) VALUES (?)").arg(tableName));
            for (int id : ids) {
                query.addBindValue(id);
                query.exec();
            }
        }
        QSqlQuery query;
        const QString tableName;
    };
public:
    QSqlDatabase getSqlDatabase() const
    {
        return db;
    }

    virtual FileInfo_T getFileInfo(int info_id)
    {
        QString sql = QString("SELECT * FROM %1 WHERE Id = %2")
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
        QString sql = QString("SELECT %1 FROM %2 WHERE Id = ?")
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
            JOIN %2 AS T_Ids ON F_Info.Id = T_Ids.Id
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
            info_list.insert(info.Id, info);
        }
        return info_list;
    }

    virtual int getFileInfo_ID(QString path)
    {
        QString sql = QString("SELECT Id FROM %1 WHERE filePath = ?")
            .arg(fileInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(DB::canonicalPath(path));
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
            JOIN %2 AS T_Ids ON F_Info.Id = T_Ids.Id
            CROSS JOIN json_each(F_Info.fileTag)
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
            JOIN %2 AS T_Ids ON F_Info.Id = T_Ids.Id
            CROSS JOIN json_tree(F_Info.fileTag, :tag_key)
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
        int info_id = getFileInfo_ID(path);
        if (info_id >= 0) return info_id;

        FileInfo_T info(path);
        return addFileInfo(info);
    }

    virtual int addFileInfo(const FileInfo_T& info)
    {
        if (QFile::exists(info.filePath) == false) return -1;

        QSqlQuery query;
        query.prepare(info.sqlQuery(fileInfoTableName));
        const auto sql_data = info.toSqlData();
        for (const auto& it : sql_data) {
            query.addBindValue(it);
        }

        if (query.exec() == false) {
            qDebug() << "DB_Database addFileInfo failed - path:" << info.filePath
                << "error:" << query.lastError().text();
            return -2;
        }
        return query.lastInsertId().toInt();
    }

    virtual int setFileInfo(int info_id, QString index, const QVariant& data)
    {
        QString sql = QString("UPDATE %1 SET %2 = :data WHERE Id = :info_id")
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
        QString sql = QString("DELETE FROM %1 WHERE Id = %2")
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

    virtual int sortFileInfo(QString list_id, QString sort_tag)
    {
        auto info_ids = getListInfo_FileIds(list_id);
        if (sortFileInfo(info_ids, sort_tag) < 0)
            return -2;

        QString sql = QString(R"(
            UPDATE %1 
            SET 
                sortTag = COALESCE(:sort_tag, sortTag),
                reverseSortTag = 
                    CASE WHEN :reverse_sort_tag = 1 THEN (~reverseSortTag & 1)
                    ELSE 0 END,
                fileInfoIds = json(:file_ids) 
            WHERE name = :list_id
        )").arg(listInfoTableName);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":list_id", list_id);
        query.bindValue(":sort_tag", sort_tag != "Id" ? sort_tag : QVariant());
        query.bindValue(":reverse_sort_tag", sort_tag == "Id");
        query.bindValue(":file_ids", DB::listToJsonByte<int>(info_ids));
        if (query.exec() == false)
        {
            qDebug() << "DB_Database sortListInfo_FileIds failed - list_id:" << list_id
                << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual int sortFileInfo(QList<int>& info_ids, QString sort_tag)
    {
        if (sort_tag == "Id") {
            std::reverse(info_ids.begin(), info_ids.end());
            return 0;
        }

        TempIdsTable temp_table(info_ids);
        QString sql = QString(R"(
            SELECT 
                F_Info.Id AS Id,
                CASE WHEN json_each.type = 'object'
                THEN '' 
                WHEN json_each.type = 'array'
                THEN json_extract(json_each.value, '$[0]')
                ELSE json_each.value 
                END AS TagValue
            FROM %1 AS F_Info
            JOIN %2 AS T_Ids ON F_Info.Id = T_Ids.Id
            CROSS JOIN json_each(F_Info.fileTag)
            WHERE json_each.key = :sort_tag
        )").arg(fileInfoTableName).arg(temp_table.tableName);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":sort_tag", sort_tag);
        if (query.exec() == false) {
            qDebug() << "DB_Database sortListInfo_FileIds failed - sort_tag:" << sort_tag
                << "error:" << query.lastError().text();
            return -1;
        }

        QHash<int, QString> sort_container;
        while (query.next()) {
            auto data = YT_Func::getObject().getChinesePinYin(query.value("TagValue").toString());;
            sort_container.insert(query.value("ID").toInt(), data);
        }
        std::stable_sort(info_ids.begin(), info_ids.end(), [this, sort_container](const int& v1, const int& v2) -> bool
            { return QString::compare(sort_container[v1], sort_container[v2], Qt::CaseInsensitive) < 0; });
        return 0;
    }

    virtual QList<int> searchFileInfo(QString list_id, QString search_tag, QString header_tag = "")
    {
        auto info_ids = getListInfo_FileIds(list_id);
        return searchFileInfo(info_ids, search_tag, header_tag);
    }

    virtual QList<int> searchFileInfo(const QList<int>& info_ids, QString search_tag, QString header_tag = "")
    {
        TempIdsTable temp_table(info_ids);
        QString sql = QString(R"(
            SELECT DISTINCT F_Info.Id AS Id
            FROM %1 AS F_Info
            JOIN %2 AS T_Ids ON F_Info.Id = T_Ids.Id
            CROSS JOIN json_tree(F_Info.fileTag, :header_tag)
            WHERE json_tree.value GLOB :search_tag
        )").arg(fileInfoTableName).arg(temp_table.tableName);

        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":search_tag", search_tag);
        query.bindValue(":header_tag", header_tag.isEmpty() ? "$" : "$." + header_tag);
        QList<int> result_info_ids;
        if (query.exec() == false) {
            qDebug() << "DB_Database searchFileInfo failed - search_tag:" << search_tag << " header_tag: " << header_tag
                << "error:" << query.lastError().text();
            return result_info_ids;
        }

        while (query.next()) {
            result_info_ids.append(query.value("Id").toInt());
        }
        return result_info_ids;
    }

    virtual ListInfo_T getListInfo(QString info_id)
    {
        QString sql = QString("SELECT * FROM %1 WHERE name = ?")
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
        QString sql = QString("SELECT %1 FROM %2 WHERE name = ?")
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
        QString sql = QString("SELECT name FROM %1 ORDER BY sortIndex ASC")
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
        QString sql = QString("SELECT json_each.value FROM %1 AS L_Info, json_each(L_Info.fileInfoIds) WHERE L_Info.name = ?")
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

    virtual int addListInfo(const ListInfo_T& info)
    {
        QSqlQuery query;
        query.prepare(info.sqlQuery(listInfoTableName));
        const auto sql_data = info.toSqlData();
        for (const auto& it : sql_data) {
            query.addBindValue(it);
        }

        if (query.exec() == false) {
            qDebug() << "DB_Database addListInfo failed - name:" << info.name
                << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual int addListInfo_FileIds(QString list_id, int file_id)
    {
        QString sql = QString(R"(
            UPDATE %1 
            SET fileInfoIds = json_insert(
                COALESCE(fileInfoIds, '[]'), 
                '$[#]', 
                :file_id
            ) 
            WHERE Name = :list_id
            AND NOT EXISTS (
                SELECT 1 
                FROM json_each(fileInfoIds) 
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

    virtual QList<QString> addListInfo_FileIds(QString list_id, const QList<QString>& path_list)
    {
        QList<QString> failed_paths;
        for (auto it : path_list) {
            QFileInfo file_info(it);
            if (file_info.isFile()) {
                int sic = addListInfo_FileIds(list_id, file_info.filePath());
                if (sic < 0) failed_paths.append(file_info.filePath());
                continue;
            }

            if (file_info.isDir() == false) continue;
            QDirIterator file_iterator(file_info.filePath(), QDir::Files, QDirIterator::Subdirectories);
            while (file_iterator.hasNext()) {
                file_iterator.next();
                int sic = addListInfo_FileIds(list_id, file_iterator.filePath());
                if (sic < 0) failed_paths.append(file_iterator.filePath());
            }
        }
        return failed_paths;
    }

    virtual int setListInfo(QString info_id, QString index, const QVariant& data)
    {
        QString sql = QString("UPDATE %1 SET %2 = :data WHERE name = :info_id")
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
        int info_sort_index = getListInfo(info_id, "sortIndex").toInt();
        int target_sort_index = getListInfo(target_id, "sortIndex").toInt();

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
                    SET sortIndex = sortIndex + 1
                    WHERE sortIndex >= :target AND sortIndex < :current
                )").arg(listInfoTableName));
                query.bindValue(":target", target_sort_index);
                query.bindValue(":current", info_sort_index);
            }
            else {
                query.prepare(QString(R"(
                    UPDATE %1 
                    SET sortIndex = sortIndex - 1
                    WHERE sortIndex > :current AND sortIndex <= :target
                )").arg(listInfoTableName));
                query.bindValue(":current", info_sort_index);
                query.bindValue(":target", target_sort_index);
            }
            if (query.exec() == false) {
                throw QString("更新中间记录失败: %1").arg(query.lastError().text());
            }

            query.prepare(QString("UPDATE %1 SET sortIndex = :idx WHERE name = :id")
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

    virtual int setListInfo_FileIds(QString info_id, const QList<int>& file_ids)
    {
        return setListInfo(info_id, DB_ListInfo::I_FileInfoIds, DB::listToJsonByte<int>(file_ids, QJsonDocument::Compact));
    }

    virtual int deleteListInfo(QString info_id)
    {
        QString sql = QString("DELETE FROM %1 WHERE name = ?")
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
        QString sql = QString("UPDATE %1 SET fileInfoIds = json_remove(fileInfoIds, '$[:index]') WHERE name = :info_id")
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