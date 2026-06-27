#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

#include <YT_GeneralH.h>
#include <MusicFileModel.h>
#include <MusicListModel.h>
#include <YT_PathWidgetModel.h>
#include <YT_SearchWidgetModel.h>
#include <tuple>

class MusicWidget : public QQuickItem
{
    Q_OBJECT
        QML_ELEMENT
public:
    friend MusicFileModel;
    friend MusicListModel;
public:
    explicit MusicWidget(QQuickItem* parent = nullptr);

    Q_INVOKABLE void playMusic(int data);
    Q_INVOKABLE void playMusic_Path(QString data);
    Q_INVOKABLE void playMusic_InfoId(int data);
    Q_INVOKABLE void playMusic_PlayIndex(int data);
    Q_INVOKABLE void playNextMusic();
    Q_INVOKABLE void playPreviousMusic();
    Q_INVOKABLE void addPlayMusic(const QList<int>& info_ids);
    Q_INVOKABLE void addPlayMusic_Path(const QList<QString>& path_list);

    Q_INVOKABLE QString getMusicInfo_CoverPath(int info_id);

    Q_INVOKABLE int updateMusicInfo(int info_id);
    Q_INVOKABLE int updateMusicInfo(const QString& path);
    Q_INVOKABLE int updateMusicInfo_Local();
    Q_INVOKABLE int updateMusicInfo_Local(int info_id);
    Q_INVOKABLE void updateMusicInfo_StarLevel(const int data, const QList<int>& info_ids);
protected:
    Q_PROPERTY(MusicFileModel* fileModel READ getFileModel CONSTANT)
        MusicFileModel fileModel;
    MusicFileModel* getFileModel() { return &fileModel; }

    Q_PROPERTY(MusicListModel* listModel READ getListModel CONSTANT)
        MusicListModel listModel;
    MusicListModel* getListModel() { return &listModel; }

    Q_PROPERTY(YT_PathWidgetModel* pathModel READ getPathModel CONSTANT)
        YT_PathWidgetModel pathModel;
    YT_PathWidgetModel* getPathModel() { return &pathModel; };

    Q_PROPERTY(YT_SearchWidgetModel* searchModel READ getSearchModel CONSTANT)
        YT_SearchWidgetModel searchModel;
    YT_SearchWidgetModel* getSearchModel() { return &searchModel; };

    QHash<int, QFutureWatcher<std::tuple<QString, QJsonObject>>*> updateTagData = {};
    Q_PROPERTY(QList<int> updateTagModel READ getUpdateTagModel NOTIFY updateTagModelChanged)
        QList<int> updateTagModel = {};
    QList<int> getUpdateTagModel() const { return updateTagModel; };
    Q_SIGNAL void updateTagModelChanged();

    Q_PROPERTY(QList<int> playModel READ getPlayModel NOTIFY playModelChanged)
        QList<int> playModel;
    QList<int> getPlayModel() { return playModel; }
    Q_SIGNAL void playModelChanged();

    Q_PROPERTY(int playIndex READ getPlayIndex NOTIFY playIndexChanged)
        int playIndex = -1;
    int getPlayIndex() const { return playIndex; };
    Q_SIGNAL void playIndexChanged();

    Q_PROPERTY(int playInfoId READ getPlayInfoId NOTIFY playInfoIdChanged)
        int getPlayInfoId() const { return playModel.value(playIndex, -1); };
    Q_SIGNAL void playInfoIdChanged();

    Q_PROPERTY(QString playMode MEMBER playMode NOTIFY playModeChanged)
        QString playMode = "qrc:/Ui_Image/shuffle.png";
    Q_SIGNAL void playModeChanged();

    Q_PROPERTY(QString curListInfoID READ getCurListInfoID WRITE setCurListInfoID NOTIFY curListInfoIDChanged)
        QString curListInfoID;
    QString getCurListInfoID() const { return curListInfoID; };
    void setCurListInfoID(const QString& data);
    Q_SIGNAL void curListInfoIDChanged();
};

#endif // MUSICWIDGET_H
