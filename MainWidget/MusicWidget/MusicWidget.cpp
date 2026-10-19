#include "MusicWidget.h"

#include "YT_AudioOutput.h"
#include <QRandomGenerator>

MusicWidget::MusicWidget(QQuickItem* parent)
    : QQuickItem{ parent }, fileModel(*this), listModel(*this), pathModel(this, "MusicWidget"), searchModel(this)
{
    this->setFlag(QQuickItem::ItemHasContents, true);

    curListInfoID = listModel.infoIds.first();
    fileModel.setInfoIds(curListInfoID);
    pathModel.initData(curListInfoID);
    playModel = MusicDatabase::obj().getListInfo_FileIds(MusicDatabase::playListInfo_Name);

    connect(&pathModel, &YT_PathWidgetModel::srcDataChanged, this, [this]() {
        if (pathModel.isSetDataing) return;
        if (pathModel.srcData.isEmpty()) return;
        const auto info_id = pathModel.srcData[0];
        if (MusicDatabase::obj().getListInfo(info_id).name.isEmpty())
            return;

        pathModel.isSetDataing = true;
        setCurListInfoID(info_id);
        pathModel.isSetDataing = false;

        for (int i = 1; i < pathModel.srcData.count(); i++) {
            fileModel.searchInfoIds(pathModel.pathData[i], pathModel.typeData[i]);
        }
        });
    connect(&YT_AudioOutput::getObject(), &YT_AudioOutput::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == YT_AudioOutput::EndOfMedia) {
            int info_id = getPlayInfoId();
            if (info_id >= 0) {
                int data = MusicDatabase::obj().getFileInfo(info_id, MusicFileInfo::I_PlayCount).toInt();
                MusicDatabase::obj().setFileInfo(info_id, MusicFileInfo::I_PlayCount, data + 1);
                fileModel.updateInfo(info_id);
            }
            playNextMusic();
        }
        });
    connect(qApp, &QGuiApplication::aboutToQuit, this, [this]() {
        for (auto it : updateTagData) {
            if (it) it->waitForFinished();
        }
        });
    connect(this, &MusicWidget::playIndexChanged, this, [this]() {
        emit playInfoIdChanged();
        });
}

Q_INVOKABLE void MusicWidget::playMusic(int data)
{
    if (curListInfoID == MusicDatabase::playListInfo_Name) return playMusic_PlayIndex(data);

    playModel.clear();
    int index = -1;
    const auto& fileModel_infoIds = fileModel.infoIds;
    if (playMode == "qrc:/Ui_Image/shuffle.png")
    {
        playModel.append(fileModel_infoIds[data]);
        for (int i = 0; i < fileModel_infoIds.count(); ++i)
        {
            int index = QRandomGenerator::global()->bounded(fileModel_infoIds.count());
            playModel.append(fileModel_infoIds[index]);
        }
        index = 0;
    }
    else if (playMode == "qrc:/Ui_Image/single_loop.png")
    {
        playModel = { fileModel_infoIds[data] };
        index = 0;
    }
    else if (playMode == "qrc:/Ui_Image/list_loop.png")
    {
        playModel = fileModel_infoIds;
        index = data;
    }
    emit playModelChanged();

    MusicDatabase::obj().setListInfo(MusicDatabase::playListInfo_Name, MusicListInfo::I_SelectTag, DB::listToJsonByte<QString>(fileModel.selectTag));
    MusicDatabase::obj().setListInfo_FileIds(MusicDatabase::playListInfo_Name, playModel);
    playMusic_PlayIndex(index);
}

Q_INVOKABLE void MusicWidget::playMusic_Path(QString data)
{
    YT_AudioOutput::getObject().setSource(data);
    YT_AudioOutput::getObject().play();

    const int infoId = MusicDatabase::obj().getFileInfo_Id(data);
    playIndex = playModel.indexOf(infoId);
    emit playIndexChanged();

    updateMusicInfo(infoId);
}

Q_INVOKABLE void MusicWidget::playMusic_InfoId(int data)
{
    playMusic_Path(MusicDatabase::obj().getFileInfo(data, DB_FileInfo::I_FilePath).toString());
}

Q_INVOKABLE void MusicWidget::playMusic_PlayIndex(int data)
{
    playMusic_Path(MusicDatabase::obj().getFileInfo(playModel.value(data, -1), DB_FileInfo::I_FilePath).toString());
}

Q_INVOKABLE void MusicWidget::playNextMusic()
{
    int index = playIndex + 1;
    if (index < 0 || index >= playModel.size()) {
        index = 0;
    }
    playMusic_PlayIndex(index);
}

Q_INVOKABLE void MusicWidget::playPreviousMusic()
{
    int index = playIndex - 1;
    if (index < 0 || index >= playModel.size()) {
        index = 0;
    }
    playMusic_PlayIndex(index);
}

void MusicWidget::addPlayMusic(const QList<int>& info_ids)
{
    int insert_index = qMax(playIndex + 1, 0);

    if (info_ids.isEmpty()) return;

    playModel.insert(playModel.begin() + insert_index, info_ids.count(), 0);
    std::copy(info_ids.begin(), info_ids.end(), playModel.begin() + insert_index);

    for (const auto& info_id : info_ids) {
        int data = MusicDatabase::obj().getFileInfo(info_id, MusicFileInfo::I_AddPlayCount).toInt();
        MusicDatabase::obj().setFileInfo(info_id, MusicFileInfo::I_AddPlayCount, data + 1);
        fileModel.updateInfo(info_id);
    }
    emit playModelChanged();

    MusicDatabase::obj().setListInfo_FileIds(MusicDatabase::playListInfo_Name, playModel);
    if (getCurListInfoID() == MusicDatabase::playListInfo_Name) {
        fileModel.setInfoIds(playModel);
    }
}

void MusicWidget::addPlayMusic_Path(const QList<QString>& path_list)
{
    QList<int> info_ids;
    for (auto it : path_list) {
        int info_id = MusicDatabase::obj().addFileInfo(it);
        if (info_id >= 0) info_ids += info_id;
    }
    addPlayMusic(info_ids);
}

Q_INVOKABLE int MusicWidget::updateMusicInfo(int info_id)
{
    /////////////////////////////////////////////////////////////////////////////////////////
    return -1;
    if (MusicDatabase::obj().getFileInfo(info_id, DB_FileInfo::I_IsValid).toBool() == false) {
        qDebug() << "MusicWidget::updateMusicInfo info_id: " << info_id << " is not valid";
        return -1;
    }
    if (updateTagData.find(info_id) != updateTagData.end()) {
        qDebug() << "MusicWidget::updateMusicInfo info_id: " << info_id << " is updateing";
        return -2;
    }

    auto info = MusicDatabase::obj().getFileInfo(info_id);
    auto watcher = new QFutureWatcher<std::tuple<QString, QJsonObject>>(this);
    connect(watcher, &QFutureWatcher<std::tuple<QString, QJsonObject>>::finished, this, [this, info_id, watcher]() {
        const auto file_path = std::get<QString>(watcher->result());
        if (file_path.isEmpty()) {
            watcher->deleteLater();
            updateTagData[info_id] = nullptr;
            return;
        }
        updateTagModel.push_front(info_id);
        emit updateTagModelChanged();
        });

    updateTagData[info_id] = watcher;
    auto future = QtConcurrent::run([info]() -> std::tuple<QString, QJsonObject> {
        QJsonObject info_tags = {};
        const auto info_path = info.filePath;
        MusicFileInfo::getMusicMetaData(info_path, info_tags);

        auto json_list = YT_Func::getTH_Wiki("https://thwiki.cc/歌词:" + DB::jsTagFirst(info_tags, Media::MetaData_Title).toString());
        if (json_list.isEmpty()) {
            json_list = YT_Func::getTH_Wiki("https://thwiki.cc/" + DB::jsTagFirst(info_tags, Media::MetaData_Album).toString());
        }
        if (json_list.isEmpty()) {
            const QString error = " MusicWidget::updateMusicInfo getTH_Wiki isEmpty";
            qDebug() << info_path << error;
            qDebug().noquote() << "=== info_tags ===" << '\n' << QJsonDocument(info_tags).toJson(QJsonDocument::Indented) << '\n';
            // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
            return { QString(), info_tags };
        }

        int index = YT_Func::getBestMusicTag(json_list, info_tags, QFileInfo(info_path).absoluteDir().dirName());
        if (index == INT_MIN) {
            const QString error = " MusicWidget::updateMusicInfo best_json duplicate";
            qDebug() << info_path << error;
            qDebug().noquote() << "=== info_tags ===" << '\n' << QJsonDocument(info_tags).toJson(QJsonDocument::Indented) << '\n';
            // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
            return { QString(), info_tags };
        }
        if (index < 0) {
            const QString error = " MusicWidget::updateMusicInfo best_json nullptr";
            qDebug() << info_path << error;
            qDebug().noquote() << "=== info_tags ===" << '\n' << QJsonDocument(info_tags).toJson(QJsonDocument::Indented) << '\n';
            // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
            return { QString(), info_tags };
        }
        if (info_tags == json_list[index]) {
            const QString error = " MusicWidget::updateMusicInfo best_json = info.Tags";
            qDebug() << info_path << error;
            // YT_Func::obj().sendMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
            return { QString(), info_tags };
        }

        MediaEncoder media_encoder;
        QFileInfo file_info(info_path);
        const QString output_path = "./YT_PlayerOutput/" + QString::number(info.Id) + '.' + file_info.suffix();

        int sic = media_encoder.setMediaPath(output_path.toUtf8());
        if (sic < 0) {
            const QString error = " MusicWidget::updateMusicInfo media_encoder setMediaPath error";
            qDebug() << info_path << error;
            // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
            return { QString(), info_tags };
        }

        AudioBuffer audio_buffer(info_path.toUtf8());
        if (audio_buffer.isValid() == false) {
            const QString error = " MusicWidget::updateMusicInfo audio_buffer setMediaPath error";
            qDebug() << info_path << error;
            // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
            return { QString(), info_tags };
        }

        sic = media_encoder.addStream(&audio_buffer);
        if (sic < 0) {
            const QString error = " MusicWidget::updateMusicInfo media_encoder addStream error";
            qDebug() << info_path << error;
            // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
            return { QString(), info_tags };
        }

        sic = YT_Func::MediaEncoder_Write_YT_Data(media_encoder, QJsonDocument(json_list[index]).toJson());
        if (sic < 0) {
            const QString error = " MusicWidget::updateMusicInfo MediaEncoder_Write_YT_Data error";
            qDebug() << info_path << error;
            // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
            return { QString(), info_tags };
        }

        sic = media_encoder.encodeRun();
        if (sic < 0) {
            const QString error = " MusicWidget::updateMusicInfo media_encoder encodeRun error";
            qDebug() << info_path << error;
            // YT_Func::obj().sendErrorMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + error);
            return { QString(), info_tags };
        }

        qDebug() << info_path << " MusicWidget::updateMusicInfo media_encoder encodeRun finished";
        YT_Func::obj().sendMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + " MusicWidget::updateMusicInfo info.Tags update finished", 10000);
        YT_Func::obj().sendMessage(DB::jsTagFirst(info_tags, Media::MetaData_Title).toString() + " output_path is " + output_path, 10000);
        // qDebug().noquote() << "\n=== info_tags ===\n" << QJsonDocument(info_tags).toJson(QJsonDocument::Indented);
        // qDebug().noquote() << "\n=== best_json ===\n" << QJsonDocument(*best_json).toJson(QJsonDocument::Indented);

        return { output_path, json_list[index] };
        });
    watcher->setFuture(future);
    return 0;
}

Q_INVOKABLE int MusicWidget::updateMusicInfo(const QString& path)
{
    return updateMusicInfo(MusicDatabase::obj().getFileInfo_Id(path));
}

Q_INVOKABLE int MusicWidget::updateMusicInfo_Local()
{
    int error_count = 0;
    while (updateTagModel.isEmpty())
    {
        error_count = error_count - (updateMusicInfo_Local(updateTagModel.front()) < 0);
    }
    return error_count;
}

Q_INVOKABLE int MusicWidget::updateMusicInfo_Local(int info_id)
{
    const auto [file_path, target_tags] = updateTagData[info_id]->result();
    const auto playInfoId = getPlayInfoId();
    const auto target_path = MusicDatabase::obj().getFileInfo(info_id, DB_FileInfo::I_FilePath).toString();

    updateTagData[info_id]->deleteLater();
    updateTagData[info_id] = nullptr;
    updateTagModel.removeIf([info_id](int data) {
        return data == info_id;
        });
    emit updateTagModelChanged();

    if (info_id == playInfoId) playMusic_PlayIndex(-1);
    int sic = moveFile(file_path, target_path, true).isEmpty();
    if (info_id == playInfoId) playMusic_PlayIndex(playInfoId);

    if (sic == false) {
        qDebug() << "MusicWidget::updateMusicInfo_Local moveFile failed!\n file_path: " + file_path + " target_path: " + target_path;
        return -1;
    }
    MusicDatabase::obj().setFileInfo(info_id, MusicFileInfo::I_FileTag, QJsonDocument(target_tags).toJson());
    fileModel.updateInfo(info_id);
    return 0;
}

Q_INVOKABLE void MusicWidget::updateMusicInfo_StarLevel(const int data, const QList<int>& info_ids)
{
    for (const auto it : info_ids)
    {
        MusicDatabase::obj().setFileInfo(it, MusicFileInfo::I_StarLevel, data);
        fileModel.updateInfo(it);
    }
}

void MusicWidget::setCurListInfoID(const QString& data)
{
    curListInfoID = data;
    emit curListInfoIDChanged();
    fileModel.setInfoIds(curListInfoID);

    if (pathModel.isSetDataing) return;
    pathModel.isSetDataing = true;
    pathModel.setSrcData({ data });
    pathModel.isSetDataing = false;
}