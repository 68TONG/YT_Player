#include "YT_SearchWidgetModel.h"

YT_SearchWidgetModel::YT_SearchWidgetModel(QObject *parent)
    : QObject{parent}
{
}

QList<QString> YT_SearchWidgetModel::getPathModel() const {
    return pathModel;
}
QList<QString> YT_SearchWidgetModel::getFilterModel() const {
    return filterModel;
}
QList<QString> YT_SearchWidgetModel::getHistoryModel() const {
    return historyModel;
}
