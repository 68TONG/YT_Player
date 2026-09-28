#include "FileListModel_LinkPathInfo.h"

#include "FileListModel.h"
#include "DB_DatabaseInstance.h"

FileListModel_LinkPathInfo::FileListModel_LinkPathInfo(QObject* parent)
    : YT_AbstractTreeModel{ parent }
{
    rootNode.m_data.resize(FileListModel::ListInfoRole_End);
    rootNode.m_data[FileListModel::ListName_Role] = "LinkPathInfo";
    rootNode.m_data[FileListModel::ListPath_Role] = "LinkPathInfo";
    loadChildNode(&rootNode);
}

int FileListModel_LinkPathInfo::beginInsertChildNode(YT_AbstractTreeNode* node, QString data)
{
    return -1;
    // if (node->m_model != this) return -1;
    // if (DB_DatabaseInstance::obj().getListInfo(data, DB_ListInfo::I_IsValid).toBool() == true) return -2;
    // return node->childCount();
}

void FileListModel_LinkPathInfo::endInsertChildNode(YT_AbstractTreeNode* node, QString data)
{
    // if (node->m_model != this)  return;
    // DB_DatabaseInstance::obj().addListInfo(DB_ListInfo(data));

    // auto child_node_ptr = std::make_unique<YT_AbstractTreeNode>(this, node);
    // auto child_node = child_node_ptr.get();

    // initNodeData(child_node, data);
    // node->appendChild(std::move(child_node_ptr));
}

int FileListModel_LinkPathInfo::beginRemoveChildNode(YT_AbstractTreeNode* node)
{
    return -1;
    // if (node->m_model != this) return -1;

    // const auto& info_id = node->m_data[FileListModel::ListName_Role].toString();
    // if (DB_DatabaseInstance::obj().deleteListInfo(info_id) < 0) return -2;

    // return node->index();
}

void FileListModel_LinkPathInfo::endRemoveChildNode(YT_AbstractTreeNode* node)
{
    // if (node->m_model != this) return;

    // auto parent_node = node->parent();
    // if (parent_node == nullptr) return;

    // parent_node->removeChild(node->index());
}

void FileListModel_LinkPathInfo::loadChildNode(YT_AbstractTreeNode* node)
{
    // if (node == &rootNode) {
    //     const auto info_list = DB_DatabaseInstance::obj().getListInfo_NameAll();
    //     for (const auto& it : info_list) {
    //         auto child_node = std::make_unique<YT_AbstractTreeNode>(this, node);
    //         initNodeData(child_node.get(), it);
    //         node->appendChild(std::move(child_node));
    //     }
    //     return;
    // }
}

bool FileListModel_LinkPathInfo::isLoadChildNode(YT_AbstractTreeNode* node)
{
    // if (node == &rootNode) {
    //     return node->childCount() != DB_DatabaseInstance::obj().getListInfo_NameAll().count();
    // }
    return false;
}

void FileListModel_LinkPathInfo::initNodeData(YT_AbstractTreeNode* node, QString path)
{
    const auto list_path = node->parent()->m_data[FileListModel::ListPath_Role].toString() + '/' + path;

    node->m_data.resize(FileListModel::ListInfoRole_End);
    node->m_data[FileListModel::ListName_Role] = path;
    node->m_data[FileListModel::ListPath_Role] = list_path;
}
