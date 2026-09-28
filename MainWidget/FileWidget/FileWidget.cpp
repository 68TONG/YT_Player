#include "FileWidget.h"

#include "DB_DatabaseInstance.h"

FileWidget::FileWidget(QQuickItem* parent)
    : QQuickItem{ parent }, pathModel(this, "FileWidget")
{
    this->setFlag(QQuickItem::ItemHasContents, true);
    connect(&pathModel, &YT_PathWidgetModel::srcDataChanged, this, &FileWidget::pathModelFunc);

    pathModel.getSubPathFunc = [this](int index)->QList<QString> {
        const auto path = pathModel.srcData.mid(0, index + 1).join("/") + '/';
        return QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        };;
}

Q_INVOKABLE QString FileWidget::getFileInfo_CoverPath(QString info_path)
{
    const auto data = DB_DatabaseInstance::obj().getFileInfo_CoverHashValue(info_path);
    return "image://FileImage/" + (data.isEmpty() ? info_path : data);
}

void FileWidget::create(const QString& path)
{
    const QString name = "新建文件"; // 基础文件名
    QString target_path = path + "/" + name;

    for (int i = 1; QFile::exists(target_path); i++) {
        target_path = path + "/" + name + "_" + QString::number(i);
    }

    QFile file(target_path);
    if (file.open(QIODevice::WriteOnly)) {
        file.close();
    }
}

void FileWidget::createFolder(const QString& path)
{
    const QString name = "新建文件夹"; // 基础文件名
    QString target_path = path + "/" + name;

    for (int i = 1; QFile::exists(target_path); i++) {
        target_path = path + "/" + name + "_" + QString::number(i);
    }

    QDir().mkpath(target_path);
}

void FileWidget::copy(const QString& path)
{
    for (auto&& it : operationPaths) {
        copyFolder(it, path, true, true);
    }
}

void FileWidget::move(const QString& path)
{
    for (auto&& it : operationPaths) {
        moveFolder(it, path, true, true);
    }
}

void FileWidget::rename(const QString& name)
{
    const auto source_name = operationPaths[operationPaths.count() - 1];
    QFile::rename(source_name, name);
}

void FileWidget::remove()
{
    for (auto&& it : operationPaths) {
        deleteFile(it);
    }
}

void FileWidget::pathModelFunc()
{
    const auto path_data = pathModel.getData();
    const auto path_count = pathModel.srcData.count();
    if (path_count == 0) return;

    QList<DB_FileInfo> info_list;
    if (QDir(path_data + '/').exists()) {
        return itemModel.setInfoIds(QDir(path_data + '/'));
    }

    if (pathModel.srcData[0] == "CreatorInfo") {
        if (path_count == 1) {
            itemModel.setInfoIds(DB_DatabaseInstance::obj().getCreatorInfo_FileIds());
        }
        else if (DB_DatabaseInstance::obj().getCreatorInfo_NameAll().contains(pathModel.srcData[1])) {
            itemModel.setInfoIds(DB_DatabaseInstance::obj().getCreatorInfo_FileIds(pathModel.srcData[1]));
            for (int i = 2; i < pathModel.srcData.count(); i++) {
                itemModel.searchInfoIds(pathModel.srcData[i]);
            }
        }
        else {
            itemModel.setInfoIds(DB_DatabaseInstance::obj().getCreatorInfo_FileIds());
            for (int i = 1; i < pathModel.srcData.count(); i++) {
                itemModel.searchInfoIds(pathModel.srcData[i]);
            }
        }
    }
}

void FileWidget::setCurListInfoPath(const QString& data)
{
    if (curListInfoPath == data) return;

    curListInfoPath = data;
    emit curListInfoPathChanged();

    pathModel.setSrcData(data);
}

// void FileWidget::setCurModel(FileItemModel*& data) {
//     if (curModel == data)
//         return;
//     if (curModel)
//         disconnect(curModel, &FileItemModel::currentDirChanged, this, &FileWidget::curModelPathChanged);
//     if (data)
//         connect(data, &FileItemModel::currentDirChanged, this, &FileWidget::curModelPathChanged);

//     curModel = data;
//     curModelPathChanged();
//     emit curModelChanged();
// }