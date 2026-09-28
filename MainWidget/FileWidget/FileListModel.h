#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractItemModel>
#include <YT_AbstractTreeModel.h>

class FileWidget;
class FileListModel : public QAbstractItemModel
{
    Q_OBJECT
        QML_ELEMENT
        friend FileWidget;
public:
    enum ListInfoRole {
        ListName_Role,
        ListPath_Role,
        ListType_Role,
        ListInfoRole_End
    };
    Q_ENUM(ListInfoRole)
public:
    enum FileInfoType {
        NoneType,
        LocalFileType,
        CreatorInfoType,
        FileListInfoType,
        PathLinkInfoType
    };
    Q_ENUM(FileInfoType)
public:
    explicit FileListModel(QObject* parent = nullptr);

    QModelIndex index(const YT_AbstractTreeNode* node) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void fetchMore(const QModelIndex& parent) override;
    bool canFetchMore(const QModelIndex& parent) const override;
    bool hasChildren(const QModelIndex& parent) const override;

    Q_INVOKABLE void menuAdd(const QModelIndex& index, QString data);
    Q_INVOKABLE void menuRemove(const QModelIndex& index);
    // private slots:
    //     void onSubModelAddNode(YT_AbstractTreeNode* node, int index);
    //     void onSubModelRemoveNode(YT_AbstractTreeNode* node, int index);
public:
    QList<YT_AbstractTreeModel*> modelList;

    Q_PROPERTY(QString localFileType READ getLocalFileType CONSTANT)
        const QString localFileType = "LocalFile";
    QString getLocalFileType() { return localFileType; };
};

#endif // FILELISTMODEL_H
