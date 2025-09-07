#include "FileItemModel.h"

FileItemModel::FileItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    headerIndex_List = {
        FileInfoRole_Name,
        FileInfoRole_Size,
        FileInfoRole_Type,
        FileInfoRole_DateTime
    };
    connect(this, &FileItemModel::currentDirChanged, this, [this](){
        auto removePaths = modelWatcher.files() + modelWatcher.directories();
        if(removePaths.count()) modelWatcher.removePaths(removePaths);
        modelWatcher.addPath(currentDir);
    });
    connect(&modelWatcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &path){
        setCurrentDir(currentDir);
    });
}

QModelIndex FileItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0 || row >= fileInfo_List.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex FileItemModel::parent(const QModelIndex &index) const
{
    return {};
}

int FileItemModel::rowCount(const QModelIndex &parent) const
{
    return fileInfo_List.count();
}

int FileItemModel::columnCount(const QModelIndex &parent) const
{
    return +true;
}

QVariant FileItemModel::data(const QModelIndex &index, int role) const
{
    return data(index.row(), role);
}

QVariant FileItemModel::data(const int &index, const int &role) const
{
    if(index < 0) return {};
    if(role == FileInfoRole_Name) {
        return fileInfo_List[index].fileName();
    } else if(role == FileInfoRole_Size) {
        static const auto getFileSize = [](qint64 size)->QString{
            QLocale locale; // 创建一个 QLocale 实例
            if (size < 1024) {
                return locale.toString(size) + " B";
            } else if (size < 1024 * 1024) {
                return locale.toString(static_cast<qint64>(size / 1024.0)) + " KB";
            } else if (size < 1024 * 1024 * 1024) {
                return locale.toString(static_cast<qint64>(size / (1024.0 * 1024))) + " MB";
            } else {
                return locale.toString(static_cast<qint64>(size / (1024.0 * 1024 * 1024))) + " GB";
            }
        };
        return getFileSize(fileInfo_List[index].size());
    } else if(role == FileInfoRole_Type) {
        if(fileInfo_List[index].isDir()) return "Folder";
        return fileInfo_List[index].suffix();
    } else if(role == FileInfoRole_DateTime) {
        return fileInfo_List[index].metadataChangeTime().toString("yyyy/MM/dd hh:mm");
    } else if(role == FileInfoRole_Path) {
        return fileInfo_List[index].filePath();
    }
    return {};
}

QHash<int, QByteArray> FileItemModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames = {
        {FileInfoRole_Name, "name"},
        {FileInfoRole_Size, "size"},
        {FileInfoRole_Type, "type"},
        {FileInfoRole_DateTime, "dateTime"},
        {FileInfoRole_ChangeSignal, "changeSignal"}
    };
    return roleNames;
}

void FileItemModel::setCurrentDir(const QString &path)
{
    this->beginResetModel();

    QDir dir(path);
    QDir::Filters list_filter = (QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    if(is_Hidden) list_filter = QDir::Hidden | list_filter;
    fileInfo_List = dir.entryInfoList(list_filter, QDir::Name);

    if(path != currentDir) {
        currentDir = path;
        emit currentDirChanged();
    }

    this->endResetModel();
}

QByteArray FileItemModel::getHeaderName(const int index) const
{
    return this->roleNames().value(index);
}

