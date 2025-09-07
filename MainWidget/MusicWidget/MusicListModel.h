#ifndef MUSICLISTMODEL_H
#define MUSICLISTMODEL_H

#include <QAbstractItemModel>
#include <YT_ConfigureInfo.h>
#include <Music_DataBase.h>

class MusicWidget;
class MusicListModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("MusicListModel cannot be created directly in QML")
    friend MusicWidget;
public:
    enum MusicInfoRole{
        InfoRole_InfoID,
        InfoRole_Name,
    };
    Q_ENUM(MusicInfoRole)
    explicit MusicListModel(MusicWidget &dataModel, QObject *parent = nullptr);
    virtual ~MusicListModel() override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE bool move(int index, int at_index, int count = 1);

    Q_INVOKABLE int index(const int infoID) const;
    Q_INVOKABLE int infoId(const int index) const;
    Q_INVOKABLE void addInfo(const QString &name);
    Q_INVOKABLE void eraseInfo(const int infoID);
    Q_INVOKABLE void setInfoName(const int infoID, const QString &name);
    Q_INVOKABLE void addItemInfo(const int infoID, const QList<QString> &list);
protected:
    QList<int> infoID_List;
private:
    MusicWidget * const dataModel;
};

#endif // MUSICLISTMODEL_H
