#ifndef FILEITEMMODEL_H
#define FILEITEMMODEL_H

#include <DB_Info.h>
#include <YT_ListModel.h>
#include <QFileSystemWatcher>
#include <QAbstractItemModel>

class FileWidget;
class FileItemModel : public QAbstractItemModel
{
    Q_OBJECT
        QML_ELEMENT
        friend FileWidget;
public:
    enum FileInfoRole {
        InfoTitle_Role,
        InfoTag_Role,

        FileName_Role,
        FileSize_Role,
        FileType_Role,
        FileCreationDate_Role,
        FileUpdatedDate_Role,
        FilePath_Role
    };
    Q_ENUM(FileInfoRole)
public:
    explicit FileItemModel(QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE QByteArray roleNames(int role) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant dataIndex(const int& index, int role = Qt::DisplayRole) const;
    // Q_INVOKABLE QList<QVariant> dataTag(const int& index, const QString& data) const;
    // Q_INVOKABLE QVariant dataTag(const int& index, const QString& data, int ret_index) const;
    // Q_INVOKABLE QList<QVariant> dataTag_InfoId(const int& info_id, const QString& data) const;
    // Q_INVOKABLE QVariant dataTag_InfoId(const int& info_id, const QString& data, int ret_index) const;

    Q_INVOKABLE void setInfoIds(const QDir& dir);
    Q_INVOKABLE void setInfoIds(const QList<int>& info_ids);

    // Q_INVOKABLE void sortInfoIds(const QString& sort_data);
    Q_INVOKABLE void searchInfoIds(QString search_tag);
private:
    QList<int> infoIds;
    QHash<int, DB_FileInfo> infoList;
    QFileSystemWatcher modelWatcher;
protected:
    Q_PROPERTY(YT_ListModel* tagModel READ getTagModel CONSTANT)
        YT_ListModel tagModel;
    YT_ListModel* getTagModel() { return &tagModel; };

    Q_PROPERTY(YT_ListModel* selectTagModel READ getSelectTagModel CONSTANT)
        YT_ListModel selectTagModel;
    YT_ListModel* getSelectTagModel() { return &selectTagModel; };
};

#endif // FILEITEMMODEL_H
