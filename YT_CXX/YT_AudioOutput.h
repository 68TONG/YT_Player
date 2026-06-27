#ifndef YT_AUDIOOUTPUT_H
#define YT_AUDIOOUTPUT_H

#include <YT_Info.h>
#include <MusicDatabase.h>
#include <QMediaPlayer>
#include <QAudioOutput>

class YT_AudioOutput : public QMediaPlayer
{
    Q_OBJECT
        QML_ELEMENT
        QML_SINGLETON

        explicit YT_AudioOutput(QObject* parent = nullptr);
public:
    static YT_AudioOutput& getObject();
    static YT_AudioOutput* create(QQmlEngine* qml_engine, QJSEngine*);
public:
    // MusicFileInfo playInfo;
    // Q_PROPERTY(int playInfoID READ getPlayInfoID NOTIFY playInfoIDChanged)
    //     int getPlayInfoID() const;
    // Q_SIGNAL void playInfoIDChanged();
    // Q_INVOKABLE QList<QVariant> getPlayInfoTags(const QString& header) const;

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
};

#endif // YT_AUDIOOUTPUT_H
