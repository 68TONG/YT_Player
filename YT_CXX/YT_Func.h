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

        explicit YT_Func(QObject* parent = nullptr);
public:
    static YT_Func& obj();
    static YT_Func& getObject();
    static YT_Func* create(QQmlEngine* qml_engine, QJSEngine*);

    Q_INVOKABLE void transformWindow(QWindow* window, int type);
    Q_INVOKABLE bool intersects(const QRectF& v1, const QRectF& v2) const;
    Q_INVOKABLE void sendMessage(int type, QString message, int duration = 3000);
    Q_INVOKABLE void sendMessage(QString message, int duration = 3000);
    Q_INVOKABLE void sendSuccessMessage(QString message, int duration = 3000);
    Q_INVOKABLE void sendWarningMessage(QString message, int duration = 3000);
    Q_INVOKABLE void sendErrorMessage(QString message, int duration = 3000);

    QString getTH_Character(const QString& data);

    chinese_util::Dict* getChinese_Dict();
    QString getChinesePinYin(const QString& data);
    QString getChineseFirstPinYin(const QString& data);

    QByteArray getNetworkData(const QString& url, QThread* cur_thread = nullptr);
    QNetworkReply* getNetworkReply(const QString& url, QThread* cur_thread = nullptr);
    QNetworkAccessManager* getNetworkManager(QThread* cur_thread = nullptr);

    static bool readXmlNext(QXmlStreamReader& xml_reader, QString name = {}, QMap<QString, QString> attributes = {}, QString text = {});

    static QList<QJsonObject> getTH_Wiki(const QString& url);
    static QList<QJsonObject> getTH_Wiki_Album(const QString& url);
    static QList<QJsonObject> getTH_Wiki_Lyrics(const QString& url);
    static QList<QJsonObject> getMusic_163_Url(const QString& url);
    static QList<QJsonObject> getMusic_163_Search(const QString& search_data);

    static int getBestMusicTag(const QList<QJsonObject>& tags, const QJsonObject& data, const QString& ex_album = {});
    static int MediaEncoder_Write_YT_Data(MediaEncoder& coder, const QByteArray& data);

    static QString outputFileCover(QString file_path);
    Q_INVOKABLE static QVariantMap getFileCoverInfo(QString file_path);
    Q_INVOKABLE static QString getFileCoverPath(QString file_path);
    Q_INVOKABLE static QString getFileCoverPath_InfoId(int info_id);
    static QPointF getFileCoverVisualFocus(const QImage& source_image);
    Q_INVOKABLE static QPointF getFileCoverVisualFocus(const QString& file_path);
    Q_INVOKABLE static QRect getFileCoverVisualRect(double aspect_ratio, const QSize& image_size, const QPointF& focus);
    static QImage outputFileCoverVisualRect(const QImage& source_image, const QRect& crop_rect, QString path_name = "_outputFileCoverVisualRect");
private:
    QNetworkAccessManager network_manager;
    QHash<QThread*, QNetworkAccessManager*> network_manager_list;

    QJsonObject TH_CharacterTable;
    QQmlEngine* qmlEngine = nullptr;
};

#endif // YT_FUNC_H
