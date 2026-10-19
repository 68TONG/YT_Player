#pragma once

#include <DB_Info.h>
#include <YT_Func.h>
#include <QCoreApplication>
#include <QThread>

template <typename FileInfo_T, typename ListInfo_T>
class DB_Database
{
    static_assert(std::is_base_of<DB_FileInfo, FileInfo_T>::value, "FileInfo_T must inherit from Base");
    static_assert(std::is_base_of<DB_ListInfo, ListInfo_T>::value, "ListInfo_T must inherit from Base");
    static QString connectionNameForThread(QThread* thread)
    {
        Q_ASSERT_X(thread != nullptr, "DB_Database", "A thread is required for a thread-specific database connection");
        Q_ASSERT_X(thread == QThread::currentThread(), "DB_Database",
            "The database connection must be created in the target thread");
        return QStringLiteral("YT_Database_%1").arg(
            reinterpret_cast<quintptr>(QThread::currentThreadId()), 0, 16);
    }
    bool removeConnectionOnDestruction = false;
protected:
    DB_Database()
        : DB_Database(QStringLiteral("YT_Database"))
    {
    }

    explicit DB_Database(QThread* thread)
        : DB_Database(connectionNameForThread(thread), true)
    {
    }

    explicit DB_Database(const QString& connectionName, bool removeConnectionOnDestruction = false)
        : removeConnectionOnDestruction(removeConnectionOnDestruction)
    {
        if (QSqlDatabase::contains(connectionName) == false) {
            auto db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
            if (QFile::exists("C:/TONG/Code/YT_Player/YT_Database/YT_Database.db")) {
                db.setDatabaseName("C:/TONG/Code/YT_Player/YT_Database/YT_Database.db");
            }
            else {
                db.setDatabaseName("./YT_PlayerData/YT_Database.db");
            }
            if (db.open() == false) {
                qWarning() << "DB_Database: connection with database failed";
            }

            QSqlQuery query(db);
            query.exec("PRAGMA foreign_keys = ON");
            query.exec("PRAGMA foreign_keys");
            if (query.next() && query.value(0).toInt() == 1) {
                qDebug() << "外键约束已启用";
            }
            else {
                qWarning() << "警告：外键约束未能启用";
            }
        }
        db = QSqlDatabase::database(connectionName);
    }
    virtual ~DB_Database()
    {
        if (removeConnectionOnDestruction == false
            || QCoreApplication::instance() == nullptr
            || db.isValid() == false) {
            return;
        }

        const QString connectionName = db.connectionName();
        qDebug() << "connectionName " << db.connectionName();
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }

    class TempIdsTable
    {
    public:
        TempIdsTable(QSqlDatabase database, const QList<int>& ids = {}, QString tableName = "tempIdsTable")
            :database(database), query(database), tableName(tableName)
        {
            if (database.transaction() == false) {
                qDebug() << "DB_Database TempIdsTable database transaction error:";
            }
            if (query.exec("CREATE TEMP TABLE IF NOT EXISTS " + tableName + " (Id INT PRIMARY KEY)") == false) {
                qDebug() << "DB_Database TempIdsTable CREATE TABLE error:" << query.lastError().text();
            }
            addIds(ids);
        }
        ~TempIdsTable()
        {
            if (query.exec("DROP TABLE IF EXISTS " + tableName) == false) {
                qDebug() << "DB_Database TempIdsTable DROP TABLE error:" << query.lastError().text();
            }
            if (database.commit() == false) {
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

        QSqlDatabase database;
        QSqlQuery query;
        const QString tableName;
    };
public:

    QSqlDatabase db;
    virtual FileInfo_T initFileInfo(QSqlQuery& query) {
        FileInfo_T info;
        initFileInfo(info, query);
        return info;
    }

    virtual void initFileInfo(FileInfo_T& info, QSqlQuery& query) {
        info.Id = query.value(DB_FileInfo::I_Id).toInt();
        info.filePath = query.value(DB_FileInfo::I_FilePath).toString();
        info.fileTitle = query.value(DB_FileInfo::I_FileTitle).toString();
        info.fileTag = QJsonDocument::fromJson(query.value(DB_FileInfo::I_FileTag).toByteArray()).array();
    }

    virtual FileInfo_T getFileInfo(int info_id)
    {
        QString sql = QString("SELECT Id, filePath, fileTitle, fileTag FROM %1 WHERE Id = ?")
            .arg(DB::FileInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(info_id);
        if (query.exec() == false || query.next() == false) {
            qDebug() << "DB_Database getFileInfo failed - info_id:" << info_id
                << "error:" << query.lastError().text();
            return {};
        }

        return initFileInfo(query);
    }

    virtual QVariant getFileInfo(int info_id, QString index)
    {
        QString sql = QString("SELECT %1 FROM %2 WHERE Id = ?")
            .arg(index)
            .arg(DB::FileInfo);

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

    virtual QHash<int, FileInfo_T> getFileInfo(const QList<int>& info_ids)
    {
        TempIdsTable temp_table(db, info_ids);
        QString sql = QString(R"(
            SELECT * FROM %1 AS F_Info
            JOIN %2 AS T_Ids ON F_Info.Id = T_Ids.Id
        )").arg(DB::FileInfo).arg(temp_table.tableName);

        QSqlQuery query(db);
        query.prepare(sql);
        if (query.exec() == false) {
            qDebug() << "DB_Database getFileInfo failed - info_ids:" << info_ids
                << "error:" << query.lastError().text();
            return {};
        }

        QHash<int, FileInfo_T> info_list;
        while (query.next()) {
            FileInfo_T info = initFileInfo(query);
            info_list.insert(info.Id, info);
        }
        return info_list;
    }

    virtual int getFileInfo_Id(QString path)
    {
        QString sql = QString("SELECT Id FROM %1 WHERE filePath = ?")
            .arg(DB::FileInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(DB::canonicalPath(path));
        if (query.exec() == false || query.next() == false) {
            // qDebug() << "DB_Database getFileInfo_Id failed - path:" << path << "error:" << query.lastError().text();
            return -1;
        }
        return query.value(0).toInt();
    }

    virtual QList<QString> getFileInfo_TagValues(QString list_id)
    {
        auto&& info_ids = getListInfo_FileIds(list_id);
        return getFileInfo_TagValues(info_ids);
    }

    virtual QList<QString> getFileInfo_TagValues(const QList<int>& info_ids)
    {
        TempIdsTable temp_table(db, info_ids);
        QString sql = QString(R"(
            SELECT DISTINCT json_each.value
            FROM %1 AS F_Info
            JOIN %2 AS T_Ids ON F_Info.Id = T_Ids.Id
            CROSS JOIN json_each(F_Info.fileTag)
        )").arg(DB::FileInfo).arg(temp_table.tableName);

        QSqlQuery query(db);
        query.prepare(sql);
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

    virtual QList<QString> getFileInfo_TagValues_CreatorId(QString creator_id)
    {
        auto&& info_ids = getCreatorInfo_FileIds(creator_id);
        return getFileInfo_TagValues(info_ids);
    }

    virtual int addFileInfo(const FileInfo_T& info)
    {
        QString sql = QString("INSERT INTO %1 (%2, %3, %4) VALUES (?, ?, ?)")
            .arg(DB::FileInfo)
            .arg(DB_FileInfo::I_FilePath)
            .arg(DB_FileInfo::I_FileTitle)
            .arg(DB_FileInfo::I_FileTag);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(DB::canonicalPath(info.filePath));
        query.addBindValue(info.fileTitle);
        query.addBindValue(QJsonDocument(info.fileTag).toJson(QJsonDocument::Indented));
        if (query.exec() == false) {
            qDebug() << "DB_Database addFileInfo failed - path:" << info.filePath
                << "error:" << query.lastError().text();
            return -1;
        }
        return query.lastInsertId().toInt();
    }

    virtual int addFileInfo(QString file_path)
    {
        int info_id = getFileInfo_Id(file_path);
        if (info_id >= 0) return info_id;

        FileInfo_T info(file_path);
        return addFileInfo(info);
    }

    virtual int setFileInfo(int info_id, QString index, const QVariant& data)
    {
        QString sql = QString("UPDATE %1 SET %2 = :data WHERE Id = :info_id")
            .arg(DB::FileInfo)
            .arg(index);

        QSqlQuery query(db);
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
            .arg(DB::FileInfo)
            .arg(info_id);

        QSqlQuery query(db);
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
        )").arg(DB::FileListInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.bindValue(":list_id", list_id);
        query.bindValue(":sort_tag", sort_tag != DB_ListInfo::I_ReverseSortTag ? sort_tag : QVariant());
        query.bindValue(":reverse_sort_tag", sort_tag == DB_ListInfo::I_ReverseSortTag);
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
        if (sort_tag == DB_ListInfo::I_ReverseSortTag) {
            std::reverse(info_ids.begin(), info_ids.end());
        }
        return 0;
    }

    virtual QList<int> searchFileInfo(QString list_id, QString search_tag)
    {
        auto&& info_ids = getListInfo_FileIds(list_id);
        return searchFileInfo(info_ids, search_tag);
    }

    virtual QList<int> searchFileInfo(const QList<int>& info_ids, QString search_tag)
    {
        TempIdsTable temp_table(db, info_ids);
        QString sql = QString(R"(
            SELECT DISTINCT F_Info.Id AS Id
            FROM %1 AS F_Info
            CROSS JOIN %2 AS T_Ids ON F_Info.Id = T_Ids.Id
            CROSS JOIN json_each(F_Info.fileTag) AS F_Tag
            WHERE F_Tag.value GLOB :search_tag
        )").arg(DB::FileInfo).arg(temp_table.tableName);

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
        if (query.exec() == false) {
            qDebug() << "DB_Database searchFileInfo failed - search_tag:" << search_tag
                << "error:" << query.lastError().text();
            return {};
        }

        QList<int> result_info_ids;
        while (query.next()) {
            result_info_ids.append(query.value("Id").toInt());
        }
        return result_info_ids;
    }

    virtual ListInfo_T initListInfo(QSqlQuery& query) {
        ListInfo_T info;
        initListInfo(info, query);
        return info;
    }

    virtual void initListInfo(ListInfo_T& info, QSqlQuery& query) {
        info.name = query.value(DB_ListInfo::I_Name).toString();
        info.type = query.value(DB_ListInfo::I_Type).toString();
        info.fileInfoIds = DB::listFromJsonByte<int>(query.value(DB_ListInfo::I_FileInfoIds).toByteArray());
        info.sortTag = query.value(DB_ListInfo::I_SortTag).toString();
        info.selectTag = DB::listFromJsonByte<QString>(query.value(DB_ListInfo::I_SelectTag).toByteArray());
    }

    virtual ListInfo_T getListInfo(QString info_id)
    {
        QString sql = QString("SELECT name, type, fileInfoIds, sortTag, selectTag FROM %1 WHERE name = ?")
            .arg(DB::FileListInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(info_id);
        if (query.exec() == false || query.next() == false) {
            qDebug() << "DB_Database getListInfo failed - info_id:" << info_id
                << "error:" << query.lastError().text();
            return {};
        }

        return initListInfo(query);
    }

    virtual QVariant getListInfo(QString info_id, QString index)
    {
        QString sql = QString("SELECT %1 FROM %2 WHERE name = ?")
            .arg(index)
            .arg(DB::FileListInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(info_id);
        if (query.exec() == false || query.next() == false) {
            qDebug() << "DB_Database getListInfo failed - info_id:" << info_id
                << "index:" << index << "error:" << query.lastError().text();
            return {};
        }
        return query.value(0);
    }

    virtual QList<QString> getListInfo_NameAll(QString type)
    {
        QString sql = QString("SELECT name FROM %1 WHERE type = ? ORDER BY listInfoIndex ASC")
            .arg(DB::FileListInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(type);
        if (query.exec() == false) {
            qDebug() << "DB_Database getListInfo_NameAll failed"
                << "error:" << query.lastError().text();
            return {};
        }

        QList<QString> info_names;
        while (query.next()) {
            info_names.append(query.value(0).toString());
        }
        return info_names;
    }

    virtual QList<int> getListInfo_FileIds(QString info_id)
    {
        QString sql = QString("SELECT value FROM json_each((SELECT fileInfoIds FROM %1 WHERE name = ?))")
            .arg(DB::FileListInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(info_id);
        if (query.exec() == false) {
            qDebug() << "DB_Database getListInfo_FileIds failed - info_id:" << info_id
                << "error:" << query.lastError().text();
            return {};
        }

        QList<int> info_ids;
        while (query.next()) {
            info_ids.append(query.value(0).toInt());
        }
        return info_ids;
    }

    virtual int addListInfo(const ListInfo_T& info)
    {
        QString sql = QString("INSERT INTO %1 (name, type, fileInfoIds, sortTag, selectTag) VALUES (?, ?, ?, ?, ?)")
            .arg(DB::FileListInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(info.name);
        query.addBindValue(info.type);
        query.addBindValue(DB::listToJsonByte<int>(info.fileInfoIds, QJsonDocument::Compact));
        query.addBindValue(info.sortTag);
        query.addBindValue(DB::listToJsonByte<QString>(info.selectTag));
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
            WHERE name = :list_id
            AND NOT EXISTS (
                SELECT 1 
                FROM json_each(fileInfoIds) 
                WHERE json_each.value = :file_id
            )
        )").arg(DB::FileListInfo);

        QSqlQuery query(db);
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

    virtual int addListInfo_FileIds(QString list_id, QString file_path)
    {
        int file_id = addFileInfo(file_path);
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
            else if (file_info.isDir()) {
                QDirIterator file_iterator(file_info.filePath(), QDir::Files, QDirIterator::Subdirectories);
                while (file_iterator.hasNext()) {
                    file_iterator.next();
                    int sic = addListInfo_FileIds(list_id, file_iterator.filePath());
                    if (sic < 0) failed_paths.append(file_iterator.filePath());
                }
                continue;
            }
        }
        return failed_paths;
    }

    virtual int setListInfo(QString info_id, QString index, const QVariant& data)
    {
        QString sql = QString("UPDATE %1 SET %2 = :data WHERE name = :info_id")
            .arg(DB::FileListInfo)
            .arg(index);

        QSqlQuery query(db);
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

    virtual int setListInfo_ListInfoIndex(QString info_id, QString target_id)
    {
        const auto info_type = getListInfo(info_id, DB_ListInfo::I_Type).toString();
        const auto target_type = getListInfo(target_id, DB_ListInfo::I_Type).toString();
        if (info_type != target_type || info_type.isEmpty() || target_type.isEmpty()) {
            qDebug() << "DB_Database setListInfo_ListInfoIndex type mismatch - info_id:" << info_id << "target_id:" << target_id
                << "info_type:" << info_type << "target_type:" << target_type;
            return -1;
        }

        int info_index = getListInfo(info_id, "listInfoIndex").toInt();
        int target_index = getListInfo(target_id, "listInfoIndex").toInt();
        QString sql = QString(R"(
            UPDATE %1
            SET listInfoIndex =
                CASE
                    WHEN name = :info_id THEN :newIndex
                    WHEN :newIndex < :oldIndex
                        AND listInfoIndex >= :newIndex
                        AND listInfoIndex < :oldIndex
                    THEN listInfoIndex + 1
                    WHEN :newIndex > :oldIndex
                        AND listInfoIndex > :oldIndex
                        AND listInfoIndex <= :newIndex
                    THEN listInfoIndex - 1
                    ELSE listInfoIndex
                END
            WHERE type = :type
            AND (
                name = :info_id
                OR (
                    :newIndex < :oldIndex
                    AND listInfoIndex >= :newIndex
                    AND listInfoIndex < :oldIndex
                )
                OR (
                    :newIndex > :oldIndex
                    AND listInfoIndex > :oldIndex
                    AND listInfoIndex <= :newIndex
                )
            )
        )").arg(DB::FileListInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.bindValue(":info_id", info_id);
        query.bindValue(":type", info_type);
        query.bindValue(":oldIndex", info_index);
        query.bindValue(":newIndex", target_index);
        if (query.exec() == false) {
            qDebug() << "DB_Database setListInfo_ListInfoIndex failed - info_id:" << info_id
                << "target_id:" << target_id << "error:" << query.lastError().text();
            return -3;
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
            .arg(DB::FileListInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(info_id);
        if (query.exec() == false) {
            qDebug() << "DB_Database deleteListInfo failed - info_id:" << info_id
                << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual int deleteListInfo_FileIds(QString list_id, int index)
    {
        QString sql = QString("UPDATE %1 SET fileInfoIds = json_remove(fileInfoIds, '$[:index]') WHERE name = :list_id")
            .arg(DB::FileListInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.bindValue(":index", index);
        query.bindValue(":list_id", list_id);
        if (query.exec() == false) {
            qDebug() << "DB_Database deleteListInfo_FileIds failed - list_id:" << list_id
                << "index:" << index
                << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    QList<QString> getCreatorInfo_NameAll()
    {
        QString sql = QString("SELECT name FROM %1")
            .arg(DB::CreatorInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        if (query.exec() == false) {
            qDebug() << "DB_Database getCreatorInfo_NameAll failed"
                << "error: " << query.lastError().text();
            return {};
        }

        QList<QString> info_list;
        while (query.next()) {
            info_list.append(query.value(0).toString());
        }
        return info_list;
    }

    QList<int> getCreatorInfo_FileIds()
    {
        QString sql = QString("SELECT fileId FROM %1")
            .arg(DB::NetworkFileInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        if (query.exec() == false) {
            qDebug() << "YT_FileDatabase getCreatorInfo_FileIds failed"
                << "error: " << query.lastError().text();
            return {};
        }

        QList<int> info_ids;
        while (query.next()) {
            info_ids.append(query.value(0).toInt());
        }
        return info_ids;
    }

    QList<int> getCreatorInfo_FileIds(QString info_id)
    {
        QString sql = QString("SELECT fileId FROM %1 WHERE creatorId = :creator_id")
            .arg(DB::NetworkFileInfo);

        QSqlQuery query(db);
        query.prepare(sql);
        query.bindValue(":creator_id", info_id);
        if (query.exec() == false) {
            qDebug() << "YT_FileDatabase getFileInfo_CreatorId failed - creator_id: " << info_id
                << " error:" << query.lastError().text();
            return {};
        }

        QList<int> info_ids;
        while (query.next()) {
            info_ids.append(query.value(0).toInt());
        }
        return info_ids;
    }

    virtual QString getFileCoverInfo_HashValue(QString file_path)
    {
        QString sql = QString("SELECT hashValue FROM FileCoverInfo WHERE filePath = ?");

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(DB::canonicalPath(file_path));
        if (query.exec() == false) {
            qDebug() << "DB_Database getFileCoverInfo_HashValue failed - path:" << file_path
                << "error:" << query.lastError().text();
            return {};
        }
        if (query.next() == false) return {};

        return query.value(0).toString();
    }

    virtual QPointF getFileCoverInfo_VisualFocus(QString file_path)
    {
        QString sql = QString("SELECT visualFocusX, visualFocusY FROM FileCoverInfo WHERE filePath = ?");

        QSqlQuery query(db);
        query.prepare(sql);
        query.addBindValue(DB::canonicalPath(file_path));
        if (query.exec() == false) {
            qDebug() << "DB_Database getFileCoverInfo_VisualFocus failed - path:" << file_path
                << "error:" << query.lastError().text();
            return {};
        }
        if (query.next() == false) return {};

        return QPointF(query.value(0).toDouble(), query.value(1).toDouble());
    }

    virtual int setFileCoverInfo_HashValue(QString file_path, QString data)
    {
        QString sql = QString(R"(
            INSERT INTO FileCoverInfo (filePath, hashValue)
            VALUES (:filePath, :data)
            ON CONFLICT(filePath) DO UPDATE SET hashValue = excluded.hashValue
        )");
        QSqlQuery query(db);
        query.prepare(sql);
        query.bindValue(":data", data);
        query.bindValue(":filePath", DB::canonicalPath(file_path));
        if (query.exec() == false) {
            qDebug() << "DB_Database setFileCoverInfo_HashValue failed - file_path:" << file_path
                << "data:" << data << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    virtual int setFileCoverInfo_VisualFocus(QString file_path, QPointF data)
    {
        QString sql = QString(R"(
            INSERT INTO FileCoverInfo (filePath, visualFocusX, visualFocusY)
            VALUES (:filePath, :x, :y)
            ON CONFLICT(filePath) DO UPDATE SET
                visualFocusX = excluded.visualFocusX,
                visualFocusY = excluded.visualFocusY
        )");
        QSqlQuery query(db);
        query.prepare(sql);
        query.bindValue(":x", data.x());
        query.bindValue(":y", data.y());
        query.bindValue(":filePath", DB::canonicalPath(file_path));
        if (query.exec() == false) {
            qDebug() << "DB_Database setFileCoverInfo_VisualFocus failed - file_path:" << file_path
                << "data:" << data << "error:" << query.lastError().text();
            return -1;
        }
        return 0;
    }

    const QString listInfoType;
};
