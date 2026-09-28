#pragma once

#include <YT_Info.h>
#include <MediaFormat.h>
#include <MusicDatabase.h>
#include <QAbstractItemModel>

class MusicWidget;
class MusicFileModel : public QAbstractItemModel
{
    Q_OBJECT
        QML_ELEMENT
        QML_UNCREATABLE("MusicFileModel cannot be created directly in QML")
        friend MusicWidget;
public:
    enum MusicInfoRole
    {
        InfoId_Role,
        TitleTag_Role,
        ArtistTag_Role,
        AlbumTag_Role,
        StarLevel_Role,
        PlayCount_Role,
        AddPlayCount_Role,
        MusicDuration_Role,
        MusicPath_Role
    };
    Q_ENUM(MusicInfoRole)
public:
    explicit MusicFileModel(MusicWidget& dataModel, QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant dataIndex(const int& index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QList<QVariant> dataTag(const int& index, const QString& data) const;
    Q_INVOKABLE QVariant dataTag(const int& index, const QString& data, int ret_index) const;
    Q_INVOKABLE QList<QVariant> dataTag_InfoId(const int& info_id, const QString& data) const;
    Q_INVOKABLE QVariant dataTag_InfoId(const int& info_id, const QString& data, int ret_index) const;

    Q_INVOKABLE void updateInfo(const int& info_id);
    Q_INVOKABLE void updateInfo(const QList<int>& info_ids);

    Q_INVOKABLE void setInfoIds(const QString& info_id);
    Q_INVOKABLE void setInfoIds(const QList<int>& info_ids);

    Q_INVOKABLE void sortInfoIds(QString sort_data);
    Q_INVOKABLE void searchInfoIds(QString search_tag, QString header_tag);

    Q_INVOKABLE void setViewPosition(QString data);
    Q_INVOKABLE void setViewPositionEx(QString index, QString data);
    Q_INVOKABLE void setFooterPosition(int index);
protected:
    QList<int> infoIds;
    QHash<int, MusicFileInfo> infoList;

    Q_PROPERTY(QString titleTag MEMBER titleTag CONSTANT)
        const QString titleTag = Media::MetaData_Title;

    Q_PROPERTY(QString sortTag MEMBER sortTag NOTIFY sortTagChanged)
        QString sortTag = {};
    Q_SIGNAL void sortTagChanged();

    Q_PROPERTY(QList<QString> selectTag MEMBER selectTag NOTIFY selectTagChanged)
        QList<QString> selectTag = {};
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

    Q_PROPERTY(QList<QString> footerModelEx READ getFooterModelEx NOTIFY footerModelExChanged)
        QList<QString> footerModelEx = {};
    QList<QString> getFooterModelEx() const { return footerModelEx; }
    Q_SIGNAL void footerModelExChanged();
    void updateFooterModelEx(int index, QString data);

    Q_PROPERTY(QList<double> itemStretchFactor READ getItemStretchFactor NOTIFY itemStretchFactorChanged)
        QList<double> itemStretchFactor = {};
    QList<double> getItemStretchFactor() const { return itemStretchFactor; }
    Q_SIGNAL void itemStretchFactorChanged();
    void updateItemStretchFactor();
private:
    MusicWidget* const dataModel;
signals:
    void infoIdsChanged();
    void viewPositionChanged(int index);
    void footerPositionChanged(int index);
};
