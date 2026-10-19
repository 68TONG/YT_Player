#include "YT_Func.h"

#include "YT_DataBase.h"
#include "DB_DatabaseInstance.h"
#include "QImageReader"

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QRect>

#include <opencv2/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cmath>
#include <mutex>

YT_Func::YT_Func(QObject* parent)
    : QObject{ parent }
{
}

YT_Func& YT_Func::obj()
{
    return getObject();
}

YT_Func& YT_Func::getObject()
{
    static YT_Func object;
    return object;
}

YT_Func* YT_Func::create(QQmlEngine* qml_engine, QJSEngine*)
{
    auto object = &getObject();
    Q_ASSERT(qml_engine->thread() == object->thread());
    if (object->qmlEngine)
        Q_ASSERT(object->qmlEngine == qml_engine);
    else
        object->qmlEngine = qml_engine;

    QJSEngine::setObjectOwnership(object, QJSEngine::CppOwnership);
    return object;
}

void YT_Func::transformWindow(QWindow* window, int type)
{
    type == 0 ? window->startSystemMove() : window->startSystemResize((Qt::Edge)type);
}

bool YT_Func::intersects(const QRectF& v1, const QRectF& v2) const
{
    return v1.intersects(v2);
}

Q_INVOKABLE void YT_Func::sendMessage(int type, QString message, int duration)
{
    auto YT = qmlEngine->singletonInstance<QObject*>("YT_Player", "YT");
    auto mainMessage = YT->property("mainMessage").value<QObject*>();
    QVariantMap data = { { "message", message },{ "type", type },{ "duration", duration } };
    QMetaObject::invokeMethod(mainMessage, "open", QVariant(data));
}

Q_INVOKABLE void YT_Func::sendMessage(QString message, int duration)
{
    sendMessage(3, message, duration);
}

Q_INVOKABLE void YT_Func::sendSuccessMessage(QString message, int duration)
{
    sendMessage(1, message, duration);
}

Q_INVOKABLE void YT_Func::sendWarningMessage(QString message, int duration)
{
    sendMessage(2, message, duration);
}

Q_INVOKABLE void YT_Func::sendErrorMessage(QString message, int duration)
{
    sendMessage(4, message, duration);
}

QString YT_Func::getTH_Character(const QString& data)
{
    static const auto Str_Similarity = [](const QString& s1, const QString& s2) -> double
        {
            const int len1 = s1.size(), len2 = s2.size();
            std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

            for (int i = 0; i <= len1; ++i)
                d[i][0] = i;
            for (int j = 0; j <= len2; ++j)
                d[0][j] = j;

            for (int i = 1; i <= len1; ++i)
            {
                for (int j = 1; j <= len2; ++j)
                {
                    int cost = s1[i - 1] == s2[j - 1] ? 0 : 1;
                    d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost });
                }
            }

            // 计算字符串相似度
            int dist = d[len1][len2];
            int maxLen = std::max(s1.size(), s2.size());
            return maxLen == 0 ? 1.0 : (1.0 - static_cast<double>(dist) / maxLen);
        };

    if (TH_CharacterTable.isEmpty())
    {
        TH_CharacterTable = readJsonObjectFromFile("./YT_PlayerData/TH_CharacterTable.json");
    }
    for (const auto it : TH_CharacterTable)
    {
        const auto info_list = it.toArray();
        for (const auto info_it : info_list)
        {
            const auto info = info_it.toObject();
            if (Str_Similarity(info["Japanese"].toString(), data) >= 0.80)
                return info["Data"].toString();
            if (Str_Similarity(info["Chinese"].toString(), data) >= 0.80)
                return info["Data"].toString();
            if (Str_Similarity(info["English"].toString(), data) >= 0.80)
                return info["Data"].toString();
        }
    }
    return QString();
}

chinese_util::Dict* YT_Func::getChinese_Dict()
{
    static chinese_util::Dict* const chineseDict = []() {
        auto dict = new chinese_util::Dict();
        dict->LoadPinyinData("./YT_PlayerData/chinese_util/pinyinData.json");
        dict->LoadCharacterData("./YT_PlayerData/chinese_util/charsData.json");
        return dict;
        }();
    return chineseDict;
}

QString YT_Func::getChinesePinYin(const QString& data)
{
    auto dict = getChinese_Dict();
    chinese_util::PinyinResultString result;
    chinese_util::Pinyin::Convert(result, dict, data.toStdString(), chinese_util::PINYIN, true, "");
    return QString::fromStdString(result.pinyin->front()).remove(" ");
    // return QString::fromStdString(accumulate(result.pinyin->begin(), result.pinyin->end(), std::string{}));
}

QString YT_Func::getChineseFirstPinYin(const QString& data)
{
    auto dict = getChinese_Dict();
    chinese_util::PinyinResultString result;
    chinese_util::Pinyin::Convert(result, dict, data.toStdString(), chinese_util::PINYIN_FIRST, true, "");
    return QString::fromStdString(result.pinyin_first->front()).toUpper();
    // return QString::fromStdString(accumulate(result.pinyin_first->begin(), result.pinyin_first->end(), std::string{}));
}

QByteArray YT_Func::getNetworkData(const QString& url, QThread* cur_thread)
{
    QByteArray data;
    auto reply = getNetworkReply(url, cur_thread);
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "YT_Func::getNetworkData error: " << reply->errorString();
    }
    else if (reply->isRunning())
    {
        qDebug() << "YT_Func::getNetworkData isFinished() == false";
    }
    else
    {
        data = reply->readAll();
    }

    reply->deleteLater();
    return data;
}

QNetworkReply* YT_Func::getNetworkReply(const QString& url, QThread* cur_thread)
{
    QNetworkRequest request(url);
    auto reply = getNetworkManager(cur_thread)->get(request);

    QEventLoop loop;
    // connect(qApp, &QCoreApplication::aboutToQuit, &loop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    return reply;
}

QNetworkAccessManager* YT_Func::getNetworkManager(QThread* cur_thread)
{
    if (cur_thread == nullptr)
        return &network_manager;
    if (network_manager.thread() == cur_thread)
        return &network_manager;
    if (network_manager_list[cur_thread] == nullptr)
        network_manager_list[cur_thread] = new QNetworkAccessManager();
    return network_manager_list[cur_thread];
}

bool YT_Func::readXmlNext(QXmlStreamReader& xml_reader, QString name, QMap<QString, QString> attributes, QString text)
{
    bool is_skip = false;
    while (xml_reader.atEnd() == false)
    {
        if (is_skip == false)
        {
            if (xml_reader.readNext() != QXmlStreamReader::StartElement)
            {
                continue;
            }
        }

        is_skip = false;
        if (name.isEmpty() == false && xml_reader.name().toString() != name)
            continue;

        bool is = true;
        auto xml_attributes = xml_reader.attributes();
        for (auto it = attributes.begin(); it != attributes.end(); it++)
        {
            if (xml_attributes.hasAttribute(it.key()) && it.value().isEmpty())
                continue;
            if (xml_attributes.value(it.key()).toString() == it.value())
                continue;
            is = false;
            break;
        }

        if (text.isEmpty() == false)
        {
            is_skip = xml_reader.readNext() == QXmlStreamReader::StartElement;
            if (xml_reader.tokenType() != QXmlStreamReader::Characters || xml_reader.isWhitespace())
                continue;
            if (text != "true" && text != xml_reader.text())
                continue;
        }

        if (is)
            return true;
    }
    return false;
}

QList<QJsonObject> YT_Func::getTH_Wiki(const QString& url)
{
    if (QUrl::fromPercentEncoding(url.toUtf8()).contains("歌词:"))
        return getTH_Wiki_Lyrics(url);
    return getTH_Wiki_Album(url);
}

QList<QJsonObject> YT_Func::getTH_Wiki_Album(const QString& url)
{
    QList<QJsonObject> ret_tags;
    QXmlStreamReader xml_reader(YT_Func::getObject().getNetworkData(url, QThread::currentThread()));

    QString Title, Album, YT_Cover_Url;
    QJsonArray* target_vec = nullptr;
    QJsonArray Societies, Artist, Original_Song, Character_Song, YT_Lyrics_Url;

    int sic = YT_Func::readXmlNext(xml_reader, "td", { { "class", "label" } }, "名称");
    if (sic == false)
    {
        qDebug() << url << " YT_Func::getTH_Wiki_Album error";
        return {};
    }
    sic = YT_Func::readXmlNext(xml_reader, "td", {}, "true");
    Album = xml_reader.text().toString().trimmed();

    sic = YT_Func::readXmlNext(xml_reader, "td", { { "class", "label" } }, "制作方");
    sic = YT_Func::readXmlNext(xml_reader, "td");
    while (xml_reader.atEnd() == false)
    {
        const auto token = xml_reader.readNext();
        if (token == QXmlStreamReader::EndElement && xml_reader.name() == "td")
            break;
        if (token != QXmlStreamReader::StartElement || xml_reader.name() != "a")
            continue;
        xml_reader.readNext();
        Societies.append(xml_reader.text().toString().trimmed());
    }

    sic = YT_Func::readXmlNext(xml_reader, "a", { { "class", "image" },{ "title", "封面图片" } });
    YT_Cover_Url = "https://thwiki.cc" + xml_reader.attributes().value("href").toString();

    sic = YT_Func::readXmlNext(xml_reader, "table", { { "class", "wikitable musicTable" } });
    if (sic == false)
    {
        qDebug() << url << " YT_Func::getTH_Wiki_Album wikitable musicTable error";
        return {};
    }

    const auto inputRetTag = [&]()
        {
            if (Title.isEmpty() == false)
            {
                YT_Lyrics_Url.append(QString("https://lyrics.thwiki.cc/%1.1.ja.lrc").arg(Title));
                YT_Lyrics_Url.append(QString("https://lyrics.thwiki.cc/%1.1.zh.lrc").arg(Title));

                for (const auto& it : Original_Song)
                {
                    auto data = YT_Func::getObject().getTH_Character(it.toString());
                    if (data.isEmpty() == false && Character_Song.contains(data) == false)
                        Character_Song += data;
                }

                QJsonObject value;
                value["Title"] = Title;
                value["Album"] = Album;
                value["Artist"] = Artist;
                value["Societies"] = Societies;
                value["Original_Song"] = Original_Song;
                value["Character_Song"] = Character_Song;
                value["YT_Lyrics_Url"] = YT_Lyrics_Url;
                value["YT_Cover_Url"] = YT_Cover_Url;
                ret_tags.append(value);
            }

            Title = {};
            Artist = {};
            Original_Song = {};
            Character_Song = {};
            YT_Lyrics_Url = {};
        };

    while (xml_reader.atEnd() == false)
    {
        xml_reader.readNext();
        const auto name = xml_reader.name();
        const auto attributes = xml_reader.attributes();

        if (xml_reader.tokenType() == QXmlStreamReader::EndElement && name == "tbody")
        {
            break;
        }
        if (xml_reader.tokenType() != QXmlStreamReader::StartElement)
        {
            continue;
        }
        if (name != "td")
        {
            continue;
        }

        if (attributes.hasAttribute("id") && attributes.value("class").toString().contains("info"))
        {
            inputRetTag();
            YT_Func::readXmlNext(xml_reader, "td", { { "class", "title" } });
            xml_reader.readNext();

            if (xml_reader.tokenType() == QXmlStreamReader::Characters)
            {
                Title = xml_reader.text().toString().trimmed();
            }
            else if (xml_reader.name() == "a")
            {
                xml_reader.readNext();
                Title = xml_reader.text().toString().trimmed();
            }
            else
            {
                YT_Func::readXmlNext(xml_reader, "a", { { "href", "" } }, "true");
                Title = xml_reader.text().toString().trimmed();
            }
            continue;
        }

        xml_reader.readNext();
        if (attributes.value("class") == "label" && xml_reader.text() == "演唱")
        {
            if (YT_Func::readXmlNext(xml_reader, "td", { { "class", "text" } }) == false)
                break;
            while (xml_reader.atEnd() == false)
            {
                xml_reader.readNext();
                const auto name = xml_reader.name();
                const auto attributes = xml_reader.attributes();
                if (xml_reader.tokenType() == QXmlStreamReader::EndElement && name == "td")
                    break;
                if (xml_reader.tokenType() != QXmlStreamReader::StartElement)
                    continue;
                if (name != "a" || attributes.hasAttribute("title") == false)
                    continue;

                xml_reader.readNext();
                auto data = xml_reader.text().toString().trimmed();
                if (data.isEmpty())
                    continue;
                Artist.append(data);
            }
        }
        else if (attributes.value("class") == "label" && xml_reader.text() == "原曲")
        {
            if (YT_Func::readXmlNext(xml_reader, "td", { { "class", "text" } }) == false)
                break;
            while (xml_reader.atEnd() == false)
            {
                xml_reader.readNext();
                const auto name = xml_reader.name();
                const auto attributes = xml_reader.attributes();
                if (xml_reader.tokenType() == QXmlStreamReader::EndElement && name == "td")
                    break;
                else if (xml_reader.tokenType() != QXmlStreamReader::StartElement)
                    continue;

                if (name == "a" && target_vec)
                {
                    xml_reader.readNext();
                    auto target_val = xml_reader.text().toString().trimmed();
                    if (target_val.isEmpty())
                        continue;
                    target_vec->append(target_val);
                }
                else if (attributes.value("class") == "ogmusic")
                {
                    target_vec = &Original_Song;
                }
                else if (attributes.value("class") == "source")
                {
                    target_vec = nullptr;
                }
            }
        }
    }
    inputRetTag();
    return ret_tags;
}

QList<QJsonObject> YT_Func::getTH_Wiki_Lyrics(const QString& url)
{
    QList<QJsonObject> ret_tags;
    QXmlStreamReader xml_reader(YT_Func::getObject().getNetworkData(url, QThread::currentThread()));

    const auto analysisTagFunc = [&ret_tags, &xml_reader]()
        {
            QString Title, Album, YT_Cover_Url;
            QJsonArray* target_vec = nullptr;
            QJsonArray Societies, Artist, Original_Song, Character_Song, YT_Lyrics_Url;

            // YT_Func::readXmlNext(xml_reader, "a", {{"class", "image"}});
            // QXmlStreamReader cover_xml_reader(YT_Func::getObject().getNetworkData("https://thwiki.cc" + xml_reader.attributes().value("href").toString(), QThread::currentThread()));
            // if ((cover_xml_reader.atEnd() == false) && (cover_xml_reader.hasError() == false))
            // {
            //     YT_Func::readXmlNext(cover_xml_reader, "a", {{"class", "internal"}});
            //     YT_Cover_Url = cover_xml_reader.attributes().value("href").toString();
            // }

            YT_Func::readXmlNext(xml_reader, "a", { { "class", "image" } });
            YT_Cover_Url = "https://thwiki.cc" + xml_reader.attributes().value("href").toString();

            YT_Func::readXmlNext(xml_reader, "a", {});
            xml_reader.readNext();
            Title = xml_reader.text().toString().trimmed();

            YT_Func::readXmlNext(xml_reader, "a", {});
            xml_reader.readNext();
            Album = xml_reader.text().toString().trimmed();

            while ((xml_reader.atEnd() == false) && (xml_reader.hasError() == false))
            {
                const auto token = xml_reader.readNext();
                if (token == QXmlStreamReader::StartElement)
                {
                    if (xml_reader.name().toString() != "a")
                        continue;
                    if (target_vec == nullptr)
                        continue;

                    xml_reader.readNext();
                    auto target_val = xml_reader.text().toString().trimmed();
                    if (target_val.isEmpty())
                        continue;
                    target_vec->append(target_val);
                }
                else if (token == QXmlStreamReader::EndElement)
                {
                    if (xml_reader.name().toString() == "dd")
                        break;
                }
                else if (token == QXmlStreamReader::Characters && xml_reader.isWhitespace() == false)
                {
                    const auto data = xml_reader.text().toString();
                    if (data.contains("社团"))
                    {
                        target_vec = &Societies;
                    }
                    else if (data.contains("演唱") || data.contains("翻唱"))
                    {
                        target_vec = &Artist;
                    }
                    else if (data.contains("原曲"))
                    {
                        target_vec = &Original_Song;
                    }
                    else if (data.contains(":") || data.contains("\uFF1A"))
                    {
                        target_vec = nullptr;
                    }
                }
            }

            for (const auto& it : Original_Song)
            {
                auto data = YT_Func::getObject().getTH_Character(it.toString());
                if (data.isEmpty() == false && Character_Song.contains(data) == false)
                    Character_Song += data;
            }

            YT_Lyrics_Url.append(QString("https://lyrics.thwiki.cc/%1.1.ja.lrc").arg(Title));
            YT_Lyrics_Url.append(QString("https://lyrics.thwiki.cc/%1.1.zh.lrc").arg(Title));

            QJsonObject value = ret_tags.isEmpty() ? QJsonObject() : ret_tags[0];
            value["Title"] = Title;
            value["Album"] = Album;
            if (Artist.isEmpty() == false)
                value["Artist"] = Artist;
            if (Societies.isEmpty() == false)
                value["Societies"] = Societies;
            if (Original_Song.isEmpty() == false)
                value["Original_Song"] = Original_Song;
            if (Character_Song.isEmpty() == false)
                value["Character_Song"] = Character_Song;
            if (YT_Lyrics_Url.isEmpty() == false)
                value["YT_Lyrics_Url"] = YT_Lyrics_Url;
            if (YT_Cover_Url.isEmpty() == false)
                value["YT_Cover_Url"] = YT_Cover_Url;
            if (value.find("Original_Title") == value.end())
                value["Original_Title"] = Title;

            ret_tags.append(value);
        };

    int sic = YT_Func::readXmlNext(xml_reader, "table", { { "class", "wikitable mw-collapsible" } });
    if (sic == false)
    {
        auto search_data = QUrl::fromPercentEncoding(url.toUtf8());
        static const QRegularExpression re_get_data(R"(歌词:(.+)$)");
        auto match = re_get_data.match(search_data);
        search_data = match.captured(1);

        static const QRegularExpression re_filter_data(R"((?i)^(.*)(?=\(|（|\[|［|~|～|-|－|feat|ver))");
        match = re_filter_data.match(search_data);
        if (match.hasMatch() == false)
        {
            qDebug() << url << " YT_Func::getTH_Wiki_Lyrics error";
            return {};
        }

        search_data = match.captured(1).trimmed();
        return getTH_Wiki_Lyrics("https://thwiki.cc/歌词:" + search_data);
    }
    analysisTagFunc();

    while (sic)
    {
        sic = YT_Func::readXmlNext(xml_reader, "tr", { { "class", "mw-collapsible mw-collapsed" } });
        if (sic == false)
            break;
        analysisTagFunc();
    }
    return ret_tags;
}

QList<QJsonObject> YT_Func::getMusic_163_Url(const QString& url)
{
    QList<QJsonObject> ret_tags;
    auto data = YT_Func::getObject().getNetworkData(url, QThread::currentThread());
    if (data.isEmpty())
        return {};

    auto json = QJsonDocument::fromJson(data, nullptr).object();
    if (json.isEmpty())
    {
        qDebug() << url << " YT_Func getMusic_163_Url json Error " << json;
        return {};
    }

    auto songs = json["songs"].toArray();
    for (auto it : songs)
    {
        auto info = it.toObject();

        auto id = info["id"].toInt();
        auto title = info["name"].toString();

        QJsonArray artist;
        auto artist_info = info["artists"].toArray();
        for (auto it : artist_info)
        {
            auto info = it.toObject();
            artist += info["name"].toString();
        }

        QString album;
        auto album_info = info["album"].toObject();
        album = album_info["name"].toString();

        QString cover_url = album_info.find("picUrl") != album_info.end() ? album_info["picUrl"].toString() : QString();
        QString lyrics_url = QString("https://music.163.com/api/song/lyric?id=%1&lv=-1&tv=-1").arg(QString::number(id));

        // const auto key = (title + " & " + album);
        QJsonObject value = QJsonObject();
        value["Title"] = title;
        value["Album"] = album;
        value["Artist"] = artist;
        value["YT_Lyrics_Url"] = lyrics_url;
        if (cover_url.isEmpty() == false)
            value["YT_Cover_Url"] = cover_url;

        // bool is_connect = title == media_tags.firstTag(Media::MetaData_Title) && album == media_tags.firstTag(Media::MetaData_Album);
        // addData(key, QString(QJsonDocument(value).toJson()), NE_Json, is_connect);
        ret_tags.append(value);
    }
    return ret_tags;
}

QList<QJsonObject> YT_Func::getMusic_163_Search(const QString& search_data)
{
    QList<QJsonObject> ret_tags;
    if (search_data.isEmpty())
    {
        qDebug() << "YT_Func getMusic_163_Search search_data = nullptr";
        return {};
    }

    const auto url = QString("https://music.163.com/api/search/get?s=%1&type=1&offset=0&limit=5").arg(search_data);
    auto data = YT_Func::getObject().getNetworkData(url, QThread::currentThread());
    if (data.isEmpty())
        return {};

    auto json = QJsonDocument::fromJson(data, nullptr).object();
    if (json.isEmpty())
    {
        qDebug() << url << " YT_Func inputMusic_163_Search json Error " << json;
        return {};
    }

    json = json["result"].toObject();
    if (json["songCount"].toInt(0) == 0)
    {
        qDebug() << url << "YT_Func inputMusic_163_Search songCount = 0" << json;
        return {};
    }

    auto songs = json["songs"].toArray();
    for (auto it : songs)
    {
        auto info = it.toObject();

        auto id = info["id"].toInt();
        auto title = info["name"].toString();

        QJsonArray artist;
        auto artist_info = info["artists"].toArray();
        for (auto it : artist_info)
        {
            auto info = it.toObject();
            artist += info["name"].toString();
        }

        QString album;
        auto album_info = info["album"].toObject();
        album = album_info["name"].toString();

        QString lyrics_url = QString("https://music.163.com/api/song/lyric?id=%1&lv=-1&tv=-1").arg(QString::number(id));

        // const auto key = (title + " & " + album);
        QJsonObject value = QJsonObject();
        value["Title"] = title;
        value["Album"] = album;
        value["Artist"] = artist;
        value["YT_Lyrics_Url"] = lyrics_url;
        // for (auto it = append_data.constBegin(); it != append_data.constEnd(); it++)
        // {
        //     value[it.key()] = it.value();
        // }
        // value["YT_Cover_Url"] = YT_Cover_Url;

        // bool is_connect = title == media_tags.firstTag(Media::MetaData_Title) && album == media_tags.firstTag(Media::MetaData_Album);
        // addData(key, QString(QJsonDocument(value).toJson()), NE_Json, is_connect);
        ret_tags.append(value);
    }
    return ret_tags;
}

int YT_Func::getBestMusicTag(const QList<QJsonObject>& tags, const QJsonObject& data, const QString& ex_album)
{
    int best_index = -true;
    const auto o_title = data.value(Media::MetaData_Title).toString();
    const auto o_album = data.value(Media::MetaData_Album).toString();
    for (int i = 0; i < tags.count(); i++)
    {
        const auto& it = tags[i];
        const auto c_title = it.value(Media::MetaData_Title).toString();
        const auto c_album = it.value(Media::MetaData_Album).toString();

        if (o_album != c_album && ex_album != c_album)
        {
            continue;
        }
        if (o_title == c_title)
        {
            best_index = i;
            break;
        }
        if (best_index >= 0 || best_index == INT_MIN)
        {
            best_index = INT_MIN;
            continue;
        }
        best_index = i;
    }
    return best_index;
}

int YT_Func::MediaEncoder_Write_YT_Data(MediaEncoder& coder, const QByteArray& data)
{
    static const auto writeMetadata = [](AVDictionary* dic, const QJsonObject& json) {
        const auto title = DB::jsTagList<QString>(json, Media::MetaData_Title).join(';').toUtf8();
        if (title.isEmpty() == false)
        {
            av_dict_set(&dic, Media::MetaData_Title, title, 0);
        }

        const auto artist = DB::jsTagList<QString>(json, Media::MetaData_Artist).join(';').toUtf8();
        if (artist.isEmpty() == false)
        {
            av_dict_set(&dic, Media::MetaData_Artist, artist, 0);
        }

        const auto album = DB::jsTagList<QString>(json, Media::MetaData_Album).join(';').toUtf8();
        if (album.isEmpty() == false)
        {
            av_dict_set(&dic, Media::MetaData_Album, album, 0);
        }
        };
    static const auto getCoverUrlData = [](QString url) -> QByteArray {
        if (url.isEmpty())
            return {};

        if (url.contains("thwiki.cc"))
        {
            QXmlStreamReader cover_xml_reader(YT_Func::getObject().getNetworkData(url, QThread::currentThread()));
            if ((cover_xml_reader.atEnd() == false) && (cover_xml_reader.hasError() == false))
            {
                YT_Func::readXmlNext(cover_xml_reader, "a", { { "class", "internal" } });
                url = cover_xml_reader.attributes().value("href").toString();
            }
            if (url.isEmpty())
                return {};
        }

        const QString file_root = "./YT_PlayerCache";
        const QString file_suffix = QFileInfo(QUrl(url).path()).suffix();
        const QString file_name = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5).toHex();
        const QString file_path = file_root + '/' + file_name + '.' + file_suffix;

        QFile file(file_path);
        if (file.exists() && file.open(QIODevice::ReadOnly))
            return file_path.toUtf8();

        auto file_data = YT_Func::obj().getNetworkData(url, QThread::currentThread());
        if (file_data.isEmpty())
        {
            qDebug() << url << " YT_Func::MediaEncoder_Write_YT_Data getCoverUrlData file_data.isEmpty()";
            return {};
        }
        if (file.open(QIODevice::WriteOnly) == false)
        {
            qDebug() << url << " YT_Func::MediaEncoder_Write_YT_Data getCoverUrlData file.open(QIODevice::WriteOnly) == false";
            return {};
        }

        file.write(file_data);
        return file_path.toUtf8();
        };
    static const auto getLyricsUrlData = [](QString url) -> QByteArray {
        const auto url_host = QUrl(url).host();
        if (url.contains("music.163.com/api/song"))
        {
            auto data = YT_Func::getObject().getNetworkData(url, QThread::currentThread());
            auto json = QJsonDocument::fromJson(data, nullptr).object();
            if (json.isEmpty())
            {
                qDebug() << url << " YT_Func::MediaEncoder_Write_YT_Data getLyricsUrlData json Error " << json;
                return {};
            }

            QByteArray ret_data = "";
            auto json_lrc = json.find("lrc");
            if (json_lrc != json.end())
            {
                ret_data += json_lrc.value().toObject().value("lyric").toString().toUtf8();
            }
            auto json_tlyric = json.find("tlyric");
            if (json_tlyric != json.end())
            {
                ret_data += json_tlyric.value().toObject().value("lyric").toString().toUtf8();
            }
            return ret_data;
        }
        else if (QFileInfo(url).suffix() == "lrc")
        {
            return YT_Func::getObject().getNetworkData(url, QThread::currentThread());
        }
        else
        {
            return {};
        }
        };

    auto format_ctx = coder.format_ctx;
    int sic = av_dict_set(&format_ctx->metadata, Media::MetaData_YT_Data, data, 0);
    if (sic < 0)
    {
        char error[256];
        av_strerror(sic, error, sizeof(error));
        qDebug() << "YT_Func::MediaEncoder_Write_YT_Data av_dict_set " << Media::MetaData_YT_Data << " " << error;
    }

    const auto json = QJsonDocument::fromJson(data, nullptr).object();
    writeMetadata(format_ctx->metadata, json);

    auto cover_url = json.find("YT_Cover_Url");
    if (cover_url != json.end())
    {
        auto url = cover_url.value().toString();
        auto data = getCoverUrlData(url);
        if (data.isEmpty() == false)
            coder.addCoverStream(data);
    }

    auto lyrics_url = json.find("YT_Lyrics_Url");
    if (lyrics_url != json.end())
    {
        auto url_list = jsonListData(lyrics_url.value());
        for (auto it : url_list)
        {
            auto data = getLyricsUrlData(it.toString());
            if (data.isEmpty() == false)
                coder.addLyricsStream(data);
        }
    }
    return Media::NoneError;
}

QString YT_Func::outputFileCover(QString file_path)
{
    const QFileInfo file_info(file_path);
    if (file_info.isFile() == false)
        return {};

    if (QDir().mkpath("./YT_PlayerCache") == false) {
        qWarning() << "YT_Func::outputFileCover could not create cover cache directory";
        return {};
    }

    QString target_path = file_path;
    const bool isQtSupported = QImageReader::supportedImageFormats().contains(file_info.suffix());
    if (isQtSupported == false)
    {
        VideoBuffer media_buffer(target_path.toUtf8(), true);
        if (media_buffer.isValid() == false) {
            return {};
        }

        QString file_suffix;
        {
            static const QMap<AVCodecID, QString> file_suffix_map = {
                { AV_CODEC_ID_PNG,  "png" },
                { AV_CODEC_ID_MJPEG, "jpg" },
                { AV_CODEC_ID_MJPEG, "jpeg" }   // 注意：同一个 key 不能出现两次！
            };
            file_suffix = file_suffix_map.value(media_buffer.mediaStream()->codecpar->codec_id);
            if (file_suffix.isEmpty()) {
                return {};
            }
        }

        target_path = "./YT_PlayerCache/" + file_info.baseName() + '.' + file_suffix;
        MediaEncoder media_encoder;
        int sic = media_encoder.setMediaPath(target_path.toUtf8());
        if (sic < 0) {
            return {};
        }

        sic = media_encoder.addStream(&media_buffer);
        if (sic < 0) {
            return {};
        }

        sic = media_encoder.encodeRun();
        if (sic < 0) {
            return {};
        }
        media_encoder.release();
    }

    QFile file(target_path);
    if (file.exists() == false || file.open(QIODevice::ReadOnly) == false) {
        return {};
    }

    const QString file_hash = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
    const QString file_cover_path = "./YT_PlayerCache/" + file_hash + '.' + QFileInfo(target_path).suffix();

    QImage q_image = QImage(target_path).scaledToWidth(300, Qt::SmoothTransformation);
    if (q_image.isNull() || q_image.save(file_cover_path) == false) {
        return {};
    }
    if (isQtSupported == false) {
        file.remove();
    }

    const auto image_focus = getFileCoverVisualFocus(q_image);
    const auto db_thread = DB_DatabaseInstance::obj().db.thread();
    QMetaObject::invokeMethod(db_thread, [file_path, file_hash, image_focus] {
        DB_DatabaseInstance::obj().setFileCoverInfo_HashValue(file_path, file_hash);
        DB_DatabaseInstance::obj().setFileCoverInfo_VisualFocus(file_path, image_focus);
        }, Qt::QueuedConnection);
    return file_hash;
}

Q_INVOKABLE QString YT_Func::getFileCoverPath(QString file_path)
{
    const auto data = DB_DatabaseInstance::obj().getFileCoverInfo_HashValue(file_path);
    return "image://CoverCache/" + (data.isEmpty() ? file_path : data);
}

Q_INVOKABLE QString YT_Func::getFileCoverPath_InfoId(int info_id)
{
    auto file_path = DB_DatabaseInstance::obj().getFileInfo(info_id, DB_FileInfo::I_FilePath).toString();
    if (file_path.isEmpty()) return {};
    return getFileCoverPath(file_path);
}

QPointF YT_Func::getFileCoverVisualFocus(const QImage& source_image)
{
    if (source_image.isNull()) {
        qWarning() << "YT_Func::getFileCoverVisualFocus received an empty source image";
        return QPointF(0.5, 0.5);
    }

    const auto q_image = source_image.convertToFormat(QImage::Format_BGR888);
    const cv::Mat mat_image(
        q_image.height(),
        q_image.width(),
        CV_8UC3,
        const_cast<uchar*>(q_image.constBits()),
        static_cast<size_t>(q_image.bytesPerLine()));

    QPointF visual_focus(0.5, 0.5);
    static std::once_flag model_initialization_flag;
    static cv::dnn::Net face_detection_model;
    std::call_once(model_initialization_flag, [] {
        const QString model_path = QDir(QCoreApplication::applicationDirPath())
            .filePath("YT_PlayerData/Models/AnimeFace/model.onnx");
        try {
            face_detection_model = cv::dnn::readNet(model_path.toStdString());
            if (face_detection_model.empty()) {
                qWarning().noquote() << "YT_Func::getFileCoverVisualFocus loaded an empty anime-face model:"
                    << model_path;
                return;
            }
            face_detection_model.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        }
        catch (const cv::Exception& error) {
            qWarning().noquote() << "YT_Func::getFileCoverVisualFocus failed to load anime-face model:"
                << model_path << QString::fromLocal8Bit(error.what());
        }
        });
    if (face_detection_model.empty()) {
        return visual_focus;
    }

    try {
        constexpr int model_input_size = 640;
        constexpr float face_score_threshold = 0.618f;
        constexpr float nms_threshold = 0.45f;

        const float resize_scale = std::min(
            static_cast<float>(model_input_size) / mat_image.cols,
            static_cast<float>(model_input_size) / mat_image.rows);
        const int resized_width = std::max(1, static_cast<int>(std::round(mat_image.cols * resize_scale)));
        const int resized_height = std::max(1, static_cast<int>(std::round(mat_image.rows * resize_scale)));
        const int padding_x = (model_input_size - resized_width) / 2;
        const int padding_y = (model_input_size - resized_height) / 2;

        cv::Mat resized_image;
        cv::resize(mat_image, resized_image, cv::Size(resized_width, resized_height));
        cv::Mat letterboxed_image(
            model_input_size,
            model_input_size,
            CV_8UC3,
            cv::Scalar(114, 114, 114));
        resized_image.copyTo(letterboxed_image(cv::Rect(
            padding_x,
            padding_y,
            resized_width,
            resized_height)));

        const cv::Mat input_blob = cv::dnn::blobFromImage(
            letterboxed_image,
            1.0 / 255.0,
            cv::Size(model_input_size, model_input_size),
            cv::Scalar(),
            true,
            false,
            CV_32F);

        static std::mutex inference_mutex;
        std::vector<cv::Mat> outputs;
        {
            const std::lock_guard<std::mutex> lock(inference_mutex);
            face_detection_model.setInput(input_blob);
            face_detection_model.forward(outputs, face_detection_model.getUnconnectedOutLayersNames());
        }

        if (outputs.empty()) {
            qWarning() << "YT_Func::getFileCoverVisualFocus received no output from the anime-face model";
            return visual_focus;
        }

        const cv::Mat& output = outputs.front();
        bool channels_first = false;
        int attributes = 0;
        int candidate_count = 0;

        if (output.dims == 3 && output.size[0] == 1) {
            if (output.size[1] == 5 && output.size[2] > 5) {
                channels_first = true;
                attributes = output.size[1];
                candidate_count = output.size[2];
            }
            else if (output.size[2] == 5 && output.size[1] > 5) {
                attributes = output.size[2];
                candidate_count = output.size[1];
            }
        }
        else if (output.dims == 2) {
            if (output.size[0] == 5 && output.size[1] > 5) {
                channels_first = true;
                attributes = output.size[0];
                candidate_count = output.size[1];
            }
            else if (output.size[1] == 5 && output.size[0] > 5) {
                attributes = output.size[1];
                candidate_count = output.size[0];
            }
        }
        if (attributes != 5 || candidate_count == 0 || output.type() != CV_32F) {
            qWarning() << "YT_Func::getFileCoverVisualFocus received an unsupported anime-face model output shape";
            return visual_focus;
        }

        const float* output_values = output.ptr<float>();
        const auto output_value = [=](int candidate, int attribute) {
            return channels_first
                ? output_values[attribute * candidate_count + candidate]
                : output_values[candidate * attributes + attribute];
            };

        std::vector<cv::Rect> face_boxes;
        std::vector<float> face_scores;
        for (int candidate = 0; candidate < candidate_count; ++candidate) {
            const float score = output_value(candidate, 4);
            if (std::isfinite(score) == false || score < face_score_threshold)
                continue;

            const float center_x = output_value(candidate, 0);
            const float center_y = output_value(candidate, 1);
            const float box_width = output_value(candidate, 2);
            const float box_height = output_value(candidate, 3);
            if (std::isfinite(center_x) == false || std::isfinite(center_y) == false
                || std::isfinite(box_width) == false || std::isfinite(box_height) == false
                || box_width <= 0 || box_height <= 0) {
                continue;
            }

            const int left = std::clamp(
                static_cast<int>(std::floor((center_x - box_width / 2.0f - padding_x) / resize_scale)),
                0,
                mat_image.cols);
            const int top = std::clamp(
                static_cast<int>(std::floor((center_y - box_height / 2.0f - padding_y) / resize_scale)),
                0,
                mat_image.rows);
            const int right = std::clamp(
                static_cast<int>(std::ceil((center_x + box_width / 2.0f - padding_x) / resize_scale)),
                0,
                mat_image.cols);
            const int bottom = std::clamp(
                static_cast<int>(std::ceil((center_y + box_height / 2.0f - padding_y) / resize_scale)),
                0,
                mat_image.rows);
            if (right <= left || bottom <= top)
                continue;

            face_boxes.emplace_back(left, top, right - left, bottom - top);
            face_scores.push_back(score);
        }

        std::vector<int> retained_faces;
        cv::dnn::NMSBoxes(
            face_boxes,
            face_scores,
            face_score_threshold,
            nms_threshold,
            retained_faces);
        if (retained_faces.empty() && face_boxes.empty() == false) {
            qWarning() << "YT_Func::getFileCoverVisualFocus could not retain any anime-face detections";
            return visual_focus;
        }
        if (retained_faces.empty())
            return visual_focus;

        int highest_scoring_face = retained_faces.front();
        for (const int face_index : retained_faces) {
            if (face_scores[face_index] > face_scores[highest_scoring_face])
                highest_scoring_face = face_index;
        }
        const cv::Rect& face = face_boxes[highest_scoring_face];
        visual_focus = QPointF(
            std::clamp((face.x + face.width * 0.5) / mat_image.cols, 0.0, 1.0),
            std::clamp((face.y + face.height * 0.5) / mat_image.rows, 0.0, 1.0));
    }
    catch (const cv::Exception& error) {
        qWarning().noquote() << "YT_Func::getFileCoverVisualFocus failed during anime-face detection:"
            << QString::fromLocal8Bit(error.what());
    }
    return visual_focus;
}

Q_INVOKABLE QPointF YT_Func::getFileCoverVisualFocus(const QString& file_path)
{
    return DB_DatabaseInstance::obj().getFileCoverInfo_VisualFocus(file_path);
}

QRect YT_Func::getFileCoverVisualRect(double aspect_ratio, const QSize& image_size, const QPointF& focus)
{
    if (aspect_ratio <= 0 || std::isfinite(aspect_ratio) == false
        || image_size.width() <= 0 || image_size.height() <= 0) {
        return {};
    }

    const double crop_width = std::min(static_cast<double>(image_size.width()), image_size.height() * aspect_ratio);
    const double crop_height = crop_width / aspect_ratio;
    const int width = std::clamp(qRound(crop_width), 1, image_size.width());
    const int height = std::clamp(qRound(crop_height), 1, image_size.height());
    const int focus_x = std::clamp(static_cast<int>(std::floor(image_size.width() * focus.x())), 0, image_size.width() - 1);
    const int focus_y = std::clamp(static_cast<int>(std::floor(image_size.height() * focus.y())), 0, image_size.height() - 1);
    const int left = std::clamp(focus_x - width / 2, 0, image_size.width() - width);
    const int top = std::clamp(focus_y - height / 2, 0, image_size.height() - height);
    return QRect(left, top, width, height);
}

QImage YT_Func::outputFileCoverVisualRect(const QImage& source_image, const QRect& crop_rect, QString path_name)
{
    if (source_image.isNull()) {
        qWarning() << "YT_Func::outputFileCoverVisualRect received an empty source image";
        return {};
    }

    const QRect image_rect(QPoint(0, 0), source_image.size());
    const QRect bounded_crop_rect = crop_rect.intersected(image_rect);
    if (crop_rect.isValid() == false || bounded_crop_rect.isValid() == false) {
        qWarning() << "YT_Func::outputFileCoverVisualRect received an invalid crop rectangle";
        return {};
    }

    QImage debug_image = source_image.convertToFormat(QImage::Format_ARGB32);
    {
        QPainter painter(&debug_image);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::NoBrush);
        const int frame_width = std::max(2, qRound(std::min(source_image.width(), source_image.height()) / 300.0));
        painter.setPen(QPen(Qt::green, frame_width));
        painter.drawRect(bounded_crop_rect);

        const int focus_radius = std::max(3, std::min(source_image.width(), source_image.height()) / 100);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::red);
        painter.drawEllipse(QPointF(crop_rect.center()), focus_radius, focus_radius);
    }

    const QString debug_directory = QDir::current().filePath("YT_PlayerCache");
    if (QDir().mkpath(debug_directory) == false) {
        qWarning().noquote() << "YT_Func::outputFileCoverVisualRect could not create debug directory:"
            << debug_directory;
        return {};
    }

    const QString debug_path = QDir(debug_directory).filePath(path_name + ".png");
    if (debug_image.save(debug_path, "PNG") == false) {
        qWarning().noquote() << "YT_Func::outputFileCoverVisualRect could not save debug image:"
            << debug_path;
        return {};
    }
    qInfo().noquote() << "YT_Func::outputFileCoverVisualRect wrote debug image:" << debug_path;
    return debug_image;
}

// QPoint YT_Func::outputFileCoverVisualFocus(QString file_path)
// {
//     QImageReader image_reader(file_path);
//     image_reader.setAutoTransform(true);
//     const QImage image = image_reader.read();
//     if (image.isNull()) {
//         qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus failed to read cover image:"
//             << file_path << image_reader.errorString();
//         return {};
//     }

//     const QImage rgb_image = image.convertToFormat(QImage::Format_RGB888);
//     const cv::Mat rgb_image_mat(
//         rgb_image.height(),
//         rgb_image.width(),
//         CV_8UC3,
//         const_cast<uchar*>(rgb_image.constBits()),
//         static_cast<size_t>(rgb_image.bytesPerLine()));
//     cv::Mat source_image;
//     cv::cvtColor(rgb_image_mat, source_image, cv::COLOR_RGB2BGR);

//     const QSize source_size(source_image.cols, source_image.rows);
//     const QPoint image_center(source_size.width() / 2, source_size.height() / 2);
//     QPoint visual_focus = image_center;

//     static std::once_flag model_initialization_flag;
//     static cv::dnn::Net face_detection_model;
//     std::call_once(model_initialization_flag, [] {
//         const QString model_path = QDir(QCoreApplication::applicationDirPath())
//             .filePath("YT_PlayerData/Models/AnimeFace/model.onnx");
//         QFile model_file(model_path);
//         if (model_file.open(QIODevice::ReadOnly) == false) {
//             qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus failed to open anime-face model:"
//                 << model_path << model_file.errorString();
//             return;
//         }

//         const QByteArray model_data = model_file.readAll();
//         if (model_data.isEmpty() || model_file.error() != QFileDevice::NoError) {
//             qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus failed to read anime-face model:"
//                 << model_path << model_file.errorString();
//             return;
//         }

//         try {
//             face_detection_model = cv::dnn::readNetFromONNX(
//                 model_data.constData(),
//                 static_cast<size_t>(model_data.size()));
//             if (face_detection_model.empty()) {
//                 qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus loaded an empty anime-face model:"
//                     << model_path;
//                 return;
//             }
//             face_detection_model.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
//         }
//         catch (const cv::Exception& error) {
//             qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus failed to load anime-face model:"
//                 << model_path << QString::fromLocal8Bit(error.what());
//         }
//         });
//     if (face_detection_model.empty()) {
//         return visual_focus;
//     }

//     try {
//         constexpr int model_input_size = 640;
//         constexpr float face_score_threshold = 0.278f;
//         constexpr float nms_threshold = 0.45f;

//         const float resize_scale = std::min(
//             static_cast<float>(model_input_size) / source_image.cols,
//             static_cast<float>(model_input_size) / source_image.rows);
//         const int resized_width = std::max(1, static_cast<int>(std::round(source_image.cols * resize_scale)));
//         const int resized_height = std::max(1, static_cast<int>(std::round(source_image.rows * resize_scale)));
//         const int padding_x = (model_input_size - resized_width) / 2;
//         const int padding_y = (model_input_size - resized_height) / 2;

//         cv::Mat resized_image;
//         cv::resize(source_image, resized_image, cv::Size(resized_width, resized_height));
//         cv::Mat letterboxed_image(
//             model_input_size,
//             model_input_size,
//             CV_8UC3,
//             cv::Scalar(114, 114, 114));
//         resized_image.copyTo(letterboxed_image(cv::Rect(
//             padding_x,
//             padding_y,
//             resized_width,
//             resized_height)));

//         const cv::Mat input_blob = cv::dnn::blobFromImage(
//             letterboxed_image,
//             1.0 / 255.0,
//             cv::Size(model_input_size, model_input_size),
//             cv::Scalar(),
//             true,
//             false,
//             CV_32F);

//         static std::mutex inference_mutex;
//         std::vector<cv::Mat> outputs;
//         {
//             const std::lock_guard<std::mutex> lock(inference_mutex);
//             face_detection_model.setInput(input_blob);
//             face_detection_model.forward(outputs, face_detection_model.getUnconnectedOutLayersNames());
//         }

//         if (outputs.empty()) {
//             qWarning() << "YT_Func::outputFileCoverVisualFocus received no output from the anime-face model";
//             return visual_focus;
//         }

//         const cv::Mat& output = outputs.front();
//         bool channels_first = false;
//         int attributes = 0;
//         int candidate_count = 0;

//         if (output.dims == 3 && output.size[0] == 1) {
//             if (output.size[1] == 5 && output.size[2] > 5) {
//                 channels_first = true;
//                 attributes = output.size[1];
//                 candidate_count = output.size[2];
//             }
//             else if (output.size[2] == 5 && output.size[1] > 5) {
//                 attributes = output.size[2];
//                 candidate_count = output.size[1];
//             }
//         }
//         else if (output.dims == 2) {
//             if (output.size[0] == 5 && output.size[1] > 5) {
//                 channels_first = true;
//                 attributes = output.size[0];
//                 candidate_count = output.size[1];
//             }
//             else if (output.size[1] == 5 && output.size[0] > 5) {
//                 attributes = output.size[1];
//                 candidate_count = output.size[0];
//             }
//         }

//         if (attributes != 5 || candidate_count == 0 || output.type() != CV_32F) {
//             qWarning() << "YT_Func::outputFileCoverVisualFocus received an unsupported anime-face model output shape";
//             return visual_focus;
//         }

//         const float* output_values = output.ptr<float>();
//         const auto output_value = [=](int candidate, int attribute) {
//             return channels_first
//                 ? output_values[attribute * candidate_count + candidate]
//                 : output_values[candidate * attributes + attribute];
//             };

//         std::vector<cv::Rect> face_boxes;
//         std::vector<float> face_scores;
//         for (int candidate = 0; candidate < candidate_count; ++candidate) {
//             const float score = output_value(candidate, 4);
//             if (std::isfinite(score) == false || score < face_score_threshold)
//                 continue;

//             const float center_x = output_value(candidate, 0);
//             const float center_y = output_value(candidate, 1);
//             const float box_width = output_value(candidate, 2);
//             const float box_height = output_value(candidate, 3);
//             if (std::isfinite(center_x) == false || std::isfinite(center_y) == false
//                 || std::isfinite(box_width) == false || std::isfinite(box_height) == false
//                 || box_width <= 0 || box_height <= 0) {
//                 continue;
//             }

//             const int left = std::clamp(
//                 static_cast<int>(std::floor((center_x - box_width / 2.0f - padding_x) / resize_scale)),
//                 0,
//                 source_image.cols);
//             const int top = std::clamp(
//                 static_cast<int>(std::floor((center_y - box_height / 2.0f - padding_y) / resize_scale)),
//                 0,
//                 source_image.rows);
//             const int right = std::clamp(
//                 static_cast<int>(std::ceil((center_x + box_width / 2.0f - padding_x) / resize_scale)),
//                 0,
//                 source_image.cols);
//             const int bottom = std::clamp(
//                 static_cast<int>(std::ceil((center_y + box_height / 2.0f - padding_y) / resize_scale)),
//                 0,
//                 source_image.rows);
//             if (right <= left || bottom <= top)
//                 continue;

//             face_boxes.emplace_back(left, top, right - left, bottom - top);
//             face_scores.push_back(score);
//         }

//         std::vector<int> retained_faces;
//         cv::dnn::NMSBoxes(
//             face_boxes,
//             face_scores,
//             face_score_threshold,
//             nms_threshold,
//             retained_faces);
//         if (retained_faces.empty() && face_boxes.empty() == false) {
//             qWarning() << "YT_Func::outputFileCoverVisualFocus could not retain any anime-face detections";
//             return visual_focus;
//         }

//         double center_x_sum = 0;
//         double center_y_sum = 0;
//         for (const int face_index : retained_faces) {
//             const cv::Rect& face = face_boxes[face_index];
//             center_x_sum += face.x + face.width / 2.0;
//             center_y_sum += face.y + face.height / 2.0;
//         }
//         visual_focus = QPoint(
//             std::clamp(
//                 qRound(center_x_sum / retained_faces.size()),
//                 0,
//                 source_image.cols - 1),
//             std::clamp(
//                 qRound(center_y_sum / retained_faces.size()),
//                 0,
//                 source_image.rows - 1));
//     }
//     catch (const cv::Exception& error) {
//         qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus failed during anime-face detection:"
//             << QString::fromLocal8Bit(error.what());
//     }

//     if (true) {
//         const auto calculateCropRect = [](double aspect_ratio, const QSize& image_size, const QPoint& focus) -> QRect {
//             if (aspect_ratio <= 0 || std::isfinite(aspect_ratio) == false
//                 || image_size.width() <= 0 || image_size.height() <= 0) {
//                 return {};
//             }

//             const double crop_width = std::min(
//                 static_cast<double>(image_size.width()),
//                 image_size.height() * aspect_ratio);
//             const double crop_height = crop_width / aspect_ratio;
//             const int width = std::clamp(qRound(crop_width), 1, image_size.width());
//             const int height = std::clamp(qRound(crop_height), 1, image_size.height());
//             const int focus_x = std::clamp(focus.x(), 0, image_size.width() - 1);
//             const int focus_y = std::clamp(focus.y(), 0, image_size.height() - 1);
//             const int left = std::clamp(focus_x - width / 2, 0, image_size.width() - width);
//             const int top = std::clamp(focus_y - height / 2, 0, image_size.height() - height);
//             return QRect(left, top, width, height);
//             };

//         const auto writeDebugImage = [&](const cv::Mat& source, const QRect& crop_rect, const QPoint& focus) {
//             cv::Mat debug_image = source.clone();
//             cv::rectangle(
//                 debug_image,
//                 cv::Rect(crop_rect.x(), crop_rect.y(), crop_rect.width(), crop_rect.height()),
//                 cv::Scalar(0, 255, 0),
//                 2,
//                 cv::LINE_AA);
//             cv::circle(
//                 debug_image,
//                 cv::Point(focus.x(), focus.y()),
//                 std::max(3, std::min(source.cols, source.rows) / 100),
//                 cv::Scalar(0, 0, 255),
//                 cv::FILLED,
//                 cv::LINE_AA);

//             std::vector<uchar> encoded_image;
//             try {
//                 if (cv::imencode(".png", debug_image, encoded_image) == false) {
//                     qWarning() << "YT_Func::outputFileCoverVisualFocus could not encode the debug image";
//                     return;
//                 }
//             }
//             catch (const cv::Exception& error) {
//                 qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus could not encode the debug image:"
//                     << QString::fromLocal8Bit(error.what());
//                 return;
//             }

//             const QString debug_directory = QDir::current().filePath("YT_PlayerCache");
//             if (QDir().mkpath(debug_directory) == false) {
//                 qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus could not create debug directory:"
//                     << debug_directory;
//                 return;
//             }
//             const QString base_name = QFileInfo(file_path).completeBaseName();
//             const QString debug_path = QDir(debug_directory).filePath(
//                 (base_name.isEmpty() ? QStringLiteral("cover") : base_name) + QStringLiteral("_visual_focus.png"));
//             QFile debug_file(debug_path);
//             if (debug_file.open(QIODevice::WriteOnly) == false) {
//                 qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus could not open debug image:"
//                     << debug_path << debug_file.errorString();
//                 return;
//             }
//             const qint64 bytes_written = debug_file.write(
//                 reinterpret_cast<const char*>(encoded_image.data()),
//                 static_cast<qint64>(encoded_image.size()));
//             if (bytes_written != static_cast<qint64>(encoded_image.size())) {
//                 qWarning().noquote() << "YT_Func::outputFileCoverVisualFocus could not write debug image:"
//                     << debug_path << debug_file.errorString();
//                 return;
//             }
//             qInfo().noquote() << "YT_Func::outputFileCoverVisualFocus wrote debug image:" << debug_path;
//             };

//         const QRect crop_rect = calculateCropRect(16.0 / 9.0, source_size, visual_focus);
//         if (crop_rect.isValid())
//             writeDebugImage(source_image, crop_rect, visual_focus);
//         else
//             qWarning() << "YT_Func::outputFileCoverVisualFocus could not calculate the debug crop rectangle";
//     }

//     return visual_focus;
// }
