#pragma once

#include <QFileInfo>
#include <QDirIterator>

#include <QSqlQuery>
#include <QSqlerror>
#include <QSqlDatabase>

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

namespace DB {
    inline const QString FileInfo = "FileInfo";
    inline const QString FileListInfo = "FileListInfo";
    inline const QString FileTagTranslateInfo = "FileTagTranslateInfo";
    inline const QString MusicFileInfo = "MusicFileInfo";
    inline const QString VideoFileInfo = "VideoFileInfo";
    inline const QString PictureFileInfo = "PictureFileInfo";
    inline const QString NetworkFileInfo = "NetworkFileInfo";
    inline const QString CreatorInfo = "CreatorInfo";
    inline const QString CreatorProfile = "CreatorProfile";

    inline QString canonicalPath(const QString& data) {
        return QFileInfo(data).canonicalFilePath();
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
    inline QList<T> jsTagList(const QJsonObject& data, QString json_path) {
        return jsTagList<T>(data.value(json_path));
    }

    inline void jsToTagArray(const QJsonValue& value, QJsonArray& result) {
        if (value.isObject()) {
            const auto data = value.toObject();
            for (const auto& it : data) {
                jsToTagArray(it, result);
            }
        }
        else if (value.isArray()) {
            const auto data = value.toArray();
            for (const auto& it : data) {
                jsToTagArray(it, result);
            }
        }
        else if (result.contains(value) == false) {
            result.append(value);
        }
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

class DB_Info {};
class DB_FileInfo : public DB_Info
{
public:
    DB_FileInfo() {}
    DB_FileInfo(QString filePath)
        : filePath(filePath), fileInfo(filePath)
    {
        {}
    }

    int Id = -1;
    QString filePath = {};
    QString fileTitle = {};
    QJsonArray fileTag = {};
    QFileInfo fileInfo = {};

    inline static const QString I_IsValid = "1";
    inline static const QString I_Id = "Id";
    inline static const QString I_FilePath = "filePath";
    inline static const QString I_FileTitle = "fileTitle";
    inline static const QString I_FileTag = "fileTag";
    inline static const QString I_CoverHashValue = "coverHashValue";
};

class DB_ListInfo : public DB_Info
{
public:
    DB_ListInfo() {}
    DB_ListInfo(QString name, QString type)
        : name(name), type(type)
    {
        {}
    }

    QString name = {};
    QString type = {};
    QList<int> fileInfoIds = {};
    QString sortTag = {};
    QList<QString> selectTag = {};

    inline static const QString I_IsValid = "1";
    inline static const QString I_Name = "name";
    inline static const QString I_Type = "type";
    inline static const QString I_FileInfoIds = "fileInfoIds";
    inline static const QString I_SortTag = "sortTag";
    inline static const QString I_SelectTag = "selectTag";
    inline static const QString I_ReverseSortTag = "reverseSortTag";

    inline static const QString I_Type_Music = "Music";
    inline static const QString I_Type_Video = "Video";
    inline static const QString I_Type_Picture = "Picture";
};

class DB_CreatorInfo : public DB_Info
{
public:
    DB_CreatorInfo() {}
    DB_CreatorInfo(QString name)
        : name(name)
    {
        {}
    }

    QString name = {};
    inline static const QString I_IsValid = "1";
    inline static const QString I_Name = "name";
};

