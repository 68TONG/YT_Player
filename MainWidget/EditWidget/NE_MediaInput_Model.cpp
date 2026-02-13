#include "NE_MediaInput_Model.h"
#include "YT_Func.h"
#include "Music_DataInfo.h"

NE_MediaInput_Model::NE_MediaInput_Model(QObject* parent)
    : NE_MediaData_Model{ parent }
{
    connect(this, &NE_MediaInput_Model::inputUrlChanged, this, [this]() {
        if (isInputing.isEmpty() == false) return;
        dataModel.clear();
        emit dataModelChanged();
        media_decoder.release();
        media_tags = QJsonObject();

        const auto _URL_Host = QUrl(inputUrl).host();
        if (QList<QString>({ "thwiki.cc", "thbwiki.cc", "touhou.review" }).contains(_URL_Host)) {
            inputTH_Wiki(inputUrl);
        }
        else if (_URL_Host == "lyrics.thwiki.cc") {
            // findData_TouHou_THB_Wiki_Lyrics(_URL);
        }
        else if (inputUrl.contains("music.163.com/song")) {
            static const QRegularExpression re(R"(\?id=(\d+))");
            const auto id = re.match(inputUrl).captured(1);
            const auto url = QString("https://music.163.com/api/song/detail?ids=[%1]").arg(id);
            inputMusic_163_Url(url);
        }
        else if (inputUrl.contains("music.163.com/api/song")) {
            inputMusic_163_Url(inputUrl);
        }
        else if (QFile::exists(inputUrl)) {
            inputLocalFile(inputUrl);
        }
        else {
            inputMusic_163_Search(inputUrl);
        } });
}

void NE_MediaInput_Model::addData(QString title, QVariant data, int type, bool is_connect)
{
    QList<QVariant> it(IndexEnd);
    it[IndexTitle] = title;
    it[IndexData] = data;
    it[IndexType] = type;
    it[IndexAutoConnect] = is_connect;

    dataModel.append(it);
    emit dataModelChanged();
}

QFutureWatcher<QList<QJsonObject>>* NE_MediaInput_Model::getFutureWatcher()
{
    isInputing.enqueue(true);
    emit isInputingChanged();

    auto watcher = new QFutureWatcher<QList<QJsonObject>>();
    if (watcher->thread() != this->thread())
        watcher->moveToThread(this->thread());
    connect(watcher, &QFutureWatcher<QList<QJsonObject>>::finished, this, [this, watcher]() {
        static const auto jsonAddFunc = [](QJsonObject& dst, const QJsonObject& src) {
            for (auto it = src.begin(); it != src.end(); ++it)
                dst.insert(it.key(), it.value());
            };

        auto additional_json = QJsonDocument::fromJson(additionalData.toUtf8()).object();
        for (auto it = additional_json.begin(); it != additional_json.end(); ++it) {
            if (it.value() == "dirName()") {
                additional_json[it.key()] = media_decoder.getFormat_ctx() ? QFileInfo(media_decoder.getFormat_ctx()->url).absoluteDir().dirName() : "";
            }
        }

        auto data = watcher->result();
        const auto ex_data = media_decoder.getFormat_ctx() ? QFileInfo(media_decoder.getFormat_ctx()->url).absoluteDir().dirName() : "";
        const auto best_index = YT_Func::getBestMusicTag(data, media_tags, ex_data);
        for (int i = 0; i < data.count(); i++) {
            auto& it = data[i];
            const auto title = it.value(Media::MetaData_Title).toString();
            const auto album = it.value(Media::MetaData_Album).toString();

            if (additional_json.isEmpty() == false)
                jsonAddFunc(it, additional_json);
            addData(title + " & " + album, QString(QJsonDocument(it).toJson()), NE_Json, i == best_index);
        }

        isInputing.dequeue();
        emit isInputingChanged();
        watcher->deleteLater();
        });
    return watcher;
}

void NE_MediaInput_Model::inputAutoMatch()
{
    for (const auto it : matchUrl)
    {
        if (it == TH_Wiki_Lyrics)
        {
            const auto data = media_tags.firstTag(Media::MetaData_Title);
            inputTH_Wiki("https://thwiki.cc/歌词:" + data);
        }
        else if (it == TH_Wiki_Album)
        {
            const auto data = media_tags.firstTag(Media::MetaData_Album);
            inputTH_Wiki("https://thwiki.cc/" + data);
        }
        else if (it == Music_163)
        {
            const auto data = media_tags.firstTag(Media::MetaData_Title);
            inputMusic_163_Search(data); //, {{"Anime", QFileInfo(path).absoluteDir().dirName()}}
        }
    }
}

void NE_MediaInput_Model::inputLocalFile(const QString& url)
{
    isInputing.enqueue(true);
    emit isInputingChanged();

    if (media_decoder.initMediaPath(url.toStdString().c_str()) < 0)
        return;

    auto format_ctx = media_decoder.getFormat_ctx();
    for (int i = 0; i < format_ctx->nb_streams; i++)
    {
        auto codec_type = format_ctx->streams[i]->codecpar->codec_type;
        if (format_ctx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC)
        {
            auto media_buffer = (VideoBuffer*)media_decoder.getCreateBuffer(i);
            addData("封面流", QVariant::fromValue(media_buffer), NE_ImageStream);
        }
        else if (codec_type == AVMEDIA_TYPE_AUDIO)
        {
            auto media_buffer = (AudioBuffer*)media_decoder.getCreateBuffer(i);
            addData("音频流", QVariant::fromValue(media_buffer), NE_AudioStream, true);
        }
        else if (codec_type == AVMEDIA_TYPE_VIDEO)
        {
            auto media_buffer = (VideoBuffer*)media_decoder.getCreateBuffer(i);
            addData("视频流", QVariant::fromValue(media_buffer), NE_VideoStream);
        }
    }

    auto lyrics_data = av_dict_get(format_ctx->metadata, Media::MetaData_Lyrics, NULL, AV_DICT_IGNORE_SUFFIX);
    if (lyrics_data != NULL)
    {
        addData("歌词流", QString(lyrics_data->value), NE_LyricsStream);
    }

    if (QSet<QString>({ "mp3", "flac" }).contains(QFileInfo(url).suffix()))
    {
        Music_ItemInfo::getMusicMetaData(media_decoder, media_tags);
        if (media_tags.isEmpty() == false)
        {
            addData("元数据", QString(QJsonDocument(media_tags).toJson()), NE_Json);
            inputAutoMatch();
        }
    }

    isInputing.dequeue();
    emit isInputingChanged();
}

void NE_MediaInput_Model::inputTH_Wiki(const QString& url)
{
    auto watcher = getFutureWatcher();
    auto future = QtConcurrent::run(&YT_Func::getTH_Wiki, url);
    watcher->setFuture(future);
}

void NE_MediaInput_Model::inputMusic_163_Url(const QString& url)
{
    auto watcher = getFutureWatcher();
    auto future = QtConcurrent::run(&YT_Func::getMusic_163_Url, url);
    watcher->setFuture(future);
}

void NE_MediaInput_Model::inputMusic_163_Search(const QString& search_data)
{
    auto watcher = getFutureWatcher();
    auto future = QtConcurrent::run(&YT_Func::getMusic_163_Search, search_data);
    watcher->setFuture(future);
}
