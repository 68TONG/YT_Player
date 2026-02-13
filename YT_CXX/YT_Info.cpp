#include "YT_ConfigureInfo.h"

YT_ConfigureInfo YT_ConfigureInfo::object;

YT_ConfigureInfo::YT_ConfigureInfo(QObject *parent)
    : QObject{parent}
{
    QDir().mkpath("./YT_OutData");
    initConfigureInfo_Data();
}

YT_ConfigureInfo::~YT_ConfigureInfo()
{
    saveConfigureInfo_Data();
}

YT_ConfigureInfo &YT_ConfigureInfo::getObject()
{
    return YT_ConfigureInfo::object;
}

YT_ConfigureInfo *YT_ConfigureInfo::create(QQmlEngine *, QJSEngine *)
{
    QJSEngine::setObjectOwnership(&object, QJSEngine::CppOwnership);
    return &object;
}

QVariant YT_ConfigureInfo::getData(const InfoFlags index) const
{
    return configureInfo_Data[index];
}

void YT_ConfigureInfo::setData(const InfoFlags index, const QVariant &data)
{
    configureInfo_Data[index] = data;
}

void YT_ConfigureInfo::transformWindow(QWindow *window, int type)
{
    type == 0 ? window->startSystemMove() : window->startSystemResize((Qt::Edge)type);
}

void YT_ConfigureInfo::initConfigureInfo_Data()
{
    configureInfo_Data.resize(InfoFlags_End);
    configureInfo_Data[FontColor] = QColor(138, 138, 138);
    configureInfo_Data[ItemColor] = QColor(38, 38, 38);
    configureInfo_Data[ItemFocusColor] = QColor(72, 72, 72);
    configureInfo_Data[BackgroundColor] = QColor(21, 21, 21);

    configureInfo_Data[ItemRadius] = (int)7;
    configureInfo_Data[ItemRadius_Big] = (int)15;
    configureInfo_Data[ItemRadius_Small] = (int)3;

    const auto configureInfo_Json = readJsonObjectFromFile(configureInfo_DataPath);
    if(configureInfo_Json.isEmpty()) return ;

    for(auto it = configureInfo_Json.constBegin(); it != configureInfo_Json.constEnd();it++){
        if(it.key() == "Size"){
            const auto configureInfo_JsonSize = it->toObject();
            for(auto it = configureInfo_JsonSize.constBegin(); it != configureInfo_JsonSize.constEnd();it++){
                QSize value;
                const auto json_value = it->toObject();
                value.setWidth(json_value.value("Width").toInt());
                value.setHeight(json_value.value("Height").toInt());
                configureInfo_Data[InfoFlags_Map.key(it.key())] = value;
            }
        } else if(it.key() == "Color"){
            const auto configureInfo_JsonColor = it->toObject();
            for(auto it = configureInfo_JsonColor.constBegin(); it != configureInfo_JsonColor.constEnd();it++){
                QColor value;
                const auto json_value = it->toObject();
                value.setRed(json_value.value("Red").toInt());
                value.setBlue(json_value.value("Blue").toInt());
                value.setGreen(json_value.value("Green").toInt());
                value.setAlpha(json_value.value("Alpha").toInt());
                configureInfo_Data[InfoFlags_Map.key(it.key())] = value;
            }
        } else {
            configureInfo_Data[InfoFlags_Map.key(it.key())] = it->toVariant();
        }
    }
}

void YT_ConfigureInfo::saveConfigureInfo_Data()
{
    QJsonObject configureInfo_Json;
    QJsonObject configureInfo_JsonSize;
    QJsonObject configureInfo_JsonColor;
    for (int i = 0;i < configureInfo_Data.count();i++) {
        const auto &it = configureInfo_Data[i];
        if (it.typeId() == QMetaType::Type::QColor) {
            QJsonObject json_value;
            const auto value = it.value<QColor>();
            json_value.insert("Red", value.red());
            json_value.insert("Blue", value.blue());
            json_value.insert("Green", value.green());
            json_value.insert("Alpha", value.alpha());
            configureInfo_JsonColor.insert(InfoFlags_Map.value((InfoFlags)i), json_value);
        } else if (it.typeId() == QMetaType::Type::QSize) {
            QJsonObject json_value;
            const auto value = it.value<QSize>();
            json_value.insert("Width", value.width());
            json_value.insert("Height", value.height());
            configureInfo_JsonSize.insert(InfoFlags_Map.value((InfoFlags)i), json_value);
        } else {
            configureInfo_Json.insert(InfoFlags_Map.value((InfoFlags)i), it.toJsonValue());
        }
    }
    configureInfo_Json.insert("Size", configureInfo_JsonSize);
    configureInfo_Json.insert("Color", configureInfo_JsonColor);
    writeJsonObjectToFile(configureInfo_DataPath, configureInfo_Json);
}
