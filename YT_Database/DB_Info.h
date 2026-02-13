#pragma once
#include <YT_Info.h>
#include <YT_Func.h>
#include <YT_GeneralH.h>

#include <QSqlQuery>
#include <QSqlerror>
#include <QSqlDatabase>

namespace DB {
    static const QString I_IsValid = "1";
    static const QString I_FileInfo_ID = "ID";
    static const QString I_FileInfo_Path = "Path";
    static const QString I_FileInfo_Tags = "Tags";

    static const QString I_ListInfo_Name = "Name";
    static const QString I_ListInfo_FileInfoIds = "FileInfoIds";
    static const QString I_ListInfo_SortTag = "SortTag";
    static const QString I_ListInfo_SelectTag = "SelectTag";

    inline QString canonicalPath(const QString &path) {
        return QFileInfo(path).canonicalFilePath();
    }

    inline QVariant jsTagFirst(const QJsonValue& data) {
        if (data.isNull() || data.isUndefined())
            return {};
        if (data.isArray())
            return data.toArray().at(0).toVariant();
        return data.toVariant();
    }
    inline QVariant jsTagFirst(const QJsonObject& data, QString key) {
        return jsTagFirst(data.value(key));
    }
    template <typename T = QVariant>
    inline QList<T> jsTagList(const QJsonValue& data) {
        QList<T> list;
        if (data.isNull() || data.isUndefined())
            return list;
        if (data.isArray()) {
            for (const auto& it : data.toArray()) {
                list.append(it.toVariant().value<T>());
            }
            return list;
        }
        list.append(data.toVariant().value<T>());
        return list;
    }
    template <typename T = QVariant>
    inline QList<T> jsTagList(const QJsonObject& data, QString key) {
        return jsTagList<T>(data.value(key));
    }

    template <typename T>
    QJsonArray listToJson(const QList<T>& list) {
        QJsonArray json;
        for (const auto& it : list) {
            json.append(it);
        }
        return json;
    }
    template <typename T>
    QByteArray listToJsonByte(const QList<T>& list, QJsonDocument::JsonFormat format = QJsonDocument::Indented) {
        auto&& json = listToJson<T>(list);
        return QJsonDocument(json).toJson(format);
    }
    template <typename T>
    QList<T> listFromJson(const QJsonArray& json) {
        QList<T> list;
        for (const auto& it : json) {
            list.append(it.toVariant().value<T>());
        }
        return list;
    }
    template <typename T>
    QList<T> listFromJsonByte(const QByteArray& json_data) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(json_data, &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug() << "DB listFromJsonByte error:" << error.errorString();
            return {};
        }
        if (doc.isArray() == false) {
            qDebug() << "DB listFromJsonByte error: not an array";
            return {};
        }
        return listFromJson<T>(doc.array());
    }
}

class DB_FileInfo
{
public:
    DB_FileInfo() {}
    DB_FileInfo(QString Path)
        : Path(Path) {
    }
    DB_FileInfo(QSqlQuery& query) {
        DB_FileInfo::fromSqlData(query);
    }

    virtual QString sqlQuery() = 0;
    virtual QList<QVariant> toSqlData() {
        QList<QVariant> data;
        data.append(DB::canonicalPath(Path));
        data.append(QJsonDocument(Tags).toJson());
        return data;
    }
    virtual void fromSqlData(QSqlQuery& query) {
        ID = query.value(DB::I_FileInfo_ID).toInt();
        Path = query.value(DB::I_FileInfo_Path).toString();
        Tags = QJsonDocument::fromJson(query.value(DB::I_FileInfo_Tags).toByteArray()).object();
    }

    int ID = -1;
    QString Path = {};
    QJsonObject Tags = {};
};

class DB_ListInfo
{
public:
    DB_ListInfo() {}
    DB_ListInfo(QString Name)
        : Name(Name) {
    }
    DB_ListInfo(QSqlQuery& query) {
        DB_ListInfo::fromSqlData(query);
    }

    virtual QString sqlQuery() = 0;
    virtual QList<QVariant> toSqlData() {
        QList<QVariant> data;
        data.append(Name);
        data.append(DB::listToJsonByte<int>(FileInfoIds));
        // QJsonArray json_FileInfoIds;
        // for (const auto& it : FileInfoIds) {
        //     json_FileInfoIds.append(it);
        // }
        // data.append(QJsonDocument(json_FileInfoIds).toJson());

        data.append(SortTag);
        data.append(DB::listToJsonByte<QString>(SelectTag));

        // QJsonArray json_SelectTag;
        // for (const auto& it : SelectTag) {
        //     json_SelectTag.append(it);
        // }
        // data.append(QJsonDocument(json_SelectTag).toJson());
        return data;
    }
    virtual void fromSqlData(QSqlQuery& query) {
        Name = query.value(DB::I_ListInfo_Name).toString();
        FileInfoIds = DB::listFromJsonByte<int>(query.value(DB::I_ListInfo_FileInfoIds).toByteArray());
        // auto json_FileInfoIds = QJsonDocument::fromJson(query.value(DB::I_ListInfo_FileInfoIds).toByteArray()).array();
        // for (const auto& it : json_FileInfoIds) {
        //     FileInfoIds.append(it.toInt());
        // }

        SortTag = query.value(DB::I_ListInfo_SortTag).toString();
        SelectTag = DB::listFromJsonByte<QString>(query.value(DB::I_ListInfo_SelectTag).toByteArray());
        // auto json_SelectTag = QJsonDocument::fromJson(query.value(DB::I_ListInfo_SelectTag).toByteArray()).array();
        // for (const auto& it : json_SelectTag) {
        //     SelectTag.append(it.toString());
        // }
    }

    QString Name = {};
    QList<int> FileInfoIds = {};
    QString SortTag = {};
    QList<QString> SelectTag = {};
};