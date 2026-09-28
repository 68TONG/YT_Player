#pragma once

#include <YT_AbstractTreeModel.h>

class FileListModel_LocalFile : public YT_AbstractTreeModel
{
    Q_OBJECT
public:
    explicit FileListModel_LocalFile(QString rootPath, QObject* parent = nullptr);

    virtual int beginInsertChildNode(YT_AbstractTreeNode* node, QString data) override;
    virtual void endInsertChildNode(YT_AbstractTreeNode* node, QString data) override;
    virtual int beginRemoveChildNode(YT_AbstractTreeNode* node) override;
    virtual void endRemoveChildNode(YT_AbstractTreeNode* node) override;

    virtual void loadChildNode(YT_AbstractTreeNode* node) override;
    virtual bool isLoadChildNode(YT_AbstractTreeNode* node) override;
private:
    void initNodeData(YT_AbstractTreeNode* node, const QFileInfo& info);

    const QString rootPath;
    const QDir::Filters fileFilter = (QDir::Filter::AllDirs | QDir::Filter::NoDotAndDotDot);
};
