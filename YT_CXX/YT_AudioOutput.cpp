#include "YT_AudioOutput.h"

YT_AudioOutput::YT_AudioOutput(QObject *parent)
    : QMediaPlayer{parent}
{
    this->setAudioOutput(new QAudioOutput(this));
    this->audioOutput()->setVolume(50);
    connect(this, &YT_AudioOutput::sourceChanged, this, [this](const QUrl &media){
        const int info_id = Music_DataBase::getObject().findItemInfo(media.toString());
        if (info_id < 0) {
            curMusicInfo = QJsonObject();
        } else {
            curMusicInfo = Music_DataBase::getObject().getItemInfo(info_id).musicData;
        }
        emit curMusicInfoChanged();
    });
    connect(this, &YT_AudioOutput::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status){
        if (status == YT_AudioOutput::EndOfMedia) {
            playNextMusic();
        }
    });
}

YT_AudioOutput &YT_AudioOutput::getObject()
{
    static YT_AudioOutput obj;
    return obj;
}

YT_AudioOutput *YT_AudioOutput::create(QQmlEngine *, QJSEngine *)
{
    QJSEngine::setObjectOwnership(&getObject(), QJSEngine::CppOwnership);
    return &getObject();
}

QList<QVariant> YT_AudioOutput::getCurMusicInfo(const QString &header) const
{
    return curMusicInfo[header].toArray().toVariantList();
}

void YT_AudioOutput::playNextMusic()
{
    const auto &ID_List = Music_DataBase::getObject().getPlayListInfo().ID_List;
    if (ID_List.isEmpty()) return ;

    playIndex = playIndex + 1;
    if (playIndex < 0 || playIndex >= ID_List.count()) playIndex = 0;

    this->setSource(Music_DataBase::getObject().getItemInfo(ID_List[playIndex]).Path);
    this->play();
}

void YT_AudioOutput::playPreviousMusic()
{
    const auto &ID_List = Music_DataBase::getObject().getPlayListInfo().ID_List;
    if (ID_List.isEmpty()) return ;

    playIndex = playIndex - 1;
    if (playIndex < 0 || playIndex >= ID_List.count()) playIndex = 0;

    this->setSource(Music_DataBase::getObject().getItemInfo(ID_List[playIndex]).Path);
    this->play();
}
