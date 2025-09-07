#ifndef YT_AUDIOOUTPUT_H
#define YT_AUDIOOUTPUT_H

#include <YT_ConfigureInfo.h>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <Music_DataBase.h>

class YT_AudioOutput : public QMediaPlayer
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QString playModel MEMBER playModel NOTIFY playModelChanged)

    explicit YT_AudioOutput(QObject *parent = nullptr);
public:
    static YT_AudioOutput &getObject();
    static YT_AudioOutput *create(QQmlEngine *, QJSEngine *);
public:
    QJsonObject curMusicInfo;
    Q_INVOKABLE QList<QVariant> getCurMusicInfo(const QString &header) const;

    QString playModel;
    int playIndex = -1;
    Q_INVOKABLE void playNextMusic();
    Q_INVOKABLE void playPreviousMusic();
signals:
    void curMusicInfoChanged();
    void playModelChanged();
};

#endif // YT_AUDIOOUTPUT_H
