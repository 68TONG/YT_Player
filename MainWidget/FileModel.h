#pragma once

#include "DB_Info.h"
#include "YT_VariantListModel.h"

#include <QQuickItem>
#include <QAbstractItemModel>

class FileModel : public QAbstractItemModel
{
    Q_OBJECT
        QML_ELEMENT
        QML_SINGLETON
public:
    enum class FileInfoType {
        None,
        Music,
        Video,
        Picture
    };
    enum FileInfoRole {
        InfoId_Role,
        InfoTitle_Role,
        InfoTag_Role,

        FileName_Role,
        FileSize_Role,
        FileType_Role,
        FileCreationDate_Role,
        FileUpdatedDate_Role,
        FilePath_Role,

        StarLevel_RoleEx,
        PlayCount_RoleEx,
        AddPlayCount_RoleEx
    };
    Q_ENUM(FileInfoRole)
private:
    explicit FileModel(QObject* parent = nullptr);
    ~FileModel();
public:
    static FileModel& obj();
    static FileModel* create(QQmlEngine* qml_engine, QJSEngine*);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant data_Index(const int& index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant data_InfoId(const int& info_id, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant data_Index(const int& index, QString data) const;
    Q_INVOKABLE QVariant data_InfoId(const int& info_id, QString data) const;
    Q_INVOKABLE QVariant dataEx_Index(const int& index, QString data) const;
    Q_INVOKABLE QVariant dataEx_InfoId(const int& info_id, QString data) const;
    Q_INVOKABLE QString dataRoleName(const int& role) const;

    Q_INVOKABLE void setInfoIds();
    Q_INVOKABLE void sortInfoIds(QString data);
    Q_INVOKABLE void searchInfoIds(QString data);
private:
    QList<int> infoIds;
    QHash<int, DB_FileInfo::Ptr> infoList;
    FileInfoType infoType = FileInfoType::None;
    const int infoGetCount = 20;

    void mSetInfoIds_ListId(QString info_id);
    void mSetInfoIds_CreatorId(QString info_id);
    void mSetInfoIds_FileDir(QString dir_path);
    void mSetInfoIds_DatabaseAsync();
    void mSortInfoIds(QString data);
    void mSearchInfoIds(QString data);

    DB_FileInfo* getInfoList(int info_id);
    const DB_FileInfo* getInfoList(int info_id) const;
    bool isDatabaseInfoList() const;
protected:
    Q_PROPERTY(QStringList properties READ getProperties NOTIFY propertiesChanged)
        QStringList properties;
    QStringList getProperties() const;
    Q_SIGNAL void propertiesChanged();
    void updateProperties();

    Q_PROPERTY(QStringList selectProperty READ getSelectProperty WRITE setSelectProperty NOTIFY selectPropertyChanged)
        QStringList selectProperty;
    QStringList getSelectProperty() const;
    void setSelectProperty(const QStringList& data);
    Q_SIGNAL void selectPropertyChanged();

    Q_PROPERTY(QString sortProperty READ getSortProperty WRITE setSortProperty NOTIFY sortPropertyChanged)
        QString sortProperty = {};
    QString getSortProperty() const;
    void setSortProperty(const QString& data);
    Q_SIGNAL void sortPropertyChanged();
};