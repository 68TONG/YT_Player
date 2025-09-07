#include "MusicListModel.h"

#include "MusicWidget.h"

MusicListModel::MusicListModel(MusicWidget &dataModel, QObject *parent)
    : QAbstractItemModel(parent)
    , dataModel(&dataModel)
{
    auto infoID_List = YT_ConfigureInfo::getObject().getData(YT_ConfigureInfo::MusicListInfoID_List).toList();
    if (infoID_List.isEmpty()) {
        this->infoID_List = Music_DataBase::getObject().getListInfo_ID_ALL();
    } else {
        for (auto &&it : infoID_List)
            this->infoID_List.append(it.toInt());
    }
}

MusicListModel::~MusicListModel()
{
    QVariantList infoID_List;
    for (auto &&it : this->infoID_List)
        infoID_List.append(it);
    YT_ConfigureInfo::getObject().setData(YT_ConfigureInfo::MusicListInfoID_List, infoID_List);
}

QModelIndex MusicListModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0 || row >= infoID_List.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex MusicListModel::parent(const QModelIndex &index) const
{
    return {};
}

int MusicListModel::rowCount(const QModelIndex &parent) const
{
    return infoID_List.count();
}

int MusicListModel::columnCount(const QModelIndex &parent) const
{
    return +true;
}

QHash<int, QByteArray> MusicListModel::roleNames() const
{
    return {
        {InfoRole_InfoID, "infoID"},
        {InfoRole_Name, "name"}
    };
}

QVariant MusicListModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() == false) return {};
    const int info_id = infoID_List[index.row()];
    auto &&info = Music_DataBase::getObject().getListInfo(info_id);

    if (role == InfoRole_InfoID) {
        return info.ID;
    } else if (role == InfoRole_Name) {
        return info.Name;
    }
    return {};
}

bool MusicListModel::move(int index, int at_index, int count)
{
    this->beginMoveRows(QModelIndex(), index, index + count - 1, QModelIndex(), at_index + (at_index > index));

    for (int i = 0; i < count; ++i) {
        infoID_List.move(index + i, at_index);
    }

    this->endMoveRows();
    return true;
}

int MusicListModel::index(const int infoID) const
{
    for (int i = 0; i < infoID_List.count(); i++) {
        if (infoID_List[i] == infoID) return i;
    }
    return -1;
}

int MusicListModel::infoId(const int index) const
{
    if (index < 0 || index >= infoID_List.count()) return -1;
    return infoID_List[index];
}

void MusicListModel::addInfo(const QString &name)
{
    Music_ListInfo info(-true, name, {});
    int info_id = Music_DataBase::getObject().addListInfo(info);
    if (info_id < 0) {
        qDebug() << "MusicListModel addInfo info_id < 0";
        return ;
    }

    this->beginInsertRows({}, info_id, info_id);
    infoID_List.insert(info_id, info_id);
    this->endInsertRows();
}

void MusicListModel::eraseInfo(const int infoID)
{
    if (Music_DataBase::getObject().eraseListInfo(infoID) < 0) {
        qDebug() << "MusicListModel eraseInfo eraseListInfo < 0";
        return ;
    }

    this->beginResetModel();
    infoID_List = Music_DataBase::getObject().getListInfo_ID_ALL();
    if (dataModel->curListInfoID == infoID)
        dataModel->setCurListInfoID(infoID);
    this->endResetModel();
}

void MusicListModel::setInfoName(const int infoID, const QString &name)
{
    auto info = Music_DataBase::getObject().getListInfo(infoID);
    info.Name = name;
    if (Music_DataBase::getObject().setListInfo(info) < 0) {
        qDebug() << "MusicListModel setInfoName setListInfo < 0";
        return ;
    }

    const int index = infoID_List.indexOf(infoID);
    if (index < 0) return ;

    const auto model_index = this->index(index, 0, {});
    emit this->dataChanged(model_index, model_index);
}

void MusicListModel::addItemInfo(const int infoID, const QList<QString> &list)
{
    QList<QString> failed_target_list;
    for(auto &&it : list){
        QFileInfo file_info(it);
        if (file_info.isFile()) {
            int sic = Music_DataBase::getObject().addListInfo_ItemPath(infoID, file_info.filePath());
            if (sic < 0) {
                failed_target_list.append(file_info.filePath());
            }
            continue;
        }
        if(file_info.isDir() == false) continue;

        QDirIterator file_iterator(file_info.filePath(), QDir::Files, QDirIterator::Subdirectories);
        while (file_iterator.hasNext()) {
            file_iterator.next();
            int sic = Music_DataBase::getObject().addListInfo_ItemPath(infoID, file_iterator.filePath());
            if (sic < 0) {
                failed_target_list.append(file_iterator.filePath());
            }
        }
    }
    if (dataModel->curListInfoID == infoID)
        dataModel->setCurListInfoID(infoID);
}
