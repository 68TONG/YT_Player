#include "FileListModel_FileListInfo.h"

#include "FileListModel.h"
#include "DB_DatabaseInstance.h"

FileListModel_FileListInfo::FileListModel_FileListInfo(QObject* parent)
    : YT_AbstractTreeModel{ parent }
{
    rootNode.m_data.resize(FileListModel::ListInfoRole_End);
    rootNode.m_data[FileListModel::ListName_Role] = "FileListInfo";
    rootNode.m_data[FileListModel::ListPath_Role] = "FileListInfo";
    rootNode.m_data[FileListModel::ListType_Role] = FileListModel::FileListInfoType;
    loadChildNode(&rootNode);
}

int FileListModel_FileListInfo::beginInsertChildNode(YT_AbstractTreeNode* node, QString data)
{
    if (node->m_model != this) return -1;
    if (node->parent() != &rootNode) return -2;
    if (DB_DatabaseInstance::obj().getListInfo(data, DB_ListInfo::I_IsValid).toBool() == true) return -3;
    return node->childCount();
}

void FileListModel_FileListInfo::endInsertChildNode(YT_AbstractTreeNode* node, QString data)
{
    if (node->m_model != this)  return;
    DB_DatabaseInstance::obj().addListInfo(DB_ListInfo(data, node->m_data[FileListModel::ListName_Role].toString()));

    auto child_node_ptr = std::make_unique<YT_AbstractTreeNode>(this, node);
    auto child_node = child_node_ptr.get();

    initNodeData(child_node, data);
    node->appendChild(std::move(child_node_ptr));
}

int FileListModel_FileListInfo::beginRemoveChildNode(YT_AbstractTreeNode* node)
{
    if (node->m_model != this) return -1;
    if (node->parent() && node->parent()->parent() != &rootNode) return -2;

    const auto& info_id = node->m_data[FileListModel::ListName_Role].toString();
    if (DB_DatabaseInstance::obj().deleteListInfo(info_id) < 0) return -3;

    return node->index();
}

void FileListModel_FileListInfo::endRemoveChildNode(YT_AbstractTreeNode* node)
{
    if (node->m_model != this) return;

    auto parent_node = node->parent();
    if (parent_node == nullptr) return;

    parent_node->removeChild(node->index());
}

void FileListModel_FileListInfo::loadChildNode(YT_AbstractTreeNode* node)
{
    if (node == &rootNode) {
        for (const auto& it : listInfoTypes) {
            auto child_node = std::make_unique<YT_AbstractTreeNode>(this, node);
            initNodeData(child_node.get(), it);
            node->appendChild(std::move(child_node));
        }

        return;
    }

    const auto info_type = node->m_data[FileListModel::ListName_Role].toString();
    if (info_type.isEmpty()) {
        return;
    }

    const auto list_names = DB_DatabaseInstance::obj().getListInfo_NameAll(info_type);
    for (const auto& name : list_names) {
        auto child_node = std::make_unique<YT_AbstractTreeNode>(this, node);
        initNodeData(child_node.get(), name);
        node->appendChild(std::move(child_node));
    }
}

bool FileListModel_FileListInfo::isLoadChildNode(YT_AbstractTreeNode* node)
{
    if (node == &rootNode) {
        return node->childCount() != listInfoTypes.count();
    }

    if (node->childCount() > 0) {
        return false;
    }

    const auto info_type = node->m_data[FileListModel::ListName_Role].toString();
    if (info_type.isEmpty()) {
        return false;
    }

    return DB_DatabaseInstance::obj().getListInfo_NameAll(info_type).isEmpty() == false;
}

void FileListModel_FileListInfo::initNodeData(YT_AbstractTreeNode* node, QString name)
{
    const auto list_path = node->parent()->m_data[FileListModel::ListPath_Role].toString() + '/' + name;

    node->m_data.resize(FileListModel::ListInfoRole_End);
    node->m_data[FileListModel::ListName_Role] = name;
    node->m_data[FileListModel::ListPath_Role] = list_path;
    node->m_data[FileListModel::ListType_Role] = FileListModel::FileListInfoType;
}
