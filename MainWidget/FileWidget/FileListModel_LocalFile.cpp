#include "FileListModel_LocalFile.h"

#include "FileListModel.h"

FileListModel_LocalFile::FileListModel_LocalFile(QString rootPath, QObject* parent)
    : rootPath(rootPath), YT_AbstractTreeModel(parent)
{
    rootNode.m_data.resize(FileListModel::ListInfoRole_End);
    rootNode.m_data[FileListModel::ListName_Role] = QDir(rootPath).isRoot() ? rootPath.mid(0, 2) : QFileInfo(rootPath).fileName();
    rootNode.m_data[FileListModel::ListPath_Role] = rootPath;
    rootNode.m_data[FileListModel::ListType_Role] = FileListModel::LocalFileType;
    loadChildNode(&rootNode);
}

int FileListModel_LocalFile::beginInsertChildNode(YT_AbstractTreeNode* node, QString data)
{
    if (node->m_model != this) return -1;

    const auto& file_path = node->m_data[FileListModel::ListPath_Role].toString() + '/' + data;
    if (QFile::exists(file_path)) return -2;
    if (QDir().mkpath(file_path) == false)  return -3;

    return node->childCount();
}

void FileListModel_LocalFile::endInsertChildNode(YT_AbstractTreeNode* node, QString data)
{
    if (node->m_model != this)  return;

    const auto& file_path = node->m_data[FileListModel::ListPath_Role].toString() + '/' + data;
    auto child_node_ptr = std::make_unique<YT_AbstractTreeNode>(this, node);
    auto child_node = child_node_ptr.get();

    initNodeData(child_node, QFileInfo(file_path));
    node->appendChild(std::move(child_node_ptr));
    // emit emitAddNode(node, child_node->index());
}

int FileListModel_LocalFile::beginRemoveChildNode(YT_AbstractTreeNode* node)
{
    if (node->m_model != this) return -1;

    const auto& file_path = node->m_data[FileListModel::ListPath_Role].toString();
    if (QFile::moveToTrash(file_path, nullptr) == false) {
        return -2;
    }

    return node->index();
}

void FileListModel_LocalFile::endRemoveChildNode(YT_AbstractTreeNode* node)
{
    if (node->m_model != this) return;

    auto parent_node = node->parent();
    if (parent_node == nullptr) return;

    parent_node->removeChild(node->index());
    // emit emitRemoveNode(parent_node, node->index());
}

void FileListModel_LocalFile::loadChildNode(YT_AbstractTreeNode* node)
{
    if (node->m_model != this) {
        return;
    }

    const auto file_path = node->m_data[FileListModel::ListPath_Role].toString();
    QFileInfo file_info(file_path);
    if (file_info.isDir() == false) {
        return;
    }

    auto info_list = QDir(file_path).entryInfoList(fileFilter);
    for (auto info : info_list) {
        auto child_node = std::make_unique<YT_AbstractTreeNode>(this, node);
        initNodeData(child_node.get(), info);
        node->appendChild(std::move(child_node));
    }
}

bool FileListModel_LocalFile::isLoadChildNode(YT_AbstractTreeNode* node)
{
    if (node->m_model != this) {
        return false;
    }

    const auto file_path = node->m_data[FileListModel::ListPath_Role].toString();
    QFileInfo file_info(file_path);
    if (file_info.isDir() == false) {
        return false;
    }

    return node->childCount() != QDir(file_path).entryList(fileFilter).count();
}

void FileListModel_LocalFile::initNodeData(YT_AbstractTreeNode* node, const QFileInfo& info)
{
    node->m_data.resize(FileListModel::ListInfoRole_End);
    node->m_data[FileListModel::ListName_Role] = info.fileName();
    node->m_data[FileListModel::ListPath_Role] = info.filePath();
    node->m_data[FileListModel::ListType_Role] = FileListModel::LocalFileType;
}
