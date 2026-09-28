#include "YT_PathWidgetModel.h"

#include <QRegularExpression>

YT_PathWidgetModel::YT_PathWidgetModel(QObject* parent, QString widgetPath)
    : QObject(parent), widgetPath(widgetPath)
{
}

Q_INVOKABLE QString YT_PathWidgetModel::getData()
{
    return srcData.join("/");
}

Q_INVOKABLE QString YT_PathWidgetModel::getData_WidgetPath()
{
    return widgetPath + '/' + getData();
}

Q_INVOKABLE void YT_PathWidgetModel::initData(const QString& data)
{
    static const QRegularExpression re(R"([/\\])");
    auto split_data = data.split(re);
    split_data.removeAll("");

    srcData = split_data;
    srcData.removeAll("");

    pathData.resize(srcData.count());
    typeData.resize(srcData.count());
    for (int i = 0;i < srcData.count();i++) {
        parseSrcData(srcData[i], pathData[i], typeData[i]);
    }
}

Q_INVOKABLE void YT_PathWidgetModel::setSrcData(const int& data)
{
    setSrcData(srcData.mid(0, data + 1));
}

Q_INVOKABLE void YT_PathWidgetModel::setSrcData(const QString& data)
{
    static const QRegularExpression re(R"([/\\])");
    setSrcData(data.split(re));
}

Q_INVOKABLE void YT_PathWidgetModel::setSrcData(const QList<QString>& data)
{
    srcData = data;
    srcData.removeAll("");
    if (srcData.isEmpty()) {
        return;
    }

    pathData.resize(srcData.count());
    typeData.resize(srcData.count());
    for (int i = 0;i < srcData.count();i++) {
        parseSrcData(srcData[i], pathData[i], typeData[i]);
    }

    emit srcDataChanged();
    emit pathDataChanged();
    emit typeDataChanged();
}

Q_INVOKABLE void YT_PathWidgetModel::addSrcData(const QString& data)
{
    if (data.isEmpty()) return;

    srcData.append(data);
    pathData.append("");
    typeData.append("");

    const int index = srcData.count() - 1;
    parseSrcData(srcData[index], pathData[index], typeData[index]);

    emit srcDataChanged();
    emit pathDataChanged();
    emit typeDataChanged();
}

Q_INVOKABLE void YT_PathWidgetModel::addSrcData(const int index, const QString& data)
{
    auto list = srcData.mid(0, index + 1);
    list.append(data);
    setSrcData(list);
}

Q_INVOKABLE QList<QString> YT_PathWidgetModel::getSubPath(const int index)
{
    return getSubPathFunc ? getSubPathFunc(index) : QList<QString>();
}

QString YT_PathWidgetModel::getWidgetPath() const
{
    return widgetPath;
}

QList<QString> YT_PathWidgetModel::getSrcData() const
{
    return srcData;
}

QList<QString> YT_PathWidgetModel::getPathData() const
{
    return pathData;
}

QList<QString> YT_PathWidgetModel::getTypeData() const
{
    return typeData;
}

void YT_PathWidgetModel::parseSrcData(const QString& data, QString& path_data, QString& type_data)
{
    static const QRegularExpression re(R"(^\(([^)]+)\)(.*)$)");
    const auto match = re.match(data);
    if (match.hasMatch()) {
        type_data = match.captured(1);
        path_data = match.captured(2);
    }
    else {
        type_data.clear();
        path_data = data;
    }
}
