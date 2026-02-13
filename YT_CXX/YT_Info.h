#ifndef YT_CONFIGUREINFO_H
#define YT_CONFIGUREINFO_H

#include <YT_GeneralH.h>

class YT_ConfigureInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    explicit YT_ConfigureInfo(QObject *parent = nullptr);
    ~YT_ConfigureInfo();
public:
    enum InfoFlags
    {
        FontColor,
        ItemColor,
        ItemFocusColor,
        BackgroundColor,

        ItemRadius,
        ItemRadius_Big,
        ItemRadius_Small,

        MusicListInfoID_List,

        InfoFlags_End
    };
    Q_ENUM(InfoFlags)

    static YT_ConfigureInfo &getObject();
    static YT_ConfigureInfo *create(QQmlEngine *, QJSEngine *);

    Q_INVOKABLE QVariant getData(const InfoFlags index) const;
    Q_INVOKABLE void setData(const InfoFlags index, const QVariant &data);

    Q_INVOKABLE void transformWindow(QWindow *window, int type);
private:
    static YT_ConfigureInfo object;

    void initConfigureInfo_Data();
    void saveConfigureInfo_Data();
    QList<QVariant> configureInfo_Data;
    const QString configureInfo_DataPath = "./YT_OutData/ConfigureInfo_Data.json";
    const QMap<InfoFlags, QString> InfoFlags_Map = {
        {FontColor, "FontColor"},
        {ItemColor, "ItemColor"},
        {ItemFocusColor, "ItemFocusColor"},
        {BackgroundColor, "BackgroundColor"},

        {ItemRadius, "ItemRadius"},
        {ItemRadius_Big, "ItemRadius_Big"},
        {ItemRadius_Small, "ItemRadius_Small"},
        {MusicListInfoID_List, "MusicListInfoID_List"}
    };
signals:
};

#endif // YT_CONFIGUREINFO_H
