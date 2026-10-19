#include "MusicFileModel.h"

#include <QCollator>
#include <QCoreApplication>
#include <QFutureWatcher>
#include <QFontMetrics>
#include <QMetaObject>
#include <QtConcurrent/QtConcurrentRun>
#include "MusicWidget.h"

MusicFileModel::MusicFileModel(MusicWidget& dataModel, QObject* parent)
    : QAbstractItemModel(parent), dataModel(&dataModel)
{
    connect(this, &MusicFileModel::infoIdsChanged, this, [this]() {
        updateHeaderModel();
        updateFooterModel();
        // setViewPosition("#");
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
        { InfoId_Role, "infoId" },
        { TitleTag_Role, "titleTag" },
        { ArtistTag_Role, "artistTag" },
        { AlbumTag_Role, "albumTag" },
        { StarLevel_Role, "starLevel" },
        { PlayCount_Role, "playCount" },
        { AddPlayCount_Role, "addPlayCount" },
        { MusicDuration_Role, "musicDuration" },
        { MusicPath_Role, "musicPath" }
    };
}

QVariant MusicFileModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() == false)
        return {};

    const auto& info = getInfoList(infoIds[index.row()]);
    if (role == InfoId_Role) {
        return info.Id;
    }
    else if (role == TitleTag_Role) {
        return DB::jsTagFirst(info.musicTag, titleTag);
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
    return this->data(this->index(index, 0), role);
}

Q_INVOKABLE QList<QVariant> MusicFileModel::dataTag(const int& index, const QString& data) const
{
    if (index < 0 || index >= infoIds.count())
        return {};

    const int info_id = infoIds[index];
    const auto& info = getInfoList(info_id);
    return DB::jsTagList(info.musicTag[data]);
}

Q_INVOKABLE QVariant MusicFileModel::dataTag(const int& index, const QString& data, int ret_index) const
{
    return dataTag(index, data).value(ret_index, QVariant());
}

Q_INVOKABLE QList<QVariant> MusicFileModel::dataTag_InfoId(const int& info_id, const QString& data) const
{
    const auto& info = getInfoList(info_id);
    if (info.Id < 0) {
        return {};
    }
    return DB::jsTagList(info.musicTag[data]);
}

Q_INVOKABLE QVariant MusicFileModel::dataTag_InfoId(const int& info_id, const QString& data, int ret_index) const
{
    return dataTag_InfoId(info_id, data).value(ret_index, QVariant());
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
        updateInfo(it);
    }
}

Q_INVOKABLE void MusicFileModel::setInfoIds(const QString& info_id)
{
    YT_TimeDebugger de_(QStringLiteral("MusicFileModel All"));
    YT_TimeDebugger de(QStringLiteral("MusicFileModel"));
    const auto&& info = MusicDatabase::obj().getListInfo(info_id);
    de << "getListInfo";

    setSelectTag(info.selectTag);
    de << "setSelectTag";

    setSortTag(info.sortTag);
    de << "setSortTag";

    setInfoIds(info.fileInfoIds);
    de << "setInfoIds";
}

Q_INVOKABLE void MusicFileModel::setInfoIds(const QList<int>& info_ids)
{
    this->beginResetModel();
    infoIds = info_ids;
    infoList = MusicDatabase::obj().getFileInfo(infoIds.mid(0, 12));
    this->endResetModel();
    emit infoIdsChanged();

    if (infoIds.size() <= 12)
        return;

    const quint64 generation = ++infoListGeneration;
    auto watcher = new QFutureWatcher<QHash<int, MusicFileInfo>>(this);
    connect(watcher, &QFutureWatcher<QHash<int, MusicFileInfo>>::finished, this, [this, watcher, generation]() {
        const auto loadedInfoList = watcher->result();
        watcher->deleteLater();
        if (generation != infoListGeneration)
            return;

        infoList = loadedInfoList;
        });
    watcher->setFuture(QtConcurrent::run([ids = infoIds]() {
        return MusicDatabase::forCurrentThread().getFileInfo(ids);
        }));
}

Q_INVOKABLE void MusicFileModel::sortInfoIds(QString sort_data)
{
    sort_data = sort_data == "Id" ? MusicListInfo::I_ReverseSortTag : sort_data;
    const auto info_id = dataModel->getCurListInfoID();
    if (info_id == MusicDatabase::playListInfo_Name) return;

    ++infoListGeneration;
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

    if (sort_data != MusicListInfo::I_ReverseSortTag) {
        setSortTag(sort_data);
    }
}

Q_INVOKABLE void MusicFileModel::searchInfoIds(QString search_tag, QString header_tag)
{
    ++infoListGeneration;
    this->beginResetModel();
    infoIds = MusicDatabase::obj().searchMusicFileInfo(infoIds, search_tag, header_tag);
    this->endResetModel();
    emit infoIdsChanged();
}

void MusicFileModel::setViewPosition(QString data)
{
    if (sortTag.isEmpty() || data.isEmpty())
        return;

    for (int i = 0; i < infoIds.count(); i++) {
        int info_id = infoIds[i];
        auto info_data = DB::jsTagFirst(getInfoList(info_id).musicTag.value(sortTag)).toString();
        auto info_data_py = YT_Func::getObject().getChinesePinYin(info_data);
        if (info_data_py.isEmpty())
            continue;

        auto compare_data = QString::compare(data, info_data_py[0], Qt::CaseInsensitive);
        if (compare_data > 0)
            continue;

        emit viewPositionChanged(i);
        setFooterPosition(i);
        break;
    }
}

void MusicFileModel::setViewPositionEx(QString index, QString data)
{
    if (sortTag.isEmpty() || index.isEmpty() || data.isEmpty())
        return;

    for (int i = 0; i < infoIds.count(); i++) {
        int info_id = infoIds[i];
        auto info_data = DB::jsTagFirst(getInfoList(info_id).musicTag.value(sortTag)).toString();
        auto info_data_py = YT_Func::getObject().getChinesePinYin(info_data);
        if (info_data_py.size() < 2)
            continue;

        auto compare_data = QString::compare(index, info_data_py[0], Qt::CaseInsensitive);
        if (compare_data > 0)
            continue;

        compare_data = QString::compare(data, info_data_py[1], Qt::CaseInsensitive);
        if (compare_data > 0)
            continue;

        emit viewPositionChanged(i);
        break;
    }
}

void MusicFileModel::setFooterPosition(int index)
{
    if (sortTag.isEmpty() || index < 0 || index >= infoIds.count())
        return;

    int info_id = infoIds[index];
    auto src_data = DB::jsTagFirst(getInfoList(info_id).musicTag.value(sortTag)).toString().left(1);
    auto data = YT_Func::getObject().getChineseFirstPinYin(src_data);
    for (int i = 0; i < footerModel.count(); i++) {
        auto compare_data = QString::compare(data, footerModel[i], Qt::CaseInsensitive);
        if (compare_data > 0)
            continue;

        emit footerPositionChanged(compare_data == 0 ? i : i - 1);
        break;
    }

    updateFooterModelEx(index, data);
}

MusicFileInfo& MusicFileModel::getInfoList(int info_id)
{
    auto it = infoList.find(info_id);
    if (it != infoList.end()) return *it;

    return *infoList.insert(info_id, MusicDatabase::obj().getFileInfo(info_id));
}

const MusicFileInfo& MusicFileModel::getInfoList(int info_id) const
{
    return const_cast<MusicFileModel*>(this)->getInfoList(info_id);
}

void MusicFileModel::setSortTag(const QString& data)
{
    if (sortTag == data) return;

    sortTag = data;
    emit sortTagChanged();

    updateFooterModel();

    auto m_selectTag = selectTag;
    m_selectTag.removeAll(sortTag);
    if (sortTag != titleTag && sortTag.isEmpty() == false) m_selectTag.insert(0, sortTag);
    setSelectTag(m_selectTag);
}

void MusicFileModel::setSelectTag(const QList<QString>& data)
{
    if (selectTag == data) return;

    selectTag = data;
    emit selectTagChanged();

    updateItemStretchFactor();

    if (this->rowCount() <= 0)
        return;
    if (this->dataModel->pathModel.srcData.count() != +true)
        return;

    auto info_id = this->dataModel->getCurListInfoID();
    auto info_data = DB::listToJsonByte<QString>(data);
    MusicDatabase::obj().setListInfo(info_id, DB_ListInfo::I_SelectTag, info_data);
}

void MusicFileModel::updateHeaderModel()
{
    const quint64 generation = ++headerModelGeneration;
    auto watcher = new QFutureWatcher<QList<QString>>(this);
    connect(watcher, &QFutureWatcher<QList<QString>>::finished, this, [this, watcher, generation]() {
        // YT_TimeDebugger de("updateHeaderModel");
        auto updatedHeaderModel = watcher->result();
        watcher->deleteLater();
        if (generation != headerModelGeneration)
            return;

        headerModel = updatedHeaderModel;
        emit headerModelChanged();
        });
    watcher->setFuture(QtConcurrent::run([ids = infoIds]() {
        // YT_TimeDebugger de("headerModelChanged QtConcurrent");
        auto headerModel = MusicDatabase::forCurrentThread().getMusicFileInfo_TagKeys(ids);
        headerModel.removeIf([](const QString& data) {
            return data.contains("YT_Lyrics_Url") || data.contains("YT_Cover_Url");
            });
        return headerModel;
        }));
}

void MusicFileModel::updateFooterModel()
{
    if (sortTag.isEmpty() || infoIds.isEmpty())
        return;

    const quint64 generation = ++footerModelGeneration;
    auto watcher = new QFutureWatcher<QList<QString>>(this);
    connect(watcher, &QFutureWatcher<QList<QString>>::finished, this, [this, watcher, generation]() {
        // YT_TimeDebugger de("updateFooterModel");
        auto updatedFooterModel = watcher->result();
        watcher->deleteLater();
        if (generation != footerModelGeneration)
            return;

        footerModel = updatedFooterModel;
        emit footerModelChanged();
        });
    watcher->setFuture(QtConcurrent::run([ids = infoIds, sortTag = sortTag]() {
        // YT_TimeDebugger de("footerModelChanged QtConcurrent");
        QList<QString> footerModel = { QString('#'), QString('~') };
        for (char c = 'A'; c <= 'Z'; ++c)
            footerModel += QString(c);

        if (sortTag.isEmpty() == false) {
            const auto tagValues = MusicDatabase::forCurrentThread().getMusicFileInfo_TagValues(ids, sortTag);
            auto chineseDict = YT_Func::getObject().getChinese_Dict();
            for (const auto& data : tagValues) {
                if (data.isEmpty())
                    continue;

                const auto it = data[0];
                if (chineseDict->GetCharacter(QString(it).toStdString()))
                    continue;
                if (footerModel.contains(it))
                    continue;
                if (it.unicode() < 128)
                    continue;
                footerModel += QString(it);
            }
        }

        footerModel.sort(Qt::CaseInsensitive);
        return footerModel;
        }));
}

void MusicFileModel::updateFooterModelEx(int index, QString data)
{
    if (sortTag.isEmpty() || index < 0 || index >= infoIds.count() || infoIds.isEmpty())
        return;

    const quint64 generation = ++footerModelExGeneration;
    auto watcher = new QFutureWatcher<QList<QString>>(this);
    connect(watcher, &QFutureWatcher<QList<QString>>::finished, this, [this, watcher, generation]() {
        auto updatedFooterModelEx = watcher->result();
        watcher->deleteLater();
        if (generation != footerModelExGeneration)
            return;

        footerModelEx = updatedFooterModelEx;
        emit footerModelExChanged();
        });
    watcher->setFuture(QtConcurrent::run([ids = infoIds, infos = infoList, sortTag = sortTag, index, data]() mutable {
        QList<QString> footerModelEx = { QString('#'), QString('~') };
        const auto appendInfoAt = [&ids, &infos, &footerModelEx, sortTag, data](int i) {
            const auto info_id = ids[i];
            if (infos.find(info_id) == infos.end()) {
                infos.insert(info_id, MusicDatabase::forCurrentThread().getFileInfo(info_id));
            }

            const auto info = infos[info_id];
            const auto data_src = DB::jsTagFirst(info.musicTag.value(sortTag)).toString();
            const auto data_py = YT_Func::getObject().getChinesePinYin(data_src);
            if (data_py.size() < 2)
                return true;
            if (data != data_py[0].toUpper())
                return false;

            const auto it = data_py[1].toUpper();
            if (footerModelEx.contains(it))
                return true;
            if (it.isLetter() == false && it.unicode() < 128)
                return true;
            footerModelEx += QString(it);
            return true;
            };

        for (int i = index; i < ids.count(); ++i) {
            if (appendInfoAt(i) == false)
                break;
        }
        for (int i = index; i >= 0; --i) {
            if (appendInfoAt(i) == false)
                break;
        }

        footerModelEx.sort(Qt::CaseInsensitive);
        return footerModelEx;
        }));
}

void MusicFileModel::updateItemStretchFactor()
{
    const quint64 generation = ++itemStretchFactorGeneration;
    if (selectTag.isEmpty() || infoIds.isEmpty()) return;

    auto watcher = new QFutureWatcher<QList<double>>(this);
    connect(watcher, &QFutureWatcher<QList<double>>::finished, this, [this, watcher, generation]() {
        auto updatedStretchFactors = watcher->result();
        watcher->deleteLater();
        if (generation != itemStretchFactorGeneration)
            return;

        itemStretchFactor = updatedStretchFactors;
        emit itemStretchFactorChanged();
        });
    watcher->setFuture(QtConcurrent::run([ids = infoIds.mid(0, 12), selectTag = selectTag, font = qApp->font()]() {
        QList<double> updatedStretchFactors;
        const QFontMetrics fontMetrics(font);
        auto getTagWidth = [&fontMetrics, &ids](const QString& tag) {
            int maxWidth = fontMetrics.boundingRect(tag).width();
            const auto tagValues = MusicDatabase::forCurrentThread().getMusicFileInfo_TagValues(ids, tag);
            for (const auto& value : tagValues)
                maxWidth = qMax(maxWidth, fontMetrics.boundingRect(value).width());
            return maxWidth;
            };

        QList<int> widths;
        for (const auto& tag : selectTag)
            widths.append(getTagWidth(tag));
        widths[0] = qMax(widths[0], getTagWidth(Media::MetaData_Title));

        int totalWidth = 0;
        for (const int width : widths)
            totalWidth += width;
        for (const int width : widths)
            updatedStretchFactors.append(double(width) / double(totalWidth));

        return updatedStretchFactors;
        }));
}
