#include "MusicItemModel.h"

#include "MusicWidget.h"
#include <QCollator>

MusicItemModel::MusicItemModel(MusicWidget &dataModel, QObject *parent)
    : QAbstractItemModel(parent)
    , dataModel(&dataModel)
{

}

QModelIndex MusicItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0 || row >= infoID_List.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex MusicItemModel::parent(const QModelIndex &index) const
{
    return {};
}

int MusicItemModel::rowCount(const QModelIndex &parent) const
{
    return infoID_List.count();
}

int MusicItemModel::columnCount(const QModelIndex &parent) const
{
    return +true;
}

QHash<int, QByteArray> MusicItemModel::roleNames() const
{
    return {
        {InfoRole_InfoID, "infoID"},
        {InfoRole_Title, "title"},
        {InfoRole_MetaData_One, "metaData_One"},
        {InfoRole_MetaData_Two, "metaData_Two"},
        {InfoRole_Duration, "duration"},
        {InfoRole_Path, "path"}
    };
}

QVariant MusicItemModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() == false) return {};
    const int info_id = infoID_List[index.row()];
    auto &&info = Music_DataBase::getObject().getItemInfo(info_id);

    if (role == InfoRole_InfoID) {
        return info.ID;
    } else if (role == InfoRole_Title) {
        const auto &data = info.musicData[Media::MetaData_Title].toArray();
        if (data.isEmpty()) return {};
        return data[0].toVariant();
    } else if (role == InfoRole_MetaData_One) {
        const auto &data = info.musicData[dataModel->metaData_One].toArray();
        if (data.isEmpty()) return {};
        return data[0].toVariant();
    } else if (role == InfoRole_MetaData_Two) {
        const auto &data = info.musicData[dataModel->metaData_Two].toArray();
        if (data.isEmpty()) return {};
        return data[0].toVariant();
    } else if (role == InfoRole_Duration) {
        return 0;
    } else if (role == InfoRole_Path) {
        return info.Path;
    }
    return {};
}

void MusicItemModel::setInfoID_List(const int &info_id)
{
    this->beginResetModel();
    infoID_List = Music_DataBase::getObject().getListInfo(info_id).ID_List;
    emit infoID_ListChanged();
    this->endResetModel();
}

void MusicItemModel::setInfoID_List(const QList<int> &info_ids)
{
    this->beginResetModel();
    infoID_List = info_ids;
    emit infoID_ListChanged();
    this->endResetModel();
}

void MusicItemModel::sortInfoID_List(const QString &sort_index)
{
    this->beginResetModel();
    const int info_id = dataModel->getCurListInfoID();
    if (dataModel->curPath.count() == true && info_id >= 0) {
        Music_DataBase::getObject().sortListInfo(info_id, sort_index);
        infoID_List = Music_DataBase::getObject().getListInfo(info_id).ID_List;
    } else {
        Music_DataBase::getObject().sortItemInfo(infoID_List, sort_index);
    }
    emit infoID_ListChanged();
    this->endResetModel();
}

void MusicItemModel::setViewPosition(const QString &data)
{
    QCollator collator;
    collator.setLocale(QLocale(QLocale::Chinese, QLocale::China));
    for (int i = 0; i < infoID_List.count(); i++) {
        int info_id = infoID_List[i];
        auto &&info = Music_DataBase::getObject().getItemInfo(info_id);
        auto &&info_data = info.musicData[Media::MetaData_Title].toArray().at(0).toString();
        auto &&compare_data = collator.compare(data, info_data);

        int index = i;
        // qDebug() << data << info_data << compare_data << index << i;
        if (compare_data > 0) continue ;
        emit viewPositionChanged(index);
        return ;
    }
}
