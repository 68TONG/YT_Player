#include "YT_AbstractTreeProxyModel.h"

YT_AbstractTreeProxyModel::YT_AbstractTreeProxyModel(QObject* parent)
    : QAbstractProxyModel{ parent }
{
    // 构造函数，初始化代理模型对象。
}

QAbstractItemModel* YT_AbstractTreeProxyModel::m_sourceModel() const
{
    return this->sourceModel();
}

void YT_AbstractTreeProxyModel::setM_sourceModel(QAbstractItemModel* model)
{
    this->setSourceModel(model);
}

void YT_AbstractTreeProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    // 设置源模型，并用删除/插入的方式重建当前可见行。
    if (sourceModel == this->sourceModel()) {
        return;
    }
    if (this->sourceModel() != nullptr) {
        disconnect(this->sourceModel(), &QAbstractItemModel::rowsInserted, this, &YT_AbstractTreeProxyModel::slotRowsInserted);
        disconnect(this->sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, &YT_AbstractTreeProxyModel::slotRowsRemoved);
    }

    QAbstractProxyModel::setSourceModel(sourceModel);
    this->beginResetModel();

    indexList.clear();
    indexExpandHash.clear();
    if (this->sourceModel() != nullptr) {
        for (int i = 0; i < this->sourceModel()->rowCount(); i++) {
            const auto sourceIndex = this->sourceModel()->index(i, 0, QModelIndex());
            indexList.push_back(QPersistentModelIndex(sourceIndex));
        }

        connect(this->sourceModel(), &QAbstractItemModel::rowsInserted, this, &YT_AbstractTreeProxyModel::slotRowsInserted);
        connect(this->sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, &YT_AbstractTreeProxyModel::slotRowsRemoved);
    }

    this->endResetModel();
    emit m_sourceModelChanged();
}

QModelIndex YT_AbstractTreeProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    // 将源模型索引映射为代理模型索引。
    if (sourceIndex.isValid() == false || this->sourceModel() == nullptr) {
        return {};
    }

    for (int i = 0; i < indexList.size(); i++) {
        if (indexList[i] == sourceIndex) {
            return this->index(i, 0, QModelIndex());
        }
    }

    return {};
}

QModelIndex YT_AbstractTreeProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    // 将代理模型索引映射回源模型索引。
    if (proxyIndex.isValid() == false || proxyIndex.model() != this) {
        return {};
    }

    if (proxyIndex.row() < 0 || proxyIndex.row() >= indexList.size()) {
        return {};
    }

    return indexList[proxyIndex.row()];
}

QModelIndex YT_AbstractTreeProxyModel::index(int row, int column, const QModelIndex& parent) const
{
    // 生成当前可见行对应的代理模型索引。
    if (parent.isValid() == true || row < 0 || row >= indexList.size() || column < 0 || column >= this->columnCount()) {
        return {};
    }

    return this->createIndex(row, column);
}

QModelIndex YT_AbstractTreeProxyModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index)
        return {};
}

int YT_AbstractTreeProxyModel::rowCount(const QModelIndex& parent) const
{
    // 只暴露一维展开后的可见行数量。
    if (parent.isValid() == true) {
        return 0;
    }

    return indexList.size();
}

int YT_AbstractTreeProxyModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
        if (this->sourceModel() == nullptr) {
            return 0;
        }

    return this->sourceModel()->columnCount();
}

QVariant YT_AbstractTreeProxyModel::data(const QModelIndex& index, int role) const
{
    if (role == ItemDepth_Role) {
        return depth(index.row());
    }
    else if (role == ItemExpanded_Role) {
        return isExpanded(index.row());
    }
    else if (role == ItemHasChildren_Role) {
        return this->hasChildren(index);
    }

    // 将代理模型的数据请求转发到源模型。
    role = role - ModelDataRole_End;
    const QModelIndex sourceIndex = mapToSource(index);
    if (sourceIndex.isValid() == false || this->sourceModel() == nullptr) {
        return {};
    }

    return this->sourceModel()->data(sourceIndex, role);
}

QHash<int, QByteArray> YT_AbstractTreeProxyModel::roleNames() const
{
    // 透传源模型的角色名称。
    if (this->sourceModel() == nullptr) {
        return {};
    }

    QHash<int, QByteArray> ret_data = {
        {ItemDepth_Role, "depth"},
        {ItemExpanded_Role, "expanded"},
        {ItemHasChildren_Role,"hasChildren"}
    };

    auto src_data = this->sourceModel()->roleNames();
    for (auto [key, value] : src_data.asKeyValueRange()) {
        ret_data.insert(key + ModelDataRole_End, value);
    }
    return ret_data;
}

Q_INVOKABLE void YT_AbstractTreeProxyModel::expand(int index)
{
    const auto thisIndex = this->createIndex(index, 0);
    const auto sourceIndex = normalizeIndex(thisIndex);
    if (sourceIndex.isValid() == false || this->sourceModel() == nullptr) {
        return;
    }

    if (indexExpandHash.contains(sourceIndex) == true) {
        return;
    }

    if (this->canFetchMore(thisIndex)) {
        this->fetchMore(thisIndex);
    }

    indexExpandHash.insert(QPersistentModelIndex(sourceIndex));
    emit this->dataChanged(thisIndex, thisIndex, { ItemExpanded_Role });

    const auto index_list = getExpandIndexList(sourceIndex);
    if (index_list.isEmpty() == true) {
        return;
    }

    const int startIndex = index + 1;
    const int endIndex = startIndex + index_list.size() - 1;
    this->beginInsertRows(QModelIndex(), startIndex, endIndex);

    // for (int i = index_list.size() - 1; i >= 0; --i) {
    //     indexList.insert(startIndex, index_list[i]);
    // }
    indexList.insert(startIndex, index_list.count(), {});
    for (int i = 0; i < index_list.size(); ++i) {
        indexList[startIndex + i] = QPersistentModelIndex(index_list[i]);
    }
    // std::copy(index_list.begin(), index_list.end(), indexList.begin() + startIndex);

    this->endInsertRows();
}

Q_INVOKABLE void YT_AbstractTreeProxyModel::collapse(int index)
{
    const auto thisIndex = this->createIndex(index, 0);
    const auto sourceIndex = normalizeIndex(thisIndex);
    if (sourceIndex.isValid() == false || this->sourceModel() == nullptr) {
        return;
    }

    if (indexExpandHash.contains(sourceIndex) == false) {
        return;
    }

    indexExpandHash.remove(sourceIndex);
    emit dataChanged(thisIndex, thisIndex, { ItemExpanded_Role });

    const int startIndex = index + 1;
    int endIndex = startIndex;
    while (endIndex < indexList.size() && isIndexParent(indexList[endIndex], sourceIndex) == true) {
        ++endIndex;
    }
    if (endIndex <= startIndex) {
        return;
    }

    this->beginRemoveRows(QModelIndex(), startIndex, endIndex - 1);

    indexList.erase(indexList.begin() + startIndex, indexList.begin() + endIndex);

    this->endRemoveRows();
}

Q_INVOKABLE bool YT_AbstractTreeProxyModel::isExpanded(int index) const
{
    // 判断指定节点当前是否处于展开状态。
    const QModelIndex sourceIndex = normalizeIndex(this->createIndex(index, 0));
    if (sourceIndex.isValid() == false) {
        return false;
    }

    return indexExpandHash.contains(sourceIndex);
}

Q_INVOKABLE void YT_AbstractTreeProxyModel::toggleExpanded(int index)
{
    isExpanded(index) ? collapse(index) : expand(index);
}

Q_INVOKABLE int YT_AbstractTreeProxyModel::depth(int index) const
{
    const QModelIndex sourceIndex = normalizeIndex(this->createIndex(index, 0));
    if (sourceIndex.isValid() == false) {
        return 0;
    }

    int depth = 0;
    QModelIndex current = sourceIndex;
    while (current.isValid()) {
        current = current.parent();
        if (current.isValid()) depth++;
    }
    return depth;
}

QList<QModelIndex> YT_AbstractTreeProxyModel::getExpandIndexList(const QModelIndex& sourceIndex) const
{
    if (sourceIndex.isValid() == false || this->sourceModel() == nullptr) {
        return {};
    }

    QList<QModelIndex> ret_;
    for (int i = 0; i < this->sourceModel()->rowCount(sourceIndex); i++) {
        const auto childIndex = this->sourceModel()->index(i, 0, sourceIndex);
        ret_.append(childIndex);

        if (indexExpandHash.contains(childIndex)) {
            ret_.append(getExpandIndexList(childIndex));
        }
    }
    return ret_;
}

bool YT_AbstractTreeProxyModel::isIndexParent(const QModelIndex& index, const QModelIndex& parent) const
{
    QModelIndex current = index;
    while (current.isValid() == true) {
        if (current == parent) {
            return true;
        }
        current = current.parent();
    }
    return false;
}

QModelIndex YT_AbstractTreeProxyModel::normalizeIndex(const QModelIndex& index) const
{
    // 将代理索引或源索引规范化为源模型索引。
    if (index.isValid() == false) {
        return {};
    }

    if (index.model() == this) {
        return this->mapToSource(index);
    }

    if (index.model() == this->sourceModel()) {
        return index;
    }

    return {};
}

void YT_AbstractTreeProxyModel::slotRowsInserted(const QModelIndex& parent, int start, int end)
{
    if (indexExpandHash.contains(parent) == false) {
        return;
    }

    QList<QModelIndex> index_list;
    for (int i = start; i <= end; ++i) {
        const auto sourceIndex = this->sourceModel()->index(i, 0, parent);
        if (sourceIndex.isValid() == false) {
            continue;
        }

        index_list.append(sourceIndex);
    }

    const int index = this->mapFromSource(parent).row();
    if (index < 0 || index_list.isEmpty()) return;

    const int startIndex = index + start + 1;
    const int endIndex = startIndex + index_list.size() - 1;
    this->beginInsertRows(QModelIndex(), startIndex, endIndex);

    indexList.insert(startIndex, index_list.count(), {});
    for (int i = 0; i < index_list.size(); ++i) {
        indexList[startIndex + i] = QPersistentModelIndex(index_list[i]);
    }
    // std::copy(index_list.begin(), index_list.end(), indexList.begin() + startIndex);

    this->endInsertRows();
}

void YT_AbstractTreeProxyModel::slotRowsRemoved(const QModelIndex& parent, int start, int end)
{
    if (indexExpandHash.contains(parent) == false) {
        return;
    }

    QList<QModelIndex> index_list;
    for (int i = start; i <= end; ++i) {
        const auto sourceIndex = this->sourceModel()->index(i, 0, parent);
        if (sourceIndex.isValid() == false) {
            continue;
        }
        index_list.append(sourceIndex);
        index_list.append(getExpandIndexList(sourceIndex));
    }

    const int index = this->mapFromSource(parent).row();
    if (index < 0 || index_list.isEmpty()) return;

    const int startIndex = index + start + 1;
    const int endIndex = startIndex + index_list.size();
    this->beginRemoveRows(QModelIndex(), startIndex, endIndex - 1);

    indexList.erase(indexList.begin() + startIndex, indexList.begin() + endIndex);
    for (const auto& it : index_list) {
        indexExpandHash.remove(it);
    }

    this->endRemoveRows();
}