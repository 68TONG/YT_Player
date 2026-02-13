#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

#include <YT_GeneralH.h>
#include <MusicItemModel.h>
#include <MusicListModel.h>

class MusicWidget : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    friend MusicItemModel;
    friend MusicListModel;

public:
    explicit MusicWidget(QQuickItem *parent = nullptr);

    Q_INVOKABLE void playMusic(int ids_index);
    Q_INVOKABLE void addPlayMusic(const QString &path);
    Q_INVOKABLE void addPlayMusic(const QList<QString> &path);

protected:
    Q_PROPERTY(MusicItemModel *itemModel READ getItemModel CONSTANT)
    MusicItemModel itemModel;
    MusicItemModel *getItemModel() { return &itemModel; }

    Q_PROPERTY(MusicListModel *listModel READ getListModel CONSTANT)
    MusicListModel listModel;
    MusicListModel *getListModel() { return &listModel; }

    Q_PROPERTY(QList<QString> curPath READ getCurPath NOTIFY curPathChanged)
    QList<QString> curPath;
    QList<QString> getCurPath() const { return curPath; }
    Q_INVOKABLE void setCurPath(const QString &data);
    Q_INVOKABLE void setCurPath(const QList<QString> &data);
    Q_INVOKABLE void addCurPath(const QString &data);
    Q_INVOKABLE QList<QString> getSubPath(const QList<QString> &data) const;
    Q_SIGNAL void curPathChanged();

    Q_PROPERTY(QString curListInfoID READ getCurListInfoID WRITE setCurListInfoID NOTIFY curListInfoIDChanged)
    QString curListInfoID;
    QString getCurListInfoID() const;
    void setCurListInfoID(const QString &data);
    Q_SIGNAL void curListInfoIDChanged();

    Q_PROPERTY(int curPlayIndex READ getCurPlayIndex NOTIFY curPlayIndexChanged)
    int curPlayIndex = -true;
    int getCurPlayIndex() const;
    void setCurPlayIndex(const QString &data);
    Q_SIGNAL void curPlayIndexChanged();
signals:
};

#endif // MUSICWIDGET_H
