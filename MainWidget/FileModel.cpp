#include "FileModel.h"

#include <QQmlEngine>
#include <QtConcurrent/QtConcurrentRun>

#include "WidgetModel.h"
#include "FileListModel.h"
#include "MusicDatabase.h"
#include "DB_DataBase.h"

FileModel::FileModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    // {
    //     tagModel.append(InfoTitle_Role);
    //     tagModel.append(InfoTag_Role);
    //     tagModel.append(FileSize_Role);
    //     tagModel.append(FileType_Role);
    //     tagModel.append(FileCreationDate_Role);
    //     tagModel.append(FileUpdatedDate_Role);
    // }

    // {
    //     selectTagModel.append(FileSize_Role);
    //     selectTagModel.append(FileType_Role);
    // }
    connect(&WidgetModel::obj(), &WidgetModel::pathModelDataChanged, this, &FileModel::setInfoIds);
}

FileModel::~FileModel()
{
    {}
}

FileModel& FileModel::obj()
{
    static FileModel object;
    return object;
}

FileModel* FileModel::create(QQmlEngine* qml_engine, QJSEngine*)
{
    QQmlEngine::setObjectOwnership(&obj(), QQmlEngine::CppOwnership);
    return &obj();
}

QModelIndex FileModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || row >= infoIds.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex FileModel::parent(const QModelIndex& index) const
{
    return {};
}

int FileModel::rowCount(const QModelIndex& parent) const
{
    return infoIds.count();
}

int FileModel::columnCount(const QModelIndex& parent) const
{
    return +true;
}

QHash<int, QByteArray> FileModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames = {
        { InfoId_Role, "infoId" },
        { InfoTitle_Role, "infoTitle" },
        { InfoTag_Role, "infoTag" },

        { FileName_Role, "fileName" },
        { FileSize_Role, "fileSize" },
        { FileType_Role, "fileType" },
        { FileCreationDate_Role, "fileCreationDate" },
        { FileUpdatedDate_Role, "fileUpdatedDate" },
        { FilePath_Role, "filePath" },

        { StarLevel_RoleEx, "starLevel" },
        { PlayCount_RoleEx, "playCount" },
        { AddPlayCount_RoleEx, "addPlayCount" },
    };
    return roleNames;
}

QVariant FileModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false) return {};

    auto info = getInfoList(infoIds[index.row()]);
    const auto& file_info = info->fileInfo;
    if (role == InfoId_Role) {
        return info->Id;
    }
    else if (role == InfoTitle_Role) {
        return file_info.fileName();
    }
    else if (role == InfoTag_Role) {
        if (info->fileTag.isEmpty()) return {};
        return DB::jsTagList(info->fileTag);
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
    else if (infoType == FileInfoType::Music) {
        auto info_ex = dynamic_cast<const MusicFileInfo*>(info);
        if (role == StarLevel_RoleEx) {
            return info_ex->starLevel;
        }
        else if (role == PlayCount_RoleEx) {
            return info_ex->playCount;
        }
        else if (role == AddPlayCount_RoleEx) {
            return info_ex->addPlayCount;
        }
    }

    return {};
}

Q_INVOKABLE QVariant FileModel::data_Index(const int& index, int role) const
{
    return this->data(this->index(index, 0), role);
}

Q_INVOKABLE QVariant FileModel::data_InfoId(const int& info_id, int role) const
{
    return {};
}

Q_INVOKABLE QVariant FileModel::data_Index(const int& index, QString data) const
{
    return this->data(this->index(index, 0), roleNames().key(data.toUtf8()));
}

Q_INVOKABLE QVariant FileModel::data_InfoId(const int& info_id, QString data) const
{
    return Q_INVOKABLE QVariant();
}

Q_INVOKABLE QVariant FileModel::dataEx_Index(const int& index, QString data) const
{
    return Q_INVOKABLE QVariant();
}

Q_INVOKABLE QVariant FileModel::dataEx_InfoId(const int& info_id, QString data) const
{
    return Q_INVOKABLE QList<QVariant>();
}

Q_INVOKABLE QString FileModel::dataRoleName(const int& role) const
{
    return roleNames().value(role);
}

Q_INVOKABLE void FileModel::setInfoIds()
{
    YT_TimeDebugger de_(QStringLiteral("FileModel All"));
    YT_TimeDebugger de(QStringLiteral("FileModel"));
    const auto path = WidgetModel::obj().getPath();
    const auto datas = WidgetModel::obj().getPathData();
    const auto type = WidgetModel::obj().getWidgetType();
    const auto count = datas.count();
    if (count <= 0 || QList<int>({
        WidgetModel::WidgetType_File,
        WidgetModel::WidgetType_Music,
        WidgetModel::WidgetType_Video,
        WidgetModel::WidgetType_Picture,
        WidgetModel::WidgetType_Creator
        }).contains(type) == false)
    {
        return;
    }
    de << "isEmpty & isType";

    this->beginResetModel();
    infoIds.clear();
    infoList.clear();
    infoType = FileInfoType::None;

    if (type == WidgetModel::WidgetType_File && QDir(path).exists()) {
        mSetInfoIds_FileDir(path);
    }
    else if (type == WidgetModel::WidgetType_File && datas[0] == "CreatorInfo") {
        const bool is = count > 1 && DB::object().getCreatorInfo_NameAll().contains(datas[1]);
        mSetInfoIds_CreatorId(is ? datas[1] : "");
        for (int i = is + 1; i < count; i++) {
            mSearchInfoIds(datas[i]);
        }
    }
    else if (type == WidgetModel::WidgetType_Creator) {
        const bool is = count > 0 && DB::object().getCreatorInfo_NameAll().contains(datas[0]);
        mSetInfoIds_CreatorId(is ? datas[0] : "");
        for (int i = is; i < count; i++) {
            mSearchInfoIds(datas[i]);
        }
    }
    else {
        mSetInfoIds_ListId(datas[0]);
        for (int i = 1; i < count; i++) {
            mSearchInfoIds(datas[i]);
        }
    }
    de << "setInfoIds";

    updateProperties();
    de << "updateProperties";

    this->endResetModel();
    de << "end Reset Model";
}

Q_INVOKABLE void FileModel::sortInfoIds(QString data)
{
    return Q_INVOKABLE void();
}

Q_INVOKABLE void FileModel::searchInfoIds(QString data)
{
    this->beginResetModel();
    mSearchInfoIds(data);
    this->endResetModel();
}

void FileModel::mSetInfoIds_ListId(QString info_id)
{
    static const auto _infoTypeHash = QHash<QString, FileInfoType>{
        { DB_ListInfo::I_Type_Music, FileInfoType::Music },
        { DB_ListInfo::I_Type_Video, FileInfoType::Video },
        { DB_ListInfo::I_Type_Picture, FileInfoType::Picture }
    };

    const auto info = DB::object().getListInfo(info_id);
    if (info.name.isEmpty()) return;

    infoIds = info.fileInfoIds;
    infoType = _infoTypeHash.value(info.type, FileInfoType::None);
    if (infoType == FileInfoType::Music) {
        infoList = DB::object<MusicDatabase>().getFileInfoPtr(infoIds.mid(0, infoGetCount));
    }
    else if (infoType == FileInfoType::Video) {
        infoList = DB::object().getFileInfoPtr(infoIds.mid(0, infoGetCount));
    }
    else if (infoType == FileInfoType::Picture) {
        infoList = DB::object().getFileInfoPtr(infoIds.mid(0, infoGetCount));
    }
    else {
        infoList = DB::object().getFileInfoPtr(infoIds.mid(0, infoGetCount));
    }
    for (auto& it : infoList) {
        it.get()->fileInfo = QFileInfo(it.get()->filePath);
    }
    mSetInfoIds_DatabaseAsync();
}

void FileModel::mSetInfoIds_CreatorId(QString info_id)
{
    if (info_id.isEmpty()) {
        infoIds = DB::object().getCreatorInfo_FileIds();
    }
    else {
        infoIds = DB::object().getCreatorInfo_FileIds(info_id);
    }
    infoList = DB::object().getFileInfoPtr(infoIds.mid(0, infoGetCount));
    for (auto& it : infoList) {
        it.get()->fileInfo = QFileInfo(it.get()->filePath);
    }
    mSetInfoIds_DatabaseAsync();
}

void FileModel::mSetInfoIds_FileDir(QString dir_path)
{
    int id_cursor = 0;
    QDir::Filters filter = (QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    QDirIterator it(DB::canonicalPath(dir_path), filter);
    while (it.hasNext() && id_cursor < 12) {
        it.next();
        infoIds.append(id_cursor);
        infoList.insert(id_cursor, DB_FileInfo::Ptr::create(it.filePath()));
        id_cursor++;
    }
}

void FileModel::mSetInfoIds_DatabaseAsync()
{
    return;
    // if (infoIds.size() <= infoGetCount)
    //     return;
    // const auto generation = ++infoListGeneration;
    // auto watcher = new QFutureWatcher<QHash<int, DB_FileInfo::Ptr>>(this);
    // connect(watcher, &QFutureWatcher<QHash<int, DB_FileInfo::Ptr>>::finished, this, [this, watcher, generation]() {
    //     const auto result = watcher->result();
    //     watcher->deleteLater();
    //     if (generation != infoListGeneration)
    //         return;

    //     infoList = result;
    //     });
    // if (infoType == FileInfoType::Music) {
    //     watcher->setFuture(QtConcurrent::run([ids = infoIds]() {
    //         return DB::objectForThread<MusicDatabase>().getFileInfoPtr(ids);
    //         }));
    // }
    // else if (infoType == FileInfoType::Video) {
    //     watcher->setFuture(QtConcurrent::run([ids = infoIds]() {
    //         return DB::objectForThread().getFileInfoPtr(ids);
    //         }));
    // }
    // else if (infoType == FileInfoType::Picture) {
    //     watcher->setFuture(QtConcurrent::run([ids = infoIds]() {
    //         return DB::objectForThread().getFileInfoPtr(ids);
    //         }));
    // }
    // else {
    //     watcher->setFuture(QtConcurrent::run([ids = infoIds]() {
    //         return DB::objectForThread().getFileInfoPtr(ids);
    //         }));
    // }
}

void FileModel::mSortInfoIds(QString data)
{
}

void FileModel::mSearchInfoIds(QString data)
{
    if (isDatabaseInfoList() == false) return;
    infoIds = DB::object().searchFileInfo(infoIds, data);
}

DB_FileInfo* FileModel::getInfoList(int info_id)
{
    auto it = infoList.find(info_id);
    if (it != infoList.end()) return it.value().get();

    if (infoType == FileInfoType::Music) {
        it = infoList.insert(info_id, DB::object<MusicDatabase>().getFileInfoPtr(info_id));
    }
    else if (infoType == FileInfoType::Video) {
        it = infoList.insert(info_id, DB::object().getFileInfoPtr(info_id));
    }
    else if (infoType == FileInfoType::Picture) {
        it = infoList.insert(info_id, DB::object().getFileInfoPtr(info_id));
    }
    else {
        it = infoList.insert(info_id, DB::object().getFileInfoPtr(info_id));
    }

    auto info = it.value().get();
    info->fileInfo = QFileInfo(info->filePath);
    return info;
}

const DB_FileInfo* FileModel::getInfoList(int info_id) const
{
    return const_cast<FileModel*>(this)->getInfoList(info_id);
}

bool FileModel::isDatabaseInfoList() const
{
    return infoList.isEmpty() == false && infoList.constBegin().value().get()->Id >= 0;
}

QStringList FileModel::getProperties() const
{
    return properties;
}

void FileModel::updateProperties()
{
    const auto& roleNames = this->roleNames();
    QStringList data_list = {
        roleNames[InfoTag_Role],
        roleNames[FileSize_Role],
        roleNames[FileType_Role],
        roleNames[FileCreationDate_Role],
        roleNames[FileUpdatedDate_Role]
    };

    if (infoType == FileInfoType::Music) {
        data_list.append(roleNames[StarLevel_RoleEx]);
        data_list.append(roleNames[PlayCount_RoleEx]);
        data_list.append(roleNames[AddPlayCount_RoleEx]);
    }
    else if (infoType == FileInfoType::Video) {
    }
    else if (infoType == FileInfoType::Picture) {
    }

    if (data_list == properties) return;
    properties = data_list;
    emit propertiesChanged();
}

QStringList FileModel::getSelectProperty() const
{
    return selectProperty;
}

void FileModel::setSelectProperty(const QStringList& data)
{
    if (selectProperty == data) return;
    selectProperty = data;
    emit selectPropertyChanged();
}

QString FileModel::getSortProperty() const
{
    return sortProperty;
}

void FileModel::setSortProperty(const QString& data)
{
    if (sortProperty == data) return;
    sortProperty = data;
    emit sortPropertyChanged();
}
