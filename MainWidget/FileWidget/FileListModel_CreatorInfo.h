#pragma once

#include <YT_AbstractTreeModel.h>

class FileListModel_CreatorInfo : public YT_AbstractTreeModel
{
    Q_OBJECT
public:
    explicit FileListModel_CreatorInfo(QObject* parent = nullptr);

    virtual void loadChildNode(YT_AbstractTreeNode* node) override;
    virtual bool isLoadChildNode(YT_AbstractTreeNode* node) override;
};
