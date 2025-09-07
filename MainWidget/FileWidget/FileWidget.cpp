#include "FileWidget.h"

FileWidget::FileWidget(QQuickItem *parent)
    : QQuickItem{parent}
{
    this->setFlag(QQuickItem::ItemHasContents, true);
}

void FileWidget::create(const QString &path)
{
    const QString name = "新建文件"; // 基础文件名
    QString target_path = path + "/" + name;

    for(int i = 1; QFile::exists(target_path); i++) {
        target_path = path + "/" + name + "_" + QString::number(i);
    }

    QFile file(target_path);
    if (file.open(QIODevice::WriteOnly)) {
        file.close();
    }
}

void FileWidget::createFolder(const QString &path)
{
    const QString name = "新建文件夹"; // 基础文件名
    QString target_path = path + "/" + name;

    for(int i = 1; QFile::exists(target_path); i++) {
        target_path = path + "/" + name + "_" + QString::number(i);
    }

    QDir().mkpath(target_path);
}

void FileWidget::copy(const QString &path)
{
    for (auto &&it : operationPaths) {
        copyFolder(it, path, true, true);
    }
}

void FileWidget::move(const QString &path)
{
    for (auto &&it : operationPaths) {
        moveFolder(it, path, true, true);
    }
}

void FileWidget::rename(const QString &name)
{
    const auto source_name = operationPaths[operationPaths.count() - 1];
    QFile::rename(source_name, name);
}

void FileWidget::remove()
{
    for (auto &&it : operationPaths) {
        deleteFile(it);
    }
}

void FileWidget::setCurPath(const QString &data)
{
    if (curModel)
        curModel->setCurrentDir(data);
}

void FileWidget::setCurPath(const QList<QString> &data)
{
    QString path = "";
    for (auto &&it : data)
        path += it + "/";
    if (curModel)
        curModel->setCurrentDir(path);
}

QList<QString> FileWidget::getSubPath(const QList<QString> &data) const
{
    QString path = "";
    for (auto &&it : data)
        path += it + "/";
    return QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}
