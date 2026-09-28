#include "YT_AbstractTreeModel.h"

YT_AbstractTreeModel::YT_AbstractTreeModel(QObject* parent)
    : rootNode(this), QObject(parent)
{
    {}
}

int YT_AbstractTreeModel::beginInsertChildNode(YT_AbstractTreeNode* node, QString data)
{
    return -1;
}

void YT_AbstractTreeModel::endInsertChildNode(YT_AbstractTreeNode* node, QString data)
{
    {}
}

int YT_AbstractTreeModel::beginRemoveChildNode(YT_AbstractTreeNode* node)
{
    return -1;
}

void YT_AbstractTreeModel::endRemoveChildNode(YT_AbstractTreeNode* node)
{
    {}
}