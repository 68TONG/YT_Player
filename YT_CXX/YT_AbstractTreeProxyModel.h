#pragma once

#include <QQuickItem>
#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QPersistentModelIndex>

class YT_AbstractTreeProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
        QML_ELEMENT
public:
    enum ModelDataRole {
        ItemDepth_Role,
        ItemExpanded_Role,
        ItemHasChildren_Role,
        ModelDataRole_End
    };
    Q_ENUM(ModelDataRole)
public:
    explicit YT_AbstractTreeProxyModel(QObject* parent = nullptr);

    void setSourceModel(QAbstractItemModel* sourceModel) override;

    Q_PROPERTY(QAbstractItemModel* m_sourceModel READ m_sourceModel WRITE setM_sourceModel NOTIFY m_sourceModelChanged)
        QAbstractItemModel* m_sourceModel() const;
    void setM_sourceModel(QAbstractItemModel* model);
    Q_SIGNAL void m_sourceModelChanged();

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void expand(int index);
    Q_INVOKABLE void collapse(int index);
    Q_INVOKABLE bool isExpanded(int index) const;
    Q_INVOKABLE void toggleExpanded(int index);
    Q_INVOKABLE int depth(int index) const;
protected:
    QList<QModelIndex> getExpandIndexList(const QModelIndex& sourceIndex) const;
    bool isIndexParent(const QModelIndex& index, const QModelIndex& parent) const;
    QModelIndex normalizeIndex(const QModelIndex& index) const;

    QList<QPersistentModelIndex> indexList;
    QSet<QPersistentModelIndex> indexExpandHash;
protected:
    void slotRowsInserted(const QModelIndex& parent, int start, int end);
    void slotRowsRemoved(const QModelIndex& parent, int start, int end);
};
