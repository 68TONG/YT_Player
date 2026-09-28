#include "FileItemModel.h"

#include "FileListModel.h"
#include "DB_DatabaseInstance.h"

FileItemModel::FileItemModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    {
        tagModel.append(InfoTitle_Role);
        tagModel.append(InfoTag_Role);
        tagModel.append(FileSize_Role);
        tagModel.append(FileType_Role);
        tagModel.append(FileCreationDate_Role);
        tagModel.append(FileUpdatedDate_Role);
    }

    {
        selectTagModel.append(InfoTitle_Role);
        selectTagModel.append(InfoTag_Role);
        selectTagModel.append(FileSize_Role);
    }

    // connect(this, &FileItemModel::currentDirChanged, this, [this]() {
    //     auto removePaths = modelWatcher.files() + modelWatcher.directories();
    //     if (removePaths.count()) modelWatcher.removePaths(removePaths);
    //     modelWatcher.addPath(currentDir);
    //     });
    // connect(&modelWatcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString& path) {
    //     setCurrentDir(currentDir);
    //     });
}

QModelIndex FileItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || row >= infoIds.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex FileItemModel::parent(const QModelIndex& index) const
{
    return {};
}

int FileItemModel::rowCount(const QModelIndex& parent) const
{
    return infoIds.count();
}

int FileItemModel::columnCount(const QModelIndex& parent) const
{
    return +true;
}

QVariant FileItemModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false) return {};
    const auto& info = infoList[infoIds[index.row()]];
    const auto& file_info = info.fileInfo;
    if (role == InfoTitle_Role) {
        return file_info.fileName();
    }
    else if (role == InfoTag_Role) {
        if (info.fileTag.isEmpty()) return {};
        return DB::jsTagList(info.fileTag);
    }
    else if (role == FileName_Role) {
        return file_info.fileName();
    }
    else if (role == FileSize_Role) {
        static const auto getFileSize = [](qint64 size)->QString {
            QLocale locale; // 创建一个 QLocale 实例
            if (size < 1024) {
                return locale.toString(size) + " B";
            }
            else if (size < 1024 * 1024) {
                return locale.toString(static_cast<qint64>(size / 1024.0)) + " KB";
            }
            else if (size < 1024 * 1024 * 1024) {
                return locale.toString(static_cast<qint64>(size / (1024.0 * 1024))) + " MB";
            }
            else {
                return locale.toString(static_cast<qint64>(size / (1024.0 * 1024 * 1024))) + " GB";
            }
            };
        return getFileSize(file_info.size());
    }
    else if (role == FileType_Role) {
        if (file_info.isDir()) return "Folder";
        return file_info.suffix();
    }
    else if (role == FileCreationDate_Role) {
        return file_info.birthTime().toString("yyyy/MM/dd hh:mm");
    }
    else if (role == FileUpdatedDate_Role) {
        return file_info.lastModified().toString("yyyy/MM/dd hh:mm");
    }
    else if (role == FilePath_Role) {
        return file_info.filePath();
    }
    return {};
}

Q_INVOKABLE QVariant FileItemModel::dataIndex(const int& index, int role) const
{
    return this->data(this->index(index, 0), role);
}

Q_INVOKABLE void FileItemModel::setInfoIds(const QDir& dir)
{
    this->beginResetModel();
    infoIds.clear();
    infoList.clear();

    int id_cursor = 0;
    QDir::Filters dir_filter = (QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    const auto list = dir.entryList(dir_filter, QDir::Name);
    for (auto&& it : list) {
        infoIds.append(id_cursor);
        infoList.insert(id_cursor, DB_FileInfo(dir.filePath(it)));
        id_cursor++;
    }
    this->endResetModel();
}

Q_INVOKABLE void FileItemModel::setInfoIds(const QList<int>& info_ids)
{
    this->beginResetModel();
    infoIds = info_ids;
    infoList = DB_DatabaseInstance::obj().getFileInfo(infoIds);
    for (auto& it : infoList) {
        it.fileInfo = QFileInfo(it.filePath);
    }
    this->endResetModel();
    // emit infoIdsChanged();
}

Q_INVOKABLE void FileItemModel::searchInfoIds(QString search_tag)
{
    this->beginResetModel();
    infoIds = DB_DatabaseInstance::obj().searchFileInfo(infoIds, search_tag);
    this->endResetModel();
}

QHash<int, QByteArray> FileItemModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames = {
        { InfoTitle_Role, "infoTitle" },
        { InfoTag_Role, "infoTag" },

        { FileName_Role, "fileName" },
        { FileSize_Role, "fileSize" },
        { FileType_Role, "fileType" },
        { FileCreationDate_Role, "fileCreationDate" },
        { FileUpdatedDate_Role, "fileUpdatedDate" },
        { FilePath_Role, "filePath" }
    };
    return roleNames;
}

Q_INVOKABLE QByteArray FileItemModel::roleNames(int role) const
{
    return this->roleNames().value(role);
}