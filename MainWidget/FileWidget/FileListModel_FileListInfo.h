#pragma once

#include <DB_Info.h>
#include <YT_AbstractTreeModel.h>

class FileListModel_FileListInfo : public YT_AbstractTreeModel
{
    Q_OBJECT
public:
    explicit FileListModel_FileListInfo(QObject* parent = nullptr);

    virtual int beginInsertChildNode(YT_AbstractTreeNode* node, QString data) override;
    virtual void endInsertChildNode(YT_AbstractTreeNode* node, QString data) override;
    virtual int beginRemoveChildNode(YT_AbstractTreeNode* node) override;
    virtual void endRemoveChildNode(YT_AbstractTreeNode* node) override;

    virtual void loadChildNode(YT_AbstractTreeNode* node) override;
    virtual bool isLoadChildNode(YT_AbstractTreeNode* node) override;
private:
    void initNodeData(YT_AbstractTreeNode* node, QString name);
    const QStringList listInfoTypes = { DB_ListInfo::I_Type_Music, DB_ListInfo::I_Type_Video, DB_ListInfo::I_Type_Picture };
};
