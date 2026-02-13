#include "YT_AudioOutput.h"

#include "MusicDatabase.h"

YT_AudioOutput::YT_AudioOutput(QObject* parent)
    : QMediaPlayer{ parent }
{
    this->setAudioOutput(new QAudioOutput(this));
    this->audioOutput()->setVolume(50);

    connect(this, &YT_AudioOutput::sourceChanged, this, [this](const QUrl& media) {
        // const int info_id = Music_DataBase::getObject().findItemInfo(media.toString());
        int info_id = MusicDatabase::obj().getFileInfo_ID(DB::canonicalPath(media.toString()));
        if (info_id < 0) {
            curMusicInfo = QJsonObject();
        }
        else {
            // curMusicInfo = Music_DataBase::getObject().getItemInfo(info_id).Tags;
            curMusicInfo = MusicDatabase::obj().getFileInfo(info_id).Tags;
        }
        emit curMusicInfoChanged();
        });
    connect(this, &YT_AudioOutput::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == YT_AudioOutput::EndOfMedia) {
            playNextMusic();
        }
        });
    connect(this, &YT_AudioOutput::emitPlayNextMusic, this, [this]() {
        // const auto& ID_List = Music_DataBase::getObject().getPlayListInfo().ID_List;
        // if (ID_List.isEmpty())
        //     return;
        const auto& play_ids = MusicDatabase::obj().playFileInfoIds;
        if (play_ids.isEmpty())
            return;

        playIndex = playIndex + 1;
        if (playIndex < 0 || playIndex >= play_ids.count())
            playIndex = 0;

        // Music_DataBase::getObject().updateItemInfo(ID_List[playIndex]);
        // const auto path = Music_DataBase::getObject().getItemInfo(ID_List[playIndex]).Path;
        MusicDatabase::obj().updateFileInfo(play_ids[playIndex]);
        this->setSource(MusicDatabase::obj().getFileInfo(play_ids[playIndex]).Path);
        this->play();
        });
    connect(this, &YT_AudioOutput::emitPlayPreviousMusic, this, [this]() {
        // const auto& ID_List = Music_DataBase::getObject().getPlayListInfo().ID_List;
        // if (ID_List.isEmpty())
        //     return;
        const auto& play_ids = MusicDatabase::obj().playFileInfoIds;
        if (play_ids.isEmpty())
            return;

        playIndex = playIndex - 1;
        if (playIndex < 0 || playIndex >= play_ids.count())
            playIndex = 0;

        this->setSource(MusicDatabase::obj().getFileInfo(play_ids[playIndex]).Path);
        this->play();
        });
}

YT_AudioOutput& YT_AudioOutput::getObject()
{
    static YT_AudioOutput obj;
    return obj;
}

YT_AudioOutput* YT_AudioOutput::create(QQmlEngine*, QJSEngine*)
{
    QJSEngine::setObjectOwnership(&getObject(), QJSEngine::CppOwnership);
    return &getObject();
}

QList<QVariant> YT_AudioOutput::getCurMusicInfo(const QString& header) const
{
    return DB::jsTagList(curMusicInfo, header);
}

void YT_AudioOutput::playMusic()
{
    this->play();
}

void YT_AudioOutput::pauseMusic()
{
    this->pause();
}

void YT_AudioOutput::playNextMusic()
{
    emit emitPlayNextMusic();
}

void YT_AudioOutput::playPreviousMusic()
{
    emit emitPlayPreviousMusic();
}
