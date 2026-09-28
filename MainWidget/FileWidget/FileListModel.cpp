#include "FileListModel.h"

#include "FileListModel_LocalFile.h"
#include "FileListModel_CreatorInfo.h"
#include "FileListModel_FileListInfo.h"

FileListModel::FileListModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    modelList.append(new FileListModel_LocalFile("C:/", this));
    modelList.append(new FileListModel_CreatorInfo(this));
    modelList.append(new FileListModel_FileListInfo(this));

    // for (auto model : modelList) {
    //     connect(model, &YT_AbstractTreeModel::emitAddNode, this, &FileListModel::onSubModelAddNode);
    //     connect(model, &YT_AbstractTreeModel::emitRemoveNode, this, &FileListModel::onSubModelRemoveNode);
    // }
}

QModelIndex FileListModel::index(const YT_AbstractTreeNode* node) const
{
    if (node == nullptr) {
        return {};
    }

    const auto parent_node = node->parent();
    const auto index = parent_node ? node->index() : modelList.indexOf(node->m_model);
    if (index < 0) {
        return {};
    }

    return this->createIndex(index, 0, parent_node ? node : &modelList[index]->rootNode);
}

QModelIndex FileListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (this->hasIndex(row, column, parent) == false)
        return {};

    if (parent.isValid()) {
        auto node = static_cast<YT_AbstractTreeNode*>(parent.internalPointer());
        if (auto child_node = node->child(row))
            return this->createIndex(row, column, child_node);
    }
    else if (row >= 0 && row < modelList.count()) {
        return this->createIndex(row, column, &modelList[row]->rootNode);
    }

    return {};
}

QModelIndex FileListModel::parent(const QModelIndex& index) const
{
    if (index.isValid() == false)
        return {};

    auto parent_node = static_cast<YT_AbstractTreeNode*>(index.internalPointer())->parent();
    if (parent_node == nullptr) {
        return {};
    }
    if (parent_node->parent()) {
        return this->createIndex(parent_node->index(), 0, parent_node);
    }

    for (int i = 0;i < modelList.count();i++) {
        if (&modelList[i]->rootNode == parent_node) {
            return this->createIndex(i, 0, parent_node);
        }
    }
    return {};
}

int FileListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() == false) {
        return modelList.count();
    }

    auto parent_node = static_cast<YT_AbstractTreeNode*>(parent.internalPointer());
    return parent_node->childCount();
}

int FileListModel::columnCount(const QModelIndex& parent) const
{
    return +true;
}

QHash<int, QByteArray> FileListModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames = {
        {ListName_Role, "listName"},
        {ListPath_Role, "listPath"},
        {ListType_Role, "listType"}
    };
    return roleNames;
}

QVariant FileListModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false)
        return {};

    const auto node = static_cast<YT_AbstractTreeNode*>(index.internalPointer());
    return node->m_data[role];
}

void FileListModel::fetchMore(const QModelIndex& parent)
{
    if (parent.isValid() == false) {
        return;
    }

    auto node = static_cast<YT_AbstractTreeNode*>(parent.internalPointer());

    if (node->childCount()) {
        this->beginRemoveRows(parent, 0, node->childCount() - 1);
        node->clearChild();
        this->endRemoveRows();
    }

    node->m_model->loadChildNode(node);
    this->beginInsertRows(parent, 0, node->childCount() - 1);
    this->endInsertRows();
}

bool FileListModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid() == false) {
        return false;
    }

    auto node = static_cast<YT_AbstractTreeNode*>(parent.internalPointer());
    return node->m_model->isLoadChildNode(node);
}

bool FileListModel::hasChildren(const QModelIndex& parent) const
{
    if (parent.isValid() == false) {
        return true;
    }

    auto node = static_cast<YT_AbstractTreeNode*>(parent.internalPointer());
    return node->childCount() || this->canFetchMore(parent);
}

Q_INVOKABLE void FileListModel::menuAdd(const QModelIndex& index, QString data)
{
    if (index.isValid() == false) {
        return;
    }

    auto node = static_cast<YT_AbstractTreeNode*>(index.internalPointer());
    const int operation_index = node->m_model->beginInsertChildNode(node, data);
    if (operation_index < 0) return;

    this->beginInsertRows(index, operation_index, operation_index);
    node->m_model->endInsertChildNode(node, data);
    this->endInsertRows();
}

Q_INVOKABLE void FileListModel::menuRemove(const QModelIndex& index)
{
    if (index.isValid() == false) {
        return;
    }

    auto node = static_cast<YT_AbstractTreeNode*>(index.internalPointer());
    auto parent_index = this->index(node->parent());
    if (parent_index.isValid() == false) {
        return;
    }

    const int operation_index = node->m_model->beginRemoveChildNode(node);
    if (operation_index < 0) return;

    this->beginRemoveRows(parent_index, operation_index, operation_index);
    node->m_model->endRemoveChildNode(node);
    this->endRemoveRows();
}