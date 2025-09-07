#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <YT_GeneralH.h>
#include <QFileSystemModel>

class FileListModel : public QFileSystemModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit FileListModel(QObject *parent = nullptr);
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
private:
};

#endif // FILELISTMODEL_H
