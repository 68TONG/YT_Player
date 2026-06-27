#pragma once

#include <YT_GeneralH.h>

class YT_SearchWidgetModel : public QObject
{
    Q_OBJECT
        QML_ELEMENT
public:
    explicit YT_SearchWidgetModel(QObject* parent = nullptr);

    Q_PROPERTY(QList<QString> pathModel READ getPathModel NOTIFY pathModelChanged)
        QList<QString> pathModel = {};
    QList<QString> getPathModel() const;
    Q_SIGNAL void pathModelChanged();

    Q_PROPERTY(QList<QString> filterModel READ getFilterModel NOTIFY filterModelChanged)
        QList<QString> filterModel = {};
    QList<QString> getFilterModel() const;
    Q_SIGNAL void filterModelChanged();

    Q_PROPERTY(QList<QString> historyModel READ getHistoryModel NOTIFY historyModelChanged)
        QList<QString> historyModel = {};
    QList<QString> getHistoryModel() const;
    Q_SIGNAL void historyModelChanged();
};
