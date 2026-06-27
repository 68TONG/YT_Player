#include "YT_AudioOutput.h"

YT_AudioOutput::YT_AudioOutput(QObject* parent)
    : QMediaPlayer{ parent }
{
    this->setAudioOutput(new QAudioOutput(this));
    this->audioOutput()->setVolume(50);

    // connect(this, &YT_AudioOutput::sourceChanged, this, [this](const QUrl& media) {
    //     // const int info_id = Music_DataBase::getObject().findItemInfo(media.toString());
    //     int info_id = MusicDatabase::obj().getFileInfo_ID(media.toString());
    //     if (info_id < 0) {
    //         playInfo = {};
    //     }
    //     else {
    //         // curMusicInfo = Music_DataBase::getObject().getItemInfo(info_id).Tags;
    //         playInfo = MusicDatabase::obj().getFileInfo(info_id);
    //     }
    //     emit playInfoIDChanged();
    //     });
    // connect(this, &YT_AudioOutput::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
    //     if (status == YT_AudioOutput::EndOfMedia) {
    //         playNextMusic();
    //     }
    //     });
    // connect(this, &YT_AudioOutput::emitPlayNextMusic, this, [this]() {
    //     // const auto& ID_List = Music_DataBase::getObject().getPlayListInfo().ID_List;
    //     // if (ID_List.isEmpty())
    //     //     return;
    //     const auto& play_ids = MusicDatabase::obj().playFileInfoIds;
    //     if (play_ids.isEmpty())
    //         return;

    //     playIndex = playIndex + 1;
    //     if (playIndex < 0 || playIndex >= play_ids.count())
    //         playIndex = 0;

    //     // Music_DataBase::getObject().updateItemInfo(ID_List[playIndex]);
    //     // const auto path = Music_DataBase::getObject().getItemInfo(ID_List[playIndex]).Path;
    //     MusicDatabase::obj().updateFileInfo(play_ids[playIndex]);
    //     this->setSource(MusicDatabase::obj().getFileInfo(play_ids[playIndex]).Path);
    //     this->play();
    //     });
    // connect(this, &YT_AudioOutput::emitPlayPreviousMusic, this, [this]() {
    //     // const auto& ID_List = Music_DataBase::getObject().getPlayListInfo().ID_List;
    //     // if (ID_List.isEmpty())
    //     //     return;
    //     const auto& play_ids = MusicDatabase::obj().playFileInfoIds;
    //     if (play_ids.isEmpty())
    //         return;

    //     playIndex = playIndex - 1;
    //     if (playIndex < 0 || playIndex >= play_ids.count())
    //         playIndex = 0;

    //     this->setSource(MusicDatabase::obj().getFileInfo(play_ids[playIndex]).Path);
    //     this->play();
    //     });
}

YT_AudioOutput& YT_AudioOutput::getObject()
{
    static YT_AudioOutput obj;
    return obj;
}

YT_AudioOutput* YT_AudioOutput::create(QQmlEngine* qml_engine, QJSEngine*)
{
    auto object = &getObject();
    Q_ASSERT(qml_engine->thread() == object->thread());
    // if (object->qmlEngine)
    //     Q_ASSERT(object->qmlEngine == qml_engine);
    // else
    //     object->qmlEngine = qml_engine;

    QJSEngine::setObjectOwnership(object, QJSEngine::CppOwnership);
    return object;
}

// Q_INVOKABLE QList<QVariant> YT_AudioOutput::getPlayInfoTags(const QString& header) const
// {
//     return DB::jsTagList(playInfo.Tags, header);
// }

// int YT_AudioOutput::getPlayInfoID() const
// {
//     return playInfo.ID;
// }

void YT_AudioOutput::play()
{
    QMediaPlayer::play();
}

void YT_AudioOutput::pause()
{
    QMediaPlayer::pause();
}