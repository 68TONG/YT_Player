#include "YT_Func.h"
#include "YT_DataBase.h"
#include "DB_DataBase.h"

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

YT_Func* YT_Func::create(QQmlEngine*, QJSEngine*)
{
    QJSEngine::setObjectOwnership(&getObject(), QJSEngine::CppOwnership);
    return &getObject();
}

void YT_Func::transformWindow(QWindow* window, int type)
{
    type == 0 ? window->startSystemMove() : window->startSystemResize((Qt::Edge)type);
}

bool YT_Func::intersects(const QRectF& v1, const QRectF& v2) const
{
    return v1.intersects(v2);
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
    if (Chinese_Dict == nullptr)
    {
        Chinese_Dict = new chinese_util::Dict();
        Chinese_Dict->LoadPinyinData("./YT_PlayerData/chinese_util/pinyinData.json");
        Chinese_Dict->LoadCharacterData("./YT_PlayerData/chinese_util/charsData.json");
    }
    return Chinese_Dict;
}

QString YT_Func::getChinesePinYin(const QString& data)
{
    auto dict = getChinese_Dict();
    chinese_util::PinyinResultString result;
    chinese_util::Pinyin::Convert(result, dict, data.toStdString(), chinese_util::PINYIN, true, "");
    return QString::fromStdString(accumulate(result.pinyin->begin(), result.pinyin->end(), std::string{}));
}

QString YT_Func::getChineseFirstPinYin(const QString& data)
{
    auto dict = getChinese_Dict();
    chinese_util::PinyinResultString result;
    chinese_util::Pinyin::Convert(result, dict, data.toStdString(), chinese_util::PINYIN_FIRST, true, "");
    return QString::fromStdString(accumulate(result.pinyin_first->begin(), result.pinyin_first->end(), std::string{}));
}

QByteArray YT_Func::getNetworkData(const QString& url, QThread* cur_thread)
{
    QByteArray data;
    auto reply = getNetworkReply(url, cur_thread);
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "YT_Func::getNetworkData error: " << reply->errorString();
    }
    else if (reply->isFinished() == false)
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
    connect(qApp, &QCoreApplication::aboutToQuit, &loop, &QEventLoop::quit);
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

    int sic = YT_Func::readXmlNext(xml_reader, "td", { {"class", "label"} }, "名称");
    if (sic == false)
    {
        qDebug() << url << " YT_Func::getTH_Wiki_Album error";
        return {};
    }
    sic = YT_Func::readXmlNext(xml_reader, "td", {}, "true");
    Album = xml_reader.text().toString().trimmed();

    sic = YT_Func::readXmlNext(xml_reader, "td", { {"class", "label"} }, "制作方");
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

    sic = YT_Func::readXmlNext(xml_reader, "a", { {"class", "image"}, {"title", "封面图片"} });
    YT_Cover_Url = "https://thwiki.cc" + xml_reader.attributes().value("href").toString();

    sic = YT_Func::readXmlNext(xml_reader, "table", { {"class", "wikitable musicTable"} });
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
            YT_Func::readXmlNext(xml_reader, "td", { {"class", "title"} });
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
                YT_Func::readXmlNext(xml_reader, "a", { {"href", ""} }, "true");
                Title = xml_reader.text().toString().trimmed();
            }
            continue;
        }

        xml_reader.readNext();
        if (attributes.value("class") == "label" && xml_reader.text() == "演唱")
        {
            if (YT_Func::readXmlNext(xml_reader, "td", { {"class", "text"} }) == false)
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
            if (YT_Func::readXmlNext(xml_reader, "td", { {"class", "text"} }) == false)
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

            YT_Func::readXmlNext(xml_reader, "a", { {"class", "image"} });
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

    int sic = YT_Func::readXmlNext(xml_reader, "table", { {"class", "wikitable mw-collapsible"} });
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
        sic = YT_Func::readXmlNext(xml_reader, "tr", { {"class", "mw-collapsible mw-collapsed"} });
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

void YT_Func::MediaEncoder_Write_YT_Data(MediaEncoder& coder, const QString& data)
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
                YT_Func::readXmlNext(cover_xml_reader, "a", { {"class", "internal"} });
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
        if (file.open(QIODevice::WriteOnly) == false)
        {
            qDebug() << url << " NE_MediaOutput_Model::MediaEncoder_Write_YT_Data getCoverUrlData file.open(QIODevice::WriteOnly) == false";
            return {};
        }

        auto file_data = YT_Func::obj().getNetworkData(url, QThread::currentThread());
        if (file_data.isEmpty())
        {
            qDebug() << url << " NE_MediaOutput_Model::MediaEncoder_Write_YT_Data getCoverUrlData file_data.isEmpty()";
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
                qDebug() << url << " NE_MediaOutput_Model::MediaEncoder_Write_YT_Data getLyricsUrlData json Error " << json;
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
    int sic = av_dict_set(&format_ctx->metadata, Media::MetaData_YT_Data, data.toUtf8(), 0);
    if (sic < 0)
    {
        char error[256];
        av_strerror(sic, error, sizeof(error));
        qDebug() << "NE_MediaOutput_Model MediaEncoder_Write_YT_Data av_dict_set " << Media::MetaData_YT_Data << " " << error;
    }

    YT_ItemInfo::YT_InfoTag json(QJsonDocument::fromJson(data.toUtf8(), nullptr).object());
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
}
