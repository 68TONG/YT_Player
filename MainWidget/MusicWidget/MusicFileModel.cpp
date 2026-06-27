#include "MusicFileModel.h"

#include <QCollator>
#include <QFontMetrics>
#include "MusicWidget.h"

MusicFileModel::MusicFileModel(MusicWidget& dataModel, QObject* parent)
    : QAbstractItemModel(parent), dataModel(&dataModel)
{
    connect(this, &MusicFileModel::infoIdsChanged, this, [this]() {
        updateHeaderModel();
        updateFooterModel(); });
    connect(this, &MusicFileModel::sortTagChanged, this, [this]() {
        selectTag.removeAll(sortTag);
        if (sortTag != titleTag)
            selectTag.insert(0, sortTag);
        emit selectTagChanged();
        updateFooterModel();
        });
    connect(this, &MusicFileModel::selectTagChanged, this, [this]() {
        updateItemStretchFactor();
        if (this->rowCount() <= 0)
            return;
        if (this->dataModel->pathModel.srcData.count() != +true)
            return;
        // auto info = Music_DataBase::getObject().getListInfoPtr(this->dataModel->curListInfoID);
        // if (info != nullptr)
        //     info->SelectTag = selectTag;
        auto info_id = this->dataModel->getCurListInfoID();
        auto select_tag = DB::listToJsonByte<QString>(selectTag);
        MusicDatabase::obj().setListInfo(info_id, DB_ListInfo::I_SelectTag, select_tag);
        });
}

QModelIndex MusicFileModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || row >= infoIds.count()) return {};
    return this->createIndex(row, column);
}

QModelIndex MusicFileModel::parent(const QModelIndex& index) const
{
    return {};
}

int MusicFileModel::rowCount(const QModelIndex& parent) const
{
    return infoIds.count();
}

int MusicFileModel::columnCount(const QModelIndex& parent) const
{
    return +true;
}

QHash<int, QByteArray> MusicFileModel::roleNames() const
{
    return {
        {InfoId_Role, "infoId"},
        {TitleTag_Role, "titleTag"},
        {ArtistTag_Role, "artistTag"},
        {AlbumTag_Role, "albumTag"},
        {StarLevel_Role, "starLevel"},
        {PlayCount_Role, "playCount"},
        {AddPlayCount_Role, "addPlayCount"},
        {MusicDuration_Role, "musicDuration"},
        {MusicPath_Role, "musicPath"}
    };
}

QVariant MusicFileModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false)
        return {};

    const auto& info = infoList[infoIds[index.row()]];
    if (role == InfoId_Role) {
        return info.Id;
    }
    else if (role == TitleTag_Role) {
        return DB::jsTagFirst(info.fileTag, titleTag);
    }

    else if (role == StarLevel_Role) {
        return info.starLevel;
    }
    else if (role == PlayCount_Role) {
        return info.playCount;
    }
    else if (role == AddPlayCount_Role) {
        return info.addPlayCount;
    }
    else if (role == MusicPath_Role) {
        return info.filePath;
    }
    return {};
}

Q_INVOKABLE QVariant MusicFileModel::dataIndex(const int& index, int role) const
{
    return data(this->index(index, 0), role);
}

Q_INVOKABLE QList<QVariant> MusicFileModel::dataTag(const int& index, const QString& data) const
{
    if (index < 0 || index >= infoIds.count())
        return {};

    const int info_id = infoIds[index];
    const auto& info = infoList[info_id];
    return DB::jsTagList(info.fileTag[data]);
}

Q_INVOKABLE QVariant MusicFileModel::dataTag(const int& index, const QString& data, int ret_index) const
{
    return dataTag(index, data).value(ret_index, QVariant());
}

Q_INVOKABLE QList<QVariant> MusicFileModel::dataTag_InfoId(const int& info_id, const QString& data) const
{
    const auto& info = infoList.find(info_id) != infoList.end() ? infoList[info_id] : MusicDatabase::obj().getFileInfo(info_id);
    if (info.Id < 0) {
        return {};
    }
    return DB::jsTagList(info.fileTag[data]);
}

Q_INVOKABLE QVariant MusicFileModel::dataTag_InfoId(const int& info_id, const QString& data, int ret_index) const
{
    return dataTag_InfoId(info_id, data).value(ret_index, QVariant());
}

QList<QVariant> MusicFileModel::tagData(const int& index, const QString& tag_index) const
{
    if (index < 0 || index >= infoIds.count())
        return {};
    // const int info_id = infoID_List[index];
    // const auto& info = Music_DataBase::getObject().getItemInfo(info_id);

    const int info_id = infoIds[index];
    const auto& info = infoList[info_id];
    // qDebug() << info.fileTag[tag_index] << jsonListData(info.fileTag[tag_index]);
    return DB::jsTagList(info.fileTag[tag_index]);
}

Q_INVOKABLE void MusicFileModel::updateInfo(const int& info_id)
{
    const int index = infoIds.indexOf(info_id);
    if (index < 0) return;

    infoList[info_id] = MusicDatabase::obj().getFileInfo(info_id);
    const auto model_index = this->index(index, 0, {});
    emit this->dataChanged(model_index, model_index);
}

Q_INVOKABLE void MusicFileModel::updateInfo(const QList<int>& info_ids)
{
    for (auto it : info_ids) {
        updateInfo(info_ids);
    }
}

Q_INVOKABLE void MusicFileModel::setInfoIds(const QString& info_id)
{
    const auto&& info = MusicDatabase::obj().getListInfo(info_id);
    setInfoIds(info.fileInfoIds);

    sortTag = info.sortTag;
    emit sortTagChanged();

    selectTag = info.selectTag;
    emit selectTagChanged();
}

Q_INVOKABLE void MusicFileModel::setInfoIds(const QList<int>& info_ids)
{
    this->beginResetModel();
    infoIds = info_ids;
    infoList = MusicDatabase::obj().getFileInfo(infoIds);
    this->endResetModel();
    emit infoIdsChanged();
}

Q_INVOKABLE void MusicFileModel::sortInfoIds(const QString& sort_data)
{
    const auto info_id = dataModel->getCurListInfoID();
    if (info_id == MusicDatabase::playListInfo_Name) return;

    this->beginResetModel();
    if (dataModel->pathModel.srcData.count() == +true && info_id.isEmpty() == false) {
        MusicDatabase::obj().sortFileInfo(info_id, sort_data);
        infoIds = MusicDatabase::obj().getListInfo_FileIds(info_id);
    }
    else {
        MusicDatabase::obj().sortFileInfo(infoIds, sort_data);
    }
    this->endResetModel();
    emit infoIdsChanged();

    if (sort_data != "Id") {
        sortTag = sort_data;
        emit sortTagChanged();
    }
}

Q_INVOKABLE void MusicFileModel::searchInfoIds(const QString& search_tag, const QString& header_tag)
{
    this->beginResetModel();
    infoIds = MusicDatabase::obj().searchFileInfo(infoIds, search_tag, header_tag);
    this->endResetModel();
    emit infoIdsChanged();
}

// void MusicFileModel::setInfoID_List(const QString& info_id)
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

// void MusicFileModel::setInfoID_List(const QList<int>& info_ids)
// {
//     this->beginResetModel();
//     infoID_List = info_ids;
//     emit infoIdsChanged();
//     this->endResetModel();
// }

// void MusicFileModel::sortInfoID_List(const QString& sort_data)
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

void MusicFileModel::setViewPosition(const QString& data)
{
    if (sortTag.isEmpty() || data.isEmpty())
        return;

    for (int i = 0; i < infoIds.count(); i++) {
        int info_id = infoIds[i];
        auto info_data = DB::jsTagFirst(infoList[info_id].fileTag.value(sortTag)).toString();
        auto info_data_py = YT_Func::getObject().getChinesePinYin(info_data);
        auto compare_data = QString::compare(data, info_data_py, Qt::CaseInsensitive);

        if (compare_data > 0)
            continue;
        emit viewPositionChanged(i);
        return;
    }
}

void MusicFileModel::setFooterPosition(const QString& data)
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

void MusicFileModel::updateHeaderModel()
{
    headerModel = MusicDatabase::obj().getFileInfo_TagKeys(infoIds);
    // headerModel = Music_DataBase::getObject().getListInfo_TagHeaders(infoID_List);
    headerModel.removeIf([](const QString& data) {
        return data.contains("YT_Lyrics_Url") || data.contains("YT_Cover_Url");
        });
    emit headerModelChanged();
}

void MusicFileModel::updateFooterModel()
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

void MusicFileModel::updateItemStretchFactor()
{
    itemStretchFactor.clear();
    if (selectTag.isEmpty() || infoIds.isEmpty()) {
        emit itemStretchFactorChanged();
        return;
    }

    QList<int> size_list = {};
    static const auto getFileInfo_TagValues_Size = [this](const QString& data)->int {
        const auto font_metrics = QFontMetrics(qApp->font());
        int max_size = font_metrics.boundingRect(data).width();
        auto tag_values = MusicDatabase::obj().getFileInfo_TagValues(infoIds.mid(0, 15), data);
        for (const auto& it : tag_values) {
            max_size = qMax(max_size, font_metrics.boundingRect(it).width());
        }
        return max_size;
        };

    for (const auto& it : selectTag) {
        size_list.append(getFileInfo_TagValues_Size(it));
    }
    auto title_size = getFileInfo_TagValues_Size(titleTag);
    size_list[0] = qMax(size_list[0], title_size);

    int max_size = 0;
    for (int it : size_list)
        max_size = max_size + it;
    for (int it : size_list)
        itemStretchFactor.append(double(it) / double(max_size));

    emit itemStretchFactorChanged();
}
