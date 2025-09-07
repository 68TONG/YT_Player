#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

#include <YT_GeneralH.h>
#include <Music_DataBase.h>
#include <MusicItemModel.h>
#include <MusicListModel.h>

class MusicWidget : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    friend MusicItemModel;
    friend MusicListModel;
    Q_PROPERTY(MusicItemModel *itemModel READ getItemModel CONSTANT)
    Q_PROPERTY(MusicListModel *listModel READ getListModel CONSTANT)
    Q_PROPERTY(QList<QString> curPath READ getCurPath NOTIFY curPathChanged)
    Q_PROPERTY(int curListInfoID READ getCurListInfoID WRITE setCurListInfoID NOTIFY curListInfoIDChanged)
    Q_PROPERTY(int curPlayIndex READ getCurPlayIndex NOTIFY curPlayIndexChanged)
    Q_PROPERTY(QList<QString> itemHeaderModel READ getItemHeaderModel NOTIFY itemHeaderModelChanged)
    Q_PROPERTY(QString metaData_Title MEMBER metaData_Title CONSTANT)
    Q_PROPERTY(QString metaData_One MEMBER metaData_One NOTIFY metaData_OneChanged)
    Q_PROPERTY(QString metaData_Two MEMBER metaData_Two NOTIFY metaData_TwoChanged)
public:
    explicit MusicWidget(QQuickItem *parent = nullptr);

    Q_INVOKABLE void playMusic(int ids_index);
    Q_INVOKABLE void addPlayMusic(const QString &path);
protected:
    MusicItemModel itemModel;
    MusicItemModel *getItemModel() { return &itemModel;}

    MusicListModel listModel;
    MusicListModel *getListModel() { return &listModel;}

    QList<QString> curPath;
    QList<QString> getCurPath() const { return curPath; }
    Q_INVOKABLE void setCurPath(const QString &data);
    Q_INVOKABLE void setCurPath(const QList<QString> &data);
    Q_INVOKABLE void addPath_FilterInfo(const QString &data, const QString &header);
    Q_INVOKABLE void addPath_SearchInfo(const QString &data);
    Q_INVOKABLE QList<QString> getSubPath(const QList<QString> &data) const;

    int curListInfoID = 0;
    int getCurListInfoID() const;
    void setCurListInfoID(const int &data);

    int curPlayIndex = -true;
    int getCurPlayIndex() const;
    void setCurPlayIndex(const QString &path);

    QList<QString> itemHeaderModel = {"Title", "Artist", "Album"};
    QList<QString> getItemHeaderModel() const { return itemHeaderModel; }

    QString metaData_Title = Media::MetaData_Title;
    QString metaData_One = Media::MetaData_Artist;
    QString metaData_Two = Media::MetaData_Album;
signals:
    void curPathChanged();
    void curMusicPathChanged();
    void curMusicPtsChanged();
    void curListInfoIDChanged();
    void curPlayIndexChanged();
    void itemHeaderModelChanged();
    void metaData_OneChanged();
    void metaData_TwoChanged();
};

#endif // MUSICWIDGET_H
