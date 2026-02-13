#include "MusicItemModel.h"

#include <QCollator>
#include "MusicWidget.h"

MusicItemModel::MusicItemModel(MusicWidget& dataModel, QObject* parent)
    : QAbstractItemModel(parent), dataModel(&dataModel)
{
    connect(this, &MusicItemModel::infoIdsChanged, this, [this]() {
        updateHeaderModel();
        updateFooterModel(); });
    connect(this, &MusicItemModel::sortTagChanged, this, [this]() {
        selectTag.removeAll(sortTag);
        if (sortTag != titleTag)
            selectTag.insert(0, sortTag);
        emit selectTagChanged();
        updateFooterModel();
        });
    connect(this, &MusicItemModel::selectTagChanged, this, [this]() {
        if (this->rowCount() <= 0)
            return;
        if (this->dataModel->curPath.count() != true)
            return;
        // auto info = Music_DataBase::getObject().getListInfoPtr(this->dataModel->curListInfoID);
        // if (info != nullptr)
        //     info->SelectTag = selectTag;
        auto info_id = this->dataModel->getCurListInfoID();
        auto select_tag = DB::listToJsonByte<QString>(selectTag);
        MusicDatabase::obj().setListInfo(info_id, DB::I_ListInfo_SelectTag, select_tag);
        });
}

QModelIndex MusicItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || row >= infoIds.count())
        return {};
    return this->createIndex(row, column);
}

QModelIndex MusicItemModel::parent(const QModelIndex& index) const
{
    return {};
}

int MusicItemModel::rowCount(const QModelIndex& parent) const
{
    return infoIds.count();
}

int MusicItemModel::columnCount(const QModelIndex& parent) const
{
    return +true;
}

QHash<int, QByteArray> MusicItemModel::roleNames() const
{
    return {
        {InfoID_Role, "infoID"},
        {TitleTag_Role, "titleTag"},
        {Path_Role, "path"}
    };
}

QVariant MusicItemModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false)
        return {};
    const int info_id = infoIds[index.row()];
    const auto& info = infoList[info_id];
    const auto& tags = info.Tags;

    if (role == InfoID_Role) {
        return info.ID;
    }
    else if (role == TitleTag_Role) {
        return tags.value(titleTag).toString();
    }
    else if (role == Path_Role) {
        return info.Path;
    }
    return {};
}

QList<QVariant> MusicItemModel::tagData(const int& index, const QString& tag_index) const
{
    if (index < 0 || index >= infoIds.count())
        return {};
    // const int info_id = infoID_List[index];
    // const auto& info = Music_DataBase::getObject().getItemInfo(info_id);

    const int info_id = infoIds[index];
    const auto& info = infoList[info_id];
    // qDebug() << info.Tags[tag_index] << jsonListData(info.Tags[tag_index]);
    return DB::jsTagList(info.Tags[tag_index]);
}

Q_INVOKABLE void MusicItemModel::setInfoIds(const QString& info_id)
{
    const auto&& info = MusicDatabase::obj().getListInfo(info_id);
    setInfoIds(info.FileInfoIds);

    sortTag = info.SortTag;
    emit sortTagChanged();

    selectTag = info.SelectTag;
    emit selectTagChanged();
}

Q_INVOKABLE void MusicItemModel::setInfoIds(const QList<int>& info_ids)
{
    this->beginResetModel();
    infoIds = info_ids;
    infoList = MusicDatabase::obj().getFileInfo(infoIds);
    emit infoIdsChanged();
    this->endResetModel();
}

Q_INVOKABLE void MusicItemModel::sortInfoIds(const QString& sort_data)
{
    this->beginResetModel();
    const auto info_id = dataModel->getCurListInfoID();
    if (dataModel->curPath.count() == +true && info_id.isEmpty() == false) {
        MusicDatabase::obj().sortFileInfo(info_id, sort_data);
        infoIds = MusicDatabase::obj().getListInfo_FileIds(info_id);
    }
    else {
        MusicDatabase::obj().sortFileInfo(infoIds, sort_data);
    }
    emit infoIdsChanged();
    this->endResetModel();

    if (sort_data != "ID") {
        sortTag = sort_data;
        emit sortTagChanged();
    }
}

Q_INVOKABLE void MusicItemModel::searchInfoIds(const QString& search_data)
{
    this->beginResetModel();
    infoIds = MusicDatabase::obj().searchFileInfo(infoIds, search_data);
    emit infoIdsChanged();
    this->endResetModel();
}

// void MusicItemModel::setInfoID_List(const QString& info_id)
// {
//     this->beginResetModel();
//     const auto&& info = Music_DataBase::getObject().getListInfo(info_id);
//     infoID_List = info.ID_List;
//     emit infoIdsChanged();
//     this->endResetModel();

//     sortTag = info.SortTag;
//     emit sortTagChanged();

//     selectTag = info.SelectTag;
//     emit selectTagChanged();
// }

// void MusicItemModel::setInfoID_List(const QList<int>& info_ids)
// {
//     this->beginResetModel();
//     infoID_List = info_ids;
//     emit infoIdsChanged();
//     this->endResetModel();
// }

// void MusicItemModel::sortInfoID_List(const QString& sort_data)
// {
//     this->beginResetModel();

//     const auto info_id = dataModel->getCurListInfoID();
//     if (dataModel->curPath.count() == +true && info_id != "") {
//         Music_DataBase::getObject().sortListInfo(info_id, sort_data);
//         infoID_List = Music_DataBase::getObject().getListInfo(info_id).ID_List;
//     }
//     else {
//         Music_DataBase::getObject().sortListInfo(infoID_List, sort_data);
//     }
//     emit infoIdsChanged();

//     this->endResetModel();

//     if (sort_data != YT_INFO_PROPERTY_ID)
//     {
//         sortTag = sort_data;
//         emit sortTagChanged();
//     }
// }

void MusicItemModel::setViewPosition(const QString& data)
{
    if (sortTag.isEmpty() || data.isEmpty())
        return;

    for (int i = 0; i < infoIds.count(); i++) {
        int info_id = infoIds[i];
        auto info_data = DB::jsTagFirst(infoList[info_id].Tags.value(sortTag)).toString();
        auto info_data_py = YT_Func::getObject().getChinesePinYin(info_data);
        auto compare_data = QString::compare(data, info_data_py, Qt::CaseInsensitive);

        if (compare_data > 0)
            continue;
        emit viewPositionChanged(i);
        return;
    }
}

void MusicItemModel::setFooterPosition(const QString& data)
{
    if (sortTag.isEmpty() || data.isEmpty())
        return;

    const auto data_pin_yin = YT_Func::getObject().getChineseFirstPinYin(data);
    for (int i = 0; i < footerModel.count(); i++) {
        const auto compare_data = QString::compare(data_pin_yin, footerModel[i], Qt::CaseInsensitive);
        if (compare_data > 0)
            continue;

        emit footerPositionChanged(i);
        return;
    }
}

void MusicItemModel::updateHeaderModel()
{
    headerModel = MusicDatabase::obj().getFileInfo_TagKeys(infoIds);
    // headerModel = Music_DataBase::getObject().getListInfo_TagHeaders(infoID_List);
    headerModel.removeIf([](const QString& data) {
        return data.contains("YT_Lyrics_Url") || data.contains("YT_Cover_Url");
        });
    emit headerModelChanged();
}

void MusicItemModel::updateFooterModel()
{
    footerModel.clear();
    footerModel += QString('#');
    for (char c = 'A'; c <= 'Z'; ++c) {
        footerModel += QString(c);
    }
    if (sortTag.isEmpty() == false) {
        auto chinese_dict = YT_Func::getObject().getChinese_Dict();
        auto tag_datas = MusicDatabase::obj().getFileInfo_TagValues(infoIds, sortTag);
        // auto&& tag_datas = Music_DataBase::getObject().getListInfo_TagDatas(infoID_List, sortTag);
        for (auto tag : tag_datas)
        {
            auto it = tag[0];
            if ((it >= 'A' && it <= 'Z') || (it >= 'a' && it <= 'z'))
                continue;
            if (chinese_dict->GetCharacter(QString(it).toStdString()))
                continue;
            if (footerModel.indexOf(it) >= 0)
                continue;
            if (it.unicode() < 128)
                continue;
            footerModel += it;
        }
    }
    footerModel.sort(Qt::CaseInsensitive);
    emit footerModelChanged();
}
