#include "MusicListModel.h"

#include "MusicWidget.h"

MusicListModel::MusicListModel(MusicWidget& dataModel, QObject* parent)
    : QAbstractItemModel(parent)
    , dataModel(&dataModel)
{
    infoIds = MusicDatabase::obj().getListInfo_NameAll();
    // auto infoID_List = YT_Info::getObject().MusicListInfo_ID_List.toList();
    // if (true) {
    //     this->infoID_List = MusicDatabase::obj().getListInfo_NameAll();
    // }
    // else if (infoID_List.isEmpty()) {
    //     this->infoID_List = Music_DataBase::getObject().getListInfo_NameALL();
    // }
    // else {
    //     for (auto&& it : infoID_List)
    //         this->infoID_List.append(it.toString());
    // }
}

MusicListModel::~MusicListModel()
{
    // QVariantList infoID_List;
    // for (auto&& it : this->infoID_List)
    //     infoID_List.append(it);
    // YT_Info::getObject().setProperty(YT_Info::MusicListInfo_ID_ListName(), infoID_List);
}

QModelIndex MusicListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || row >= infoIds.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex MusicListModel::parent(const QModelIndex& index) const
{
    return {};
}

int MusicListModel::rowCount(const QModelIndex& parent) const
{
    return infoIds.count();
}

int MusicListModel::columnCount(const QModelIndex& parent) const
{
    return +true;
}

QHash<int, QByteArray> MusicListModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "infoId"}
    };
}

QVariant MusicListModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false) return {};
    const auto info_id = infoIds[index.row()];
    return info_id;
}

int MusicListModel::index(const QString& info_id) const
{
    return infoIds.indexOf(info_id);
}

QString MusicListModel::infoId(const int& index) const
{
    if (index < 0 || index >= infoIds.count()) return "";
    return infoIds[index];
}

bool MusicListModel::move(int index, int at_index)
{
    this->beginMoveRows(QModelIndex(), index, index, QModelIndex(), at_index + (at_index > index));
    MusicDatabase::obj().setListInfo_SortIndex(infoIds[index], infoIds[at_index]);
    infoIds.move(index, at_index);
    this->endMoveRows();
    emit infoIdsChanged();
    return true;
}

void MusicListModel::addInfo(const QString& info_id)
{
    // Music_ListInfo info(info_id, {});
    // int sic = Music_DataBase::getObject().addListInfo(info);

    MusicListInfo info(info_id);
    int sic = MusicDatabase::obj().addListInfo(info);
    if (sic < 0) {
        qDebug() << "MusicListModel addInfo addListInfo return < 0";
        return;
    }

    int index = infoIds.count();
    this->beginInsertRows({}, index, index);
    infoIds.insert(index, info_id);
    this->endInsertRows();
    emit infoIdsChanged();
}

void MusicListModel::eraseInfo(const QString& info_id)
{
    if (info_id == MusicDatabase::obj().playListInfo_Name) {
        qDebug() << "MusicListModel setInfoName playListInfo_Name is not erase";
        return;
    }
    
    int sic = MusicDatabase::obj().deleteListInfo(info_id);
    if (sic < 0) {
        qDebug() << "MusicListModel eraseInfo deleteListInfo return < 0";
        return;
    }

    int index = infoIds.indexOf(info_id);
    this->beginRemoveRows({}, index, index);
    infoIds.remove(index);
    if (dataModel->curListInfoID == info_id)
        dataModel->setCurListInfoID("");
    this->endRemoveRows();
    emit infoIdsChanged();
}

void MusicListModel::setInfoName(const QString& info_id, const QString& name)
{
    if (info_id == MusicDatabase::obj().playListInfo_Name) {
        qDebug() << "MusicListModel setInfoName playListInfo_Name is not set";
        return;
    }

    int sic = MusicDatabase::obj().setListInfo(info_id, DB_ListInfo::I_Name, name);
    if (sic < 0) {
        qDebug() << "MusicListModel setInfoName setListInfo error";
        return;
    }

    const int index = infoIds.indexOf(info_id);
    infoIds[index] = name;
    const auto model_index = this->index(index, 0, {});
    emit this->dataChanged(model_index, model_index);
    emit infoIdsChanged();
}

void MusicListModel::addFileInfo(const QString& info_id, const QList<QString>& path_list)
{
    MusicDatabase::obj().addListInfo_FileIds(info_id, path_list);
    if (dataModel->curListInfoID == info_id) {
        dataModel->setCurListInfoID(info_id);
    }
    if (info_id == MusicDatabase::obj().playListInfo_Name) {
        dataModel->playModel = MusicDatabase::obj().getListInfo_FileIds(MusicDatabase::obj().playListInfo_Name);
        emit dataModel->playModelChanged();
    }
}
