#pragma once

#include <YT_GeneralH.h>

class YT_PathWidgetModel : public QObject {
    Q_OBJECT
        QML_ELEMENT
public:
    explicit YT_PathWidgetModel(QObject* parent = nullptr, QString widgetPath = "");

    Q_INVOKABLE QString getData();
    Q_INVOKABLE QString getData_WidgetPath();
    Q_INVOKABLE void initData(const QString& data);

    Q_INVOKABLE void setSrcData(const int& data);
    Q_INVOKABLE void setSrcData(const QString& data);
    Q_INVOKABLE void setSrcData(const QList<QString>& data);
    Q_INVOKABLE void addSrcData(const QString& data);
    Q_INVOKABLE void addSrcData(const int index, const QString& data);
    Q_INVOKABLE QList<QString> getSubPath(const int index);
public:
    bool isSetDataing = false;
    Q_PROPERTY(QString widgetPath READ getWidgetPath CONSTANT)
        QString widgetPath = {};
    QString getWidgetPath() const;

    Q_PROPERTY(QList<QString> srcData READ getSrcData NOTIFY srcDataChanged)
        QList<QString> srcData = {};
    QList<QString> getSrcData() const;
    Q_SIGNAL void srcDataChanged();

    Q_PROPERTY(QList<QString> pathData READ getPathData NOTIFY pathDataChanged)
        QList<QString> pathData = {};
    QList<QString> getPathData() const;
    Q_SIGNAL void pathDataChanged();

    Q_PROPERTY(QList<QString> typeData READ getTypeData NOTIFY typeDataChanged)
        QList<QString> typeData = {};
    QList<QString> getTypeData() const;
    Q_SIGNAL void typeDataChanged();

    std::function<QList<QString>(int)> getSubPathFunc = nullptr;
    static void parseSrcData(const QString& data, QString& path_data, QString& type_data);
};