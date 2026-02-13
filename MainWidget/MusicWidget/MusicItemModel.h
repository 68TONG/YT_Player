#ifndef MUSICITEMMODEL_H
#define MUSICITEMMODEL_H

#include <YT_Info.h>
#include <MediaFormat.h>
#include <MusicDatabase.h>
#include <QAbstractItemModel>

class MusicWidget;
class MusicItemModel : public QAbstractItemModel
{
    Q_OBJECT
        QML_ELEMENT
        QML_UNCREATABLE("MusicItemModel cannot be created directly in QML")
        friend MusicWidget;
public:
    enum MusicInfoRole
    {
        InfoID_Role,
        TitleTag_Role,
        Path_Role
    };
    Q_ENUM(MusicInfoRole)
public:
    explicit MusicItemModel(MusicWidget& dataModel, QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QList<QVariant> tagData(const int& index, const QString& tag_index) const;

    Q_INVOKABLE void setInfoIds(const QString& info_id);
    Q_INVOKABLE void setInfoIds(const QList<int>& info_ids);
    Q_INVOKABLE void sortInfoIds(const QString& sort_data);
    Q_INVOKABLE void searchInfoIds(const QString& search_data);

    // Q_INVOKABLE void setInfoID_List(const QString& info_id);
    // Q_INVOKABLE void setInfoID_List(const QList<int>& info_ids);
    // Q_INVOKABLE void sortInfoID_List(const QString& sort_data);

    Q_INVOKABLE void setViewPosition(const QString& data);
    Q_INVOKABLE void setFooterPosition(const QString& data);
protected:
    QList<int> infoIds;
    QHash<int, MusicFileInfo> infoList;

    Q_PROPERTY(QString titleTag MEMBER titleTag CONSTANT)
        const QString titleTag = Media::MetaData_Title;

    Q_PROPERTY(QString sortTag MEMBER sortTag NOTIFY sortTagChanged)
        QString sortTag = {};
    Q_SIGNAL void sortTagChanged();

    Q_PROPERTY(QList<QString> selectTag MEMBER selectTag NOTIFY selectTagChanged)
        QList<QString> selectTag = { };
    Q_SIGNAL void selectTagChanged();

    Q_PROPERTY(QList<QString> headerModel READ getHeaderModel NOTIFY headerModelChanged)
        QList<QString> headerModel = {};
    QList<QString> getHeaderModel() const { return headerModel; }
    Q_SIGNAL void headerModelChanged();
    void updateHeaderModel();

    Q_PROPERTY(QList<QString> footerModel READ getFooterModel NOTIFY footerModelChanged)
        QList<QString> footerModel = {};
    QList<QString> getFooterModel() const { return footerModel; }
    Q_SIGNAL void footerModelChanged();
    void updateFooterModel();
private:
    MusicWidget* const dataModel;
signals:
    void infoIdsChanged();
    void viewPositionChanged(int index);
    void footerPositionChanged(int index);
};

#endif // MUSICITEMMODEL_H
