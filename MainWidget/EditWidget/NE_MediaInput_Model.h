#ifndef NE_MEDIAINPUT_MODEL_H
#define NE_MEDIAINPUT_MODEL_H

#include <YT_Info.h>
#include <MediaDecoder.h>
#include <NE_Data.h>
#include <NE_MediaData_Model.h>
#include <YT_DataBase.h>
#include <QtConcurrent>

class NE_MediaInput_Model : public NE_MediaData_Model
{
    Q_OBJECT
        QML_ELEMENT
public:
    enum DataIndex
    {
        IndexTitle,
        IndexData,
        IndexType,
        IndexAutoConnect,

        IndexEnd
    };
    Q_ENUM(DataIndex)
        explicit NE_MediaInput_Model(QObject* parent = nullptr);

    Q_PROPERTY(QQueue<bool> isInputing READ getIsInputing NOTIFY isInputingChanged)
        QQueue<bool> isInputing;
    QQueue<bool> getIsInputing() const { return isInputing; }
    Q_SIGNAL void isInputingChanged();

    Q_PROPERTY(QString inputUrl MEMBER inputUrl NOTIFY inputUrlChanged)
        QString inputUrl;
    Q_SIGNAL void inputUrlChanged();

    const QString TH_Wiki_Lyrics = "东方维基_歌词";
    const QString TH_Wiki_Album = "东方维基_专辑";
    const QString TH_Project = "东方Project";
    const QString Music_163 = "网易云";
    Q_PROPERTY(QList<QString> matchUrl MEMBER matchUrl NOTIFY matchUrlChanged)
        QList<QString> matchUrl;
    Q_SIGNAL void matchUrlChanged();
    Q_INVOKABLE QList<QString> getMatchUrlModel() const
    {
        return { TH_Wiki_Lyrics, TH_Wiki_Album, TH_Project, Music_163 };
    }

    Q_PROPERTY(QString additionalData MEMBER additionalData NOTIFY additionalDataChanged)
        QString additionalData;
    Q_SIGNAL void additionalDataChanged();

    Q_PROPERTY(QList<QVariantList> dataModel READ getDataModel NOTIFY dataModelChanged)
        QList<QVariantList> dataModel;
    QList<QVariantList> getDataModel() const { return dataModel; }
    Q_SIGNAL void dataModelChanged();
    void addData(QString title, QVariant data, int type, bool is_connect = false);

protected:
    MediaDecoder media_decoder;
    YT_ItemInfo::YT_InfoTag media_tags;
    QFutureWatcher<QList<QJsonObject>>* getFutureWatcher();
    void inputAutoMatch();
    void inputLocalFile(const QString& url);
    void inputTH_Wiki(const QString& url);
    void inputMusic_163_Url(const QString& url);
    void inputMusic_163_Search(const QString& search_data);
};

#endif // NE_MEDIAINPUT_MODEL_H