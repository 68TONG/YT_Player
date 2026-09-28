#pragma once

#include <YT_GeneralH.h>

class YT_AbstractTreeModel;
class YT_AbstractTreeNode
{
public:
    explicit YT_AbstractTreeNode(YT_AbstractTreeModel* model, YT_AbstractTreeNode* parent = nullptr);

    int index() const;
    YT_AbstractTreeNode* parent() const;

    YT_AbstractTreeNode* child(int index) const;
    void appendChild(std::unique_ptr<YT_AbstractTreeNode>&& child);
    void removeChild(int index);
    void clearChild();
    int childCount() const;

    QVariantList m_data;
    YT_AbstractTreeModel* const m_model;
    // QVariant& operator[](int index) {
    //     return m_data[index];
    // }
    // const QVariant& operator[](int index) const {
    //     return m_data[index];
    // }
private:
    YT_AbstractTreeNode* m_parent;
    std::vector<std::unique_ptr<YT_AbstractTreeNode>> m_children;
};