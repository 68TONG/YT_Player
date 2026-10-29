#include "WidgetModel.h"

#include <QQmlEngine>
#include <QRegularExpression>

#include "YT_IconProvider.h"

WidgetModel::WidgetModel(QObject* parent)
    : QAbstractItemModel{ parent }
{
    connect(&pathModel, &YT_VariantListModel::rowsInserted, this, &WidgetModel::pathModelDataChanged);
    connect(&pathModel, &YT_VariantListModel::rowsRemoved, this, &WidgetModel::pathModelDataChanged);
    connect(&pathModel, &YT_VariantListModel::rowsMoved, this, &WidgetModel::pathModelDataChanged);
    connect(&pathModel, &YT_VariantListModel::modelReset, this, &WidgetModel::pathModelDataChanged);
    connect(&pathModel, &YT_VariantListModel::dataChanged, this, &WidgetModel::pathModelDataChanged);

    widgetDatas.append(QVariantList{
        QVariant::fromValue(WidgetType_File),
        QVariant("File"),
        QVariant::fromValue(YT_IconProvider::IconIndex::FolderCopy),
        QVariant("YT_FileWidget.qml")
        });
    widgetDatas.append(QVariantList{
        QVariant::fromValue(WidgetType_Music),
        QVariant("Music"),
        QVariant::fromValue(YT_IconProvider::IconIndex::LibraryMusic),
        QVariant("YT_MusicWidget.qml")
        });
    widgetDatas.append(QVariantList{
        QVariant::fromValue(WidgetType_Video),
        QVariant("Video"),
        QVariant::fromValue(0),
        QVariant("Item.qml")
        });
    widgetDatas.append(QVariantList{
        QVariant::fromValue(WidgetType_Picture),
        QVariant("Picture"),
        QVariant::fromValue(0),
        QVariant("Item.qml")
        });
    widgetDatas.append(QVariantList{
        QVariant::fromValue(WidgetType_Creator),
        QVariant("Creator"),
        QVariant::fromValue(0),
        QVariant("Item.qml")
        });
    widgetDatas.append(QVariantList{
        QVariant::fromValue(WidgetType_Edit),
        QVariant("Edit"),
        QVariant::fromValue(YT_IconProvider::IconIndex::EditSquare),
        QVariant("Item.qml")
        });
    widgetDatas.append(QVariantList{
        QVariant::fromValue(WidgetType_Set),
        QVariant("Set"),
        QVariant::fromValue(YT_IconProvider::IconIndex::Settings),
        QVariant("YT_SetWidget.qml")
        });
    widgetDatas.append(QVariantList{
        QVariant::fromValue(WidgetType_Test),
        QVariant("Test"),
        QVariant::fromValue(YT_IconProvider::IconIndex::FrameBug),
        QVariant("YT_TestWidget.qml")
        });
}

WidgetModel& WidgetModel::obj()
{
    static WidgetModel object;
    return object;
}

WidgetModel* WidgetModel::create(QQmlEngine* qml_engine, QJSEngine*)
{
    QQmlEngine::setObjectOwnership(&obj(), QQmlEngine::CppOwnership);
    return &obj();
}

QModelIndex WidgetModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || row >= widgetDatas.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex WidgetModel::parent(const QModelIndex& index) const
{
    return {};
}

int WidgetModel::rowCount(const QModelIndex& parent) const
{
    return widgetDatas.count();
}

int WidgetModel::columnCount(const QModelIndex& parent) const
{
    return +true;
}

QHash<int, QByteArray> WidgetModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames = {
      { TypeRole, "widgetType" },
      { TitleRole, "widgetTitle" },
      { LogoIconRole, "widgetLogoIcon" },
      { ComponentRole, "widgetComponent" }
    };
    return roleNames;
}

QVariant WidgetModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false) return {};
    auto info = widgetDatas[index.row()];
    return info[role];
}

Q_INVOKABLE QVariant WidgetModel::data_Index(const int& index, int role) const
{
    return this->data(this->index(index, 0), role);
}

Q_INVOKABLE QString WidgetModel::getPath() const
{
    return getPathData().join("/");
}

Q_INVOKABLE QString WidgetModel::getPathEx() const
{
    return widgetDatas[widgetType][TitleRole].toString() + '/' + getPath();
}

QList<QString> WidgetModel::getPathData() const
{
    QList<QString> ret;
    ret.reserve(pathModel.count());
    for (int i = 0;i < pathModel.count();i++) {
        ret.append(pathModel.get(i).toString());
    }
    return ret;
}

Q_INVOKABLE void WidgetModel::setPath(int index)
{
    pathModel.remove(index + 1, pathModel.count() - (index + 1));
}

Q_INVOKABLE void WidgetModel::setPath(QString data)
{
    static const QRegularExpression re(R"([/\\])");
    setPath(data.split(re));
}

Q_INVOKABLE void WidgetModel::setPath(QList<QString> data)
{
    data.removeAll("");
    if (data.isEmpty()) return;
    for (const auto& it : widgetDatas) {
        if (it[TitleRole] == data[0]) {
            data.removeFirst();
            widgetType = it[TypeRole].value<WidgetType>();
            emit widgetTypeChanged();
            break;
        }
    }
    if (data.isEmpty()) return;
    if (data.count() == pathModel.count()) {
        bool is = false;
        for (int i = 0;i < data.count();i++) {
            if (data[i] != pathModel.get(i).toString()) {
                is = true;
                break;
            }
        }
        if (is == false) return;
    }

    QVariantList data_list;
    data_list.reserve(data.size());
    for (const auto& it : data) {
        data_list.append(it);
    }
    pathModel.reset(data_list);
}

Q_INVOKABLE void WidgetModel::addPath(QString data)
{
    if (data.isEmpty()) return;
    pathModel.append(data);
}

Q_INVOKABLE void WidgetModel::addPath(int index, QString data)
{
    setPath(index);
    addPath(data);
}

Q_INVOKABLE QList<QString> WidgetModel::getSubPath(int index) const
{
    return {};
}

WidgetModel::WidgetType WidgetModel::getWidgetType() const
{
    return widgetType;
}

void WidgetModel::setWidgetType(const WidgetType& data)
{
    if (data < 0 || data >= widgetDatas.count() || widgetType == data) return;
    widgetType = data;
    emit widgetTypeChanged();

    // pathModel.reset({ widgetDatas[data][TitleRole].toString() });
}
