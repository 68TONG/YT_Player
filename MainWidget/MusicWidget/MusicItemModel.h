#ifndef MUSICITEMMODEL_H
#define MUSICITEMMODEL_H

#include <QAbstractItemModel>
#include <YT_ConfigureInfo.h>
#include <Music_DataBase.h>
#include <MediaFormat.h>

class MusicWidget;
class MusicItemModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("MusicItemModel cannot be created directly in QML")
    friend MusicWidget;
public:
    enum MusicInfoRole{
        InfoRole_InfoID,
        InfoRole_Title,
        InfoRole_MetaData_One,
        InfoRole_MetaData_Two,
        InfoRole_Duration,
        InfoRole_Path,
    };
    Q_ENUM(MusicInfoRole)
    explicit MusicItemModel(MusicWidget &dataModel, QObject *parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void setInfoID_List(const int &info_id);
    Q_INVOKABLE void setInfoID_List(const QList<int> &info_ids);
    Q_INVOKABLE void sortInfoID_List(const QString &sort_index);

    Q_INVOKABLE void setViewPosition(const QString &data);
protected:
    QList<int> infoID_List;
private:
    MusicWidget * const dataModel;
signals:
    void infoID_ListChanged();
    void viewPositionChanged(int index);
};

#endif // MUSICITEMMODEL_H
