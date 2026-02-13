#include "YT_Info.h"

YT_Info YT_Info::object;

YT_Info::YT_Info(QObject *parent)
    : QObject{parent}
{
    QDir().mkpath("./YT_PlayerData");
    QDir().mkpath("./YT_PlayerCache");
    QDir().mkpath("./YT_PlayerOutput");

    FontColor = QColor(138, 138, 138);
    FontFocusColor = QColor(255, 215, 0);

    ItemColor = QColor(38, 38, 38);
    ItemFocusColor = QColor(72, 72, 72);
    BackgroundColor = QColor(21, 21, 21);

    ItemSize = QSize(180, 30);
    ItemSizeBig = QSize(180, 50);
    ItemSizeSmall = QSize(180, 20);

    Radius = (int)7;
    RadiusBig = (int)15;
    RadiusSmall = (int)3;

    Spacing = (int)7;
    SpacingBig = (int)15;
    SpacingSmall = (int)3;

    Margin = (int)7;
    MarginBig = (int)15;
    MarginSmall = (int)3;

    const auto json = readJsonObjectFromFile(InfoPath);
    if (json.isEmpty())
        return;

    static const auto fromJson = [](void *P, QJsonValue &&J, QMetaType &&T)
    {
        if (T.id() == QMetaType::Int)
        {
            *(int *)P = J.toInt();
        }
        else if (T.id() == QMetaType::QSize)
        {
            auto p = (QSize *)P;
            const auto json_value = J.toObject();
            p->setWidth(json_value.value("Width").toInt());
            p->setHeight(json_value.value("Height").toInt());
        }
        else if (T.id() == QMetaType::QColor)
        {
            auto p = (QColor *)P;
            const auto json_value = J.toObject();
            p->setRed(json_value.value("Red").toInt());
            p->setBlue(json_value.value("Blue").toInt());
            p->setGreen(json_value.value("Green").toInt());
            p->setAlpha(json_value.value("Alpha").toInt());
        }
        else if (T.id() == QMetaType::QVariantList)
        {
            *(QVariantList *)P = J.toArray().toVariantList();
        }
    };

    fromJson(&FontColor, json.value(FontColorName()), FontColorType());
    fromJson(&FontFocusColor, json.value(FontFocusColorName()), FontFocusColorType());

    fromJson(&ItemColor, json.value(ItemColorName()), ItemColorType());
    fromJson(&ItemFocusColor, json.value(ItemFocusColorName()), ItemFocusColorType());
    fromJson(&BackgroundColor, json.value(BackgroundColorName()), BackgroundColorType());

    fromJson(&ItemSize, json.value(ItemSizeName()), ItemSizeType());
    fromJson(&ItemSizeBig, json.value(ItemSizeBigName()), ItemSizeBigType());
    fromJson(&ItemSizeSmall, json.value(ItemSizeSmallName()), ItemSizeSmallType());

    fromJson(&Radius, json.value(RadiusName()), RadiusType());
    fromJson(&RadiusBig, json.value(RadiusBigName()), RadiusBigType());
    fromJson(&RadiusSmall, json.value(RadiusSmallName()), RadiusSmallType());

    fromJson(&Spacing, json.value(SpacingName()), SpacingType());
    fromJson(&SpacingBig, json.value(SpacingBigName()), SpacingBigType());
    fromJson(&SpacingSmall, json.value(SpacingSmallName()), SpacingSmallType());

    fromJson(&Margin, json.value(MarginName()), MarginType());
    fromJson(&MarginBig, json.value(MarginBigName()), MarginBigType());
    fromJson(&MarginSmall, json.value(MarginSmallName()), MarginSmallType());

    fromJson(&MusicListInfo_ID_List, json.value(MusicListInfo_ID_ListName()), MusicListInfo_ID_ListType());
}

YT_Info::~YT_Info()
{
    QJsonObject json;
    static const auto toJson = [](void *P, QMetaType &&T) -> QJsonValue
    {
        if (T.id() == QMetaType::Int)
        {
            return *(int *)P;
        }
        else if (T.id() == QMetaType::QSize)
        {
            auto p = (QSize *)P;
            QJsonObject json_value;
            json_value.insert("Width", p->width());
            json_value.insert("Height", p->height());
            return json_value;
        }
        else if (T.id() == QMetaType::QColor)
        {
            auto p = (QColor *)P;
            QJsonObject json_value;
            json_value.insert("Red", p->red());
            json_value.insert("Blue", p->blue());
            json_value.insert("Green", p->green());
            json_value.insert("Alpha", p->alpha());
            return json_value;
        }
        else if (T.id() == QMetaType::QVariantList)
        {
            QVariant json_value = *(QVariantList *)P;
            return json_value.toJsonValue();
        }
        return {};
    };

    json.insert(FontColorName(), toJson(&FontColor, FontColorType()));
    json.insert(FontFocusColorName(), toJson(&FontFocusColor, FontFocusColorType()));

    json.insert(ItemColorName(), toJson(&ItemColor, ItemColorType()));
    json.insert(ItemFocusColorName(), toJson(&ItemFocusColor, ItemFocusColorType()));
    json.insert(BackgroundColorName(), toJson(&BackgroundColor, BackgroundColorType()));

    json.insert(ItemSizeName(), toJson(&ItemSize, ItemSizeType()));
    json.insert(ItemSizeBigName(), toJson(&ItemSizeBig, ItemSizeBigType()));
    json.insert(ItemSizeSmallName(), toJson(&ItemSizeSmall, ItemSizeSmallType()));

    json.insert(RadiusName(), toJson(&Radius, RadiusType()));
    json.insert(RadiusBigName(), toJson(&RadiusBig, RadiusBigType()));
    json.insert(RadiusSmallName(), toJson(&RadiusSmall, RadiusSmallType()));

    json.insert(SpacingName(), toJson(&Spacing, SpacingType()));
    json.insert(SpacingBigName(), toJson(&SpacingBig, SpacingBigType()));
    json.insert(SpacingSmallName(), toJson(&SpacingSmall, SpacingSmallType()));

    json.insert(MarginName(), toJson(&Margin, MarginType()));
    json.insert(MarginBigName(), toJson(&MarginBig, MarginBigType()));
    json.insert(MarginSmallName(), toJson(&MarginSmall, MarginSmallType()));

    json.insert(MusicListInfo_ID_ListName(), toJson(&MusicListInfo_ID_List, MusicListInfo_ID_ListType()));

    writeJsonObjectToFile(InfoPath, json);
}

YT_Info &YT_Info::getObject()
{
    return object;
}

YT_Info *YT_Info::create(QQmlEngine *, QJSEngine *)
{
    QJSEngine::setObjectOwnership(&object, QJSEngine::CppOwnership);
    return &object;
}