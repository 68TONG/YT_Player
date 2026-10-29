#include "YT_VariantListModel.h"

YT_VariantListModel::YT_VariantListModel(QObject* parent)
    : QAbstractItemModel{ parent }
{
    {}
}

QModelIndex YT_VariantListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || row >= dataList.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex YT_VariantListModel::parent(const QModelIndex& index) const
{
    return {};
}

int YT_VariantListModel::rowCount(const QModelIndex& parent) const
{
    return dataList.count();
}

int YT_VariantListModel::columnCount(const QModelIndex& parent) const
{
    return +true;
}

QHash<int, QByteArray> YT_VariantListModel::roleNames() const
{
    return {
        { Qt::DisplayRole, "modelData" }
    };
}

QVariant YT_VariantListModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false) return {};
    return dataList[index.row()];
}

Q_INVOKABLE int YT_VariantListModel::count() const
{
    return dataList.count();
}

Q_INVOKABLE QVariant YT_VariantListModel::get(int index) const
{
    return dataList.value(index);
}

Q_INVOKABLE void YT_VariantListModel::set(int index, const QVariant& data)
{
    const auto model_index = this->index(index, 0);
    if (model_index.isValid() == false) return;

    dataList[index] = data;
    emit this->dataChanged(model_index, model_index);
}

Q_INVOKABLE void YT_VariantListModel::reset(const QList<QVariant>& data)
{
    this->beginResetModel();
    dataList = data;
    this->endResetModel();
}

Q_INVOKABLE void YT_VariantListModel::append(const QVariant& data)
{
    insert(dataList.count(), data);
}

Q_INVOKABLE void YT_VariantListModel::insert(int index, const QVariant& data)
{
    if (index < 0 || index > dataList.count()) return;
    this->beginInsertRows({}, index, index);
    dataList.insert(index, data);
    this->endInsertRows();
}

Q_INVOKABLE void YT_VariantListModel::move(int index, int at_index)
{
    this->beginMoveRows(QModelIndex(), index, index, QModelIndex(), at_index + (at_index > index));
    dataList.move(index, at_index);
    this->endMoveRows();
}

Q_INVOKABLE void YT_VariantListModel::remove(int index, int count)
{
    this->beginRemoveRows({}, index, index + count - 1);
    dataList.remove(index, count);
    this->endRemoveRows();
}

Q_INVOKABLE void YT_VariantListModel::clear()
{
    this->beginResetModel();
    dataList.clear();
    this->endResetModel();
}
