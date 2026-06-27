#include "YT_ListModel.h"

YT_ListModel::YT_ListModel(QObject* parent)
    : QAbstractItemModel{ parent }
{
}

QModelIndex YT_ListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || row >= dataList.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex YT_ListModel::parent(const QModelIndex& index) const
{
    return {};
}

int YT_ListModel::rowCount(const QModelIndex& parent) const
{
    return dataList.count();
}

int YT_ListModel::columnCount(const QModelIndex& parent) const
{
    return +true;
}

QHash<int, QByteArray> YT_ListModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "modelData"}
    };
}

QVariant YT_ListModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false) return {};
    return dataList[index.row()];
}

Q_INVOKABLE QVariant YT_ListModel::get(int index)
{
    return this->data(this->index(index, 0));
}

Q_INVOKABLE void YT_ListModel::set(int index, const QVariant& data)
{
    const auto model_index = this->index(index, 0);
    if (model_index.isValid() == false) return;

    dataList[index] = data;
    emit this->dataChanged(model_index, model_index);
}

Q_INVOKABLE void YT_ListModel::append(const QVariant& data)
{
    insert(dataList.count(), data);
}

Q_INVOKABLE void YT_ListModel::insert(int index, const QVariant& data)
{
    if (index < 0 || index > dataList.count()) return;
    this->beginInsertRows({}, index, index);
    dataList.insert(index, data);
    this->endInsertRows();
}

Q_INVOKABLE void YT_ListModel::move(int index, int at_index)
{
    this->beginMoveRows(QModelIndex(), index, index, QModelIndex(), at_index + (at_index > index));
    dataList.move(index, at_index);
    this->endMoveRows();
}

Q_INVOKABLE void YT_ListModel::remove(int index, int count)
{
    this->beginRemoveRows({}, index, index + count - 1);
    dataList.remove(index, count);
    this->endRemoveRows();
}

Q_INVOKABLE void YT_ListModel::clear()
{
    this->beginResetModel();
    dataList.clear();
    this->endResetModel();
}
