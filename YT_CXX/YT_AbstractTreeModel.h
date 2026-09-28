#pragma once

#include <QObject>
#include <YT_AbstractTreeNode.h>

class YT_AbstractTreeModel : public QObject
{
    Q_OBJECT
public:
    explicit YT_AbstractTreeModel(QObject* parent = nullptr);

    virtual int beginInsertChildNode(YT_AbstractTreeNode* node, QString data);
    virtual void endInsertChildNode(YT_AbstractTreeNode* node, QString data);
    virtual int beginRemoveChildNode(YT_AbstractTreeNode* node);
    virtual void endRemoveChildNode(YT_AbstractTreeNode* node);

    virtual void loadChildNode(YT_AbstractTreeNode* node) = 0;
    virtual bool isLoadChildNode(YT_AbstractTreeNode* node) = 0;

    YT_AbstractTreeNode rootNode;
signals:
    void emitAddNode(YT_AbstractTreeNode* node, int index);
    void emitRemoveNode(YT_AbstractTreeNode* node, int index);
};