#ifndef YT_INFO_H
#define YT_INFO_H

#include <YT_GeneralH.h>

#define YT_PROPERTY_AUTO(T, P)                  \
    Q_PROPERTY(T P MEMBER P NOTIFY P##Changed)  \
    T P;                                        \
    Q_SIGNAL void P##Changed();                 \
    static const char *P##Name() { return #P; } \
    static QMetaType P##Type() { return QMetaType::fromType<T>(); }

class YT_Info : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    explicit YT_Info(QObject *parent = nullptr);
    ~YT_Info();

public:
    static YT_Info &getObject();
    static YT_Info *create(QQmlEngine *, QJSEngine *);
    
    YT_PROPERTY_AUTO(QColor, FontColor);
    YT_PROPERTY_AUTO(QColor, FontFocusColor);

    YT_PROPERTY_AUTO(QColor, ItemColor);
    YT_PROPERTY_AUTO(QColor, ItemFocusColor);
    YT_PROPERTY_AUTO(QColor, BackgroundColor);

    YT_PROPERTY_AUTO(QSize, ItemSize);
    YT_PROPERTY_AUTO(QSize, ItemSizeBig);
    YT_PROPERTY_AUTO(QSize, ItemSizeSmall);

    YT_PROPERTY_AUTO(int, Radius);
    YT_PROPERTY_AUTO(int, RadiusBig);
    YT_PROPERTY_AUTO(int, RadiusSmall);

    YT_PROPERTY_AUTO(int, Spacing);
    YT_PROPERTY_AUTO(int, SpacingBig);
    YT_PROPERTY_AUTO(int, SpacingSmall);

    YT_PROPERTY_AUTO(int, Margin);
    YT_PROPERTY_AUTO(int, MarginBig);
    YT_PROPERTY_AUTO(int, MarginSmall);

    YT_PROPERTY_AUTO(QVariantList, MusicListInfo_ID_List);

private:
    static YT_Info object;
    const QString InfoPath = "./YT_PlayerData/Info.json";
};

#endif // YT_INFO_H
