#include "FileListModel_CreatorInfo.h"

#include <DB_DataBase.h>
#include "FileListModel.h"

FileListModel_CreatorInfo::FileListModel_CreatorInfo(QObject* parent)
    : YT_AbstractTreeModel(parent)
{
    rootNode.m_data.resize(FileListModel::ListInfoRole_End);
    rootNode.m_data[FileListModel::ListName_Role] = "CreatorInfo";
    rootNode.m_data[FileListModel::ListPath_Role] = "CreatorInfo";
    rootNode.m_data[FileListModel::ListType_Role] = FileListModel::CreatorInfoType;
    loadChildNode(&rootNode);
}

void FileListModel_CreatorInfo::loadChildNode(YT_AbstractTreeNode* node)
{
    static const auto initDataFunc = [](YT_AbstractTreeNode* node, QString name) {
        const auto list_path = node->parent()->m_data[FileListModel::ListPath_Role].toString() + '/' + name;

        node->m_data.resize(FileListModel::ListInfoRole_End);
        node->m_data[FileListModel::ListName_Role] = name;
        node->m_data[FileListModel::ListPath_Role] = list_path;
        node->m_data[FileListModel::ListType_Role] = FileListModel::CreatorInfoType;
        };

    if (node == &rootNode) {
        const auto info_list = DB::object().getCreatorInfo_NameAll();
        for (const auto& it : info_list) {
            auto child_node = std::make_unique<YT_AbstractTreeNode>(this, node);
            initDataFunc(child_node.get(), it);
            node->appendChild(std::move(child_node));
        }
        return;
    }

    const auto info_id = node->m_data[FileListModel::ListName_Role].toString();
    if (info_id.isEmpty()) {
        return;
    }

    const auto tag_values = DB::object().getFileInfo_TagValues_CreatorId(info_id);
    for (const auto& tag_value : tag_values) {
        auto child_node = std::make_unique<YT_AbstractTreeNode>(this, node);
        initDataFunc(child_node.get(), tag_value);
        node->appendChild(std::move(child_node));
    }
}

bool FileListModel_CreatorInfo::isLoadChildNode(YT_AbstractTreeNode* node)
{
    if (node == &rootNode) {
        return node->childCount() != DB::object().getCreatorInfo_NameAll().count();
    }

    if (node->childCount() > 0) {
        return false;
    }

    const auto info_id = node->m_data[FileListModel::ListName_Role].toString();
    if (info_id.isEmpty()) {
        return false;
    }

    return DB::object().getFileInfo_TagValues_CreatorId(info_id).isEmpty() == false;
}
