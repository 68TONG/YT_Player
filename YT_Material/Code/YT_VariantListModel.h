#pragma once

#include "YT_Global.h"
#include <QQuickItem>
#include <QAbstractItemModel>

class YT_EXPORT YT_VariantListModel : public QAbstractItemModel
{
    Q_OBJECT
        QML_NAMED_ELEMENT(VariantListModel)
public:
    explicit YT_VariantListModel(QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int count() const;
    Q_INVOKABLE QVariant get(int index) const;
    Q_INVOKABLE void set(int index, const QVariant& data);
    Q_INVOKABLE void reset(const QList<QVariant>& data);
    Q_INVOKABLE void append(const QVariant& data);
    Q_INVOKABLE void insert(int index, const QVariant& data);
    Q_INVOKABLE void move(int index, int at_index);
    Q_INVOKABLE void remove(int index, int count);
    Q_INVOKABLE void clear();
protected:
    QList<QVariant> dataList;
};
