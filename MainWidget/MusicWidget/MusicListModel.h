#ifndef MUSICLISTMODEL_H
#define MUSICLISTMODEL_H

#include <YT_Info.h>
#include <MusicDatabase.h>
#include <QAbstractItemModel>

class MusicWidget;
class MusicListModel : public QAbstractItemModel
{
    Q_OBJECT
        QML_ELEMENT
        QML_UNCREATABLE("MusicListModel cannot be created directly in QML")
        friend MusicWidget;
public:
    explicit MusicListModel(MusicWidget& dataModel, QObject* parent = nullptr);
    virtual ~MusicListModel() override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int index(const QString& infoID) const;
    Q_INVOKABLE QString infoID(const int& index) const;
    Q_INVOKABLE bool move(int index, int at_index);

    Q_INVOKABLE void addInfo(const QString& info_id);
    Q_INVOKABLE void eraseInfo(const QString& info_id);
    Q_INVOKABLE void setInfoName(const QString& info_id, const QString& name);
    Q_INVOKABLE void addFileInfo(const QString& info_id, const QList<QString>& path_list);
protected:
    QList<QString> infoIds;
private:
    MusicWidget* const dataModel;
};

#endif // MUSICLISTMODEL_H
