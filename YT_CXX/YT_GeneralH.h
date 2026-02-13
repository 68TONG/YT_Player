#ifndef YT_GENERALH_H
#define YT_GENERALH_H

#include <QObject>
#include <QWindow>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDirIterator>

#include <QFont>
#include <QSize>
#include <QColor>

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QThread>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>

#include <QLocale>
#include <QCollator>

#include <QSet>
#include <QMap>
#include <QHash>
#include <QList>
#include <QQueue>
#include <QVector>

#include <TimeDebugger.h>

int KMP_String(const QString &data, const QString &pattern);

QVariant jsonFirstData(const QJsonValue &data);
QList<QVariant> jsonListData(const QJsonValue &data);
QJsonObject readJsonObjectFromFile(const QString &file_path);
void writeJsonObjectToFile(const QString &file_path, const QJsonObject &json_object);

bool deleteFile(const QString &path);
QString copyFile(const QString &path, const QString &target_path, bool is_cover = false);
QList<QString> copyFolder(const QString &path, const QString &target_path, bool is_cover = false, bool is_while = false);
QString moveFile(const QString &path, const QString &target_path, bool is_cover = false);
QList<QString> moveFolder(const QString &path, const QString &target_path, bool is_cover = false, bool is_while = false);

#endif // YT_GENERALH_H
