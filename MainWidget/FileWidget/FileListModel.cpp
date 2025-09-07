#include "FileListModel.h"

FileListModel::FileListModel(QObject *parent)
    : QFileSystemModel(parent)
{
    this->setRootPath("C:/");
    this->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
}

int FileListModel::columnCount(const QModelIndex &parent) const
{
    return +true;
}
