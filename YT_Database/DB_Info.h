#pragma once
#include <YT_Info.h>
#include <YT_Func.h>
#include <YT_GeneralH.h>

#include <QSqlQuery>
#include <QSqlerror>
#include <QSqlDatabase>

namespace DB {
    inline QString canonicalPath(const QString& path) {
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
    DB_FileInfo(QString filePath)
        : filePath(filePath) {
    }
    DB_FileInfo(QSqlQuery& query) {
        DB_FileInfo::fromSqlData(query);
    }

    virtual QString sqlQuery(QString table_name) const {
        return QString("INSERT INTO %1 (%2, %3) VALUES (?, ?)")
            .arg(table_name)
            .arg(I_FilePath)
            .arg(I_FileTag);
    }
    virtual QList<QVariant> toSqlData() const {
        QList<QVariant> data;
        data.append(DB::canonicalPath(filePath));
        data.append(QJsonDocument(fileTag).toJson());
        return data;
    }
    virtual void fromSqlData(QSqlQuery& query) {
        Id = query.value(I_Id).toInt();
        filePath = query.value(I_FilePath).toString();
        fileTag = QJsonDocument::fromJson(query.value(I_FileTag).toByteArray()).object();
    }

    int Id = -1;
    QString filePath = {};
    QJsonObject fileTag = {};

    inline static const QString I_IsValid = "1";
    inline static const QString I_Id = "Id";
    inline static const QString I_FilePath = "filePath";
    inline static const QString I_FileTag = "fileTag";
};

class DB_ListInfo
{
public:
    DB_ListInfo() {}
    DB_ListInfo(QString name)
        : name(name) {
    }
    DB_ListInfo(QSqlQuery& query) {
        DB_ListInfo::fromSqlData(query);
    }

    virtual QString sqlQuery(QString table_name) const {
        return QString("INSERT INTO %1 (%2, %3, %4, %5) VALUES (?, ?, ?, ?)")
            .arg(table_name)
            .arg(I_Name)
            .arg(I_FileInfoIds)
            .arg(I_SortTag)
            .arg(I_SelectTag);
    }
    virtual QList<QVariant> toSqlData() const {
        QList<QVariant> data;
        data.append(name);
        data.append(DB::listToJsonByte<int>(fileInfoIds, QJsonDocument::Compact));
        data.append(sortTag);
        data.append(DB::listToJsonByte<QString>(selectTag));
        return data;
    }
    virtual void fromSqlData(QSqlQuery& query) {
        name = query.value(I_Name).toString();
        fileInfoIds = DB::listFromJsonByte<int>(query.value(I_FileInfoIds).toByteArray());
        sortTag = query.value(I_SortTag).toString();
        selectTag = DB::listFromJsonByte<QString>(query.value(I_SelectTag).toByteArray());
    }

    QString name = {};
    QList<int> fileInfoIds = {};
    QString sortTag = {};
    QList<QString> selectTag = {};

    inline static const QString I_IsValid = "1";
    inline static const QString I_Name = "name";
    inline static const QString I_FileInfoIds = "fileInfoIds";
    inline static const QString I_SortTag = "sortTag";
    inline static const QString I_SelectTag = "selectTag";
};