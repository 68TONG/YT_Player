#ifndef YT_FUNC_H
#define YT_FUNC_H

#include <YT_GeneralH.h>
#include <QXmlStreamReader>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <MediaEncoder.h>

#include <dict.h>
#include <pinyin.h>

class YT_Func : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    explicit YT_Func(QObject *parent = nullptr);
public:
    static YT_Func &obj();
    static YT_Func &getObject();
    static YT_Func *create(QQmlEngine *, QJSEngine *);

    Q_INVOKABLE void transformWindow(QWindow *window, int type);
    Q_INVOKABLE bool intersects(const QRectF &v1, const QRectF &v2) const;

    QString getTH_Character(const QString &data);

    chinese_util::Dict *getChinese_Dict();
    QString getChinesePinYin(const QString &data);
    QString getChineseFirstPinYin(const QString &data);

    QByteArray getNetworkData(const QString &url, QThread *cur_thread = nullptr);
    QNetworkReply *getNetworkReply(const QString &url, QThread *cur_thread = nullptr);
    QNetworkAccessManager *getNetworkManager(QThread *cur_thread = nullptr);

    static bool readXmlNext(QXmlStreamReader &xml_reader, QString name = {}, QMap<QString, QString> attributes = {}, QString text = {});

    static QList<QJsonObject> getTH_Wiki(const QString &url);
    static QList<QJsonObject> getTH_Wiki_Album(const QString &url);
    static QList<QJsonObject> getTH_Wiki_Lyrics(const QString &url);
    static QList<QJsonObject> getMusic_163_Url(const QString &url);
    static QList<QJsonObject> getMusic_163_Search(const QString &search_data);

    static int getBestMusicTag(const QList<QJsonObject> &tags, const QJsonObject &data, const QString &ex_album = {});
    static void MediaEncoder_Write_YT_Data(MediaEncoder &coder, const QString &data);

private:
    QNetworkAccessManager network_manager;
    QHash<QThread *, QNetworkAccessManager *> network_manager_list;

    QJsonObject TH_CharacterTable;
    chinese_util::Dict *Chinese_Dict = nullptr;
};

#endif // YT_FUNC_H
