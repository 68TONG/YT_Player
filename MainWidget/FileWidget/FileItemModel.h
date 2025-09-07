#ifndef FILEITEMMODEL_H
#define FILEITEMMODEL_H

#include <YT_GeneralH.h>
#include <QAbstractItemModel>
#include <QFileSystemWatcher>

class FileWidget;
class FileItemModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool is_Hidden MEMBER is_Hidden)
    Q_PROPERTY(QString currentDir READ getCurrentDir WRITE setCurrentDir NOTIFY currentDirChanged)
    Q_PROPERTY(QList<int> headerIndex_List READ getHeaderIndex_List NOTIFY headerIndex_ListChanged)
    friend FileWidget;
public:
    enum FileInfoRole{
        FileInfoRole_Name,
        FileInfoRole_Size,
        FileInfoRole_Type,
        FileInfoRole_DateTime,

        FileInfoRole_Path,
        FileInfoRole_ChangeSignal,
    };
    Q_ENUM(FileInfoRole)
    explicit FileItemModel(QObject *parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant data(const int &index, const int &role) const;
private:
    QList<QFileInfo> fileInfo_List;
    QFileSystemWatcher modelWatcher;
protected:
    bool is_Hidden = true;

    QString currentDir;
    QString getCurrentDir() const { return currentDir; }
    void setCurrentDir(const QString &path);

    QList<int> headerIndex_List;
    QList<int> getHeaderIndex_List() const { return headerIndex_List; }
    Q_INVOKABLE QByteArray getHeaderName(const int index) const;
signals:
    void currentDirChanged();
    void headerIndex_ListChanged();
};

#endif // FILEITEMMODEL_H
