#include "FileWidget.h"

FileWidget::FileWidget(QQuickItem* parent)
    : QQuickItem{ parent }, pathModel(this, "FileWidget")
{
    this->setFlag(QQuickItem::ItemHasContents, true);
    connect(&pathModel, &YT_PathWidgetModel::srcDataChanged, this, [this]() {
        if (pathModel.isSetDataing) return;
        if (curModel == nullptr) return;

        const auto data = pathModel.getData() + '/';
        pathModel.isSetDataing = true;
        curModel->setCurrentDir(data);
        pathModel.isSetDataing = false;
        });

    pathModel.getSubPathFunc = [this](int index)->QList<QString> {
        const auto path = pathModel.srcData.mid(0, index + 1).join("/") + '/';
        return QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        };;
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

void FileWidget::setCurModel(FileItemModel*& data) {
    if (curModel == data)
        return;
    if (curModel)
        disconnect(curModel, &FileItemModel::currentDirChanged, this, &FileWidget::curModelPathChanged);
    if (data)
        connect(data, &FileItemModel::currentDirChanged, this, &FileWidget::curModelPathChanged);

    curModel = data;
    curModelPathChanged();
    emit curModelChanged();
}

void FileWidget::curModelPathChanged()
{
    if (pathModel.isSetDataing) return;

    pathModel.isSetDataing = true;
    pathModel.setSrcData(curModel ? curModel->currentDir : "");
    pathModel.isSetDataing = false;
}