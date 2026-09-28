#include "YT_AbstractTreeNode.h"

YT_AbstractTreeNode::YT_AbstractTreeNode(YT_AbstractTreeModel* model, YT_AbstractTreeNode* parent)
    : m_model(model), m_parent(parent)
{
}

int YT_AbstractTreeNode::index() const
{
    if (m_parent == nullptr)
        return -1;

    const auto it = std::find_if(m_parent->m_children.cbegin(), m_parent->m_children.cend(),
        [this](const std::unique_ptr<YT_AbstractTreeNode>& data) {
            return data.get() == this;
        });

    if (it != m_parent->m_children.cend())
        return std::distance(m_parent->m_children.cbegin(), it);
    Q_ASSERT(false); // should not happen
    return -2;
}

YT_AbstractTreeNode* YT_AbstractTreeNode::parent() const
{
    return m_parent;
}

YT_AbstractTreeNode* YT_AbstractTreeNode::child(int index) const
{
    return index >= 0 && index < childCount() ? m_children.at(index).get() : nullptr;
}

void YT_AbstractTreeNode::appendChild(std::unique_ptr<YT_AbstractTreeNode>&& child)
{
    m_children.push_back(std::move(child));
}

void YT_AbstractTreeNode::removeChild(int index)
{
    if (index < 0 || index >= m_children.size()) return;
    m_children.erase(m_children.begin() + index);
}

void YT_AbstractTreeNode::clearChild()
{
    m_children.clear();
}

int YT_AbstractTreeNode::childCount() const
{
    return int(m_children.size());
}