#include "MusicWidget.h"

#include "YT_AudioOutput.h"
#include <QRandomGenerator>

MusicWidget::MusicWidget(QQuickItem* parent)
    : QQuickItem{ parent }, itemModel(*this), listModel(*this)
{
    this->setFlag(QQuickItem::ItemHasContents, true);

    // curListInfoID = Music_DataBase::getObject().getListInfo_NameALL().at(0);
    curListInfoID = listModel.infoIds.first();
    itemModel.setInfoIds(curListInfoID);
    curPath = { curListInfoID };

    connect(&itemModel, &MusicItemModel::infoIdsChanged, this, [this]()
        { setCurPlayIndex(YT_AudioOutput::getObject().source().toString()); });
    connect(&YT_AudioOutput::getObject(), &YT_AudioOutput::sourceChanged, this, [this](const QUrl& media)
        { setCurPlayIndex(media.toString()); });
}

void MusicWidget::playMusic(int ids_index)
{
    // auto& play_info = Music_DataBase::getObject().getPlayListInfo();
    auto &play_ids = MusicDatabase::obj().playFileInfoIds;
    const auto playModel = YT_AudioOutput::getObject().playModel;
    if (curListInfoID == MusicDatabase::obj().playFileInfoIdsName)
    {
        YT_AudioOutput::getObject().playIndex = ids_index - 1;
    }
    else if (playModel == "qrc:/Resource_UI/shuffle.png")
    {
        const auto& infoIds = itemModel.infoIds;

        play_ids.clear();
        play_ids.append(infoIds[ids_index]);
        for (int i = 0; i < infoIds.count(); ++i)
        {
            int index = QRandomGenerator::global()->bounded(infoIds.count());
            play_ids.append(infoIds[index]);
        }
        YT_AudioOutput::getObject().playIndex = 0 - 1;
    }
    else if (playModel == "qrc:/Resource_UI/single_loop.png")
    {
        play_ids = { itemModel.infoIds[ids_index] };
        YT_AudioOutput::getObject().playIndex = 0 - 1;
    }
    else if (playModel == "qrc:/Resource_UI/list_loop.png")
    {
        play_ids = itemModel.infoIds;
        YT_AudioOutput::getObject().playIndex = ids_index - 1;
    }
    YT_AudioOutput::getObject().playNextMusic();
}

void MusicWidget::addPlayMusic(const QString& path)
{
    // int info_id = Music_DataBase::getObject().addItemInfo(path);
    int info_id = MusicDatabase::obj().addFileInfo(path);
    int insert_index = qMax(YT_AudioOutput::getObject().playIndex + 1, 0);

    // auto &ID_List = Music_DataBase::getObject().getPlayListInfo().ID_List;
    // ID_List.insert(insert_index, info_id);
    auto& playFileIds = MusicDatabase::obj().playFileInfoIds;
    playFileIds.insert(insert_index, info_id);
}

void MusicWidget::addPlayMusic(const QList<QString>& path)
{
    QList<int> info_ids;
    int insert_index = qMax(YT_AudioOutput::getObject().playIndex + 1, 0);

    for (auto it : path)
    {
        // info_ids += Music_DataBase::getObject().addItemInfo(it);
        info_ids += MusicDatabase::obj().addFileInfo(it);
    }

    // auto& ID_List = Music_DataBase::getObject().getPlayListInfo().ID_List;
    // ID_List.insert(ID_List.begin() + insert_index, info_ids.count(), 0); // 占位
    // std::copy(info_ids.begin(), info_ids.end(), ID_List.begin() + insert_index);
    auto& playFileIds = MusicDatabase::obj().playFileInfoIds;
    playFileIds.insert(playFileIds.begin() + insert_index, info_ids.count(), 0); // 占位
    std::copy(info_ids.begin(), info_ids.end(), playFileIds.begin() + insert_index);
}

void MusicWidget::setCurPath(const QString& data)
{
    static const QRegularExpression re(R"([/\\])");
    auto curPath = data.split(re);
    curPath.removeAll("");
    setCurPath(curPath);
}

void MusicWidget::setCurPath(const QList<QString>& data)
{
    if (data.isEmpty())
        return;

    const auto info_id = data[0];
    // if (Music_DataBase::getObject().isListInfo(info_id) == false)
    //     return;
    if (MusicDatabase::obj().getListInfo(info_id).Name.isEmpty())
        return;

    setCurListInfoID(info_id);
    for (int i = 1; i < data.count(); i++)
    {
        addCurPath(data[i]);
    }
}

void MusicWidget::addCurPath(const QString& data)
{
    curPath.append(data);
    // const auto &&infoID_List = Music_DataBase::getObject().searchItemInfo(data, itemModel.infoID_List);
    // itemModel.setInfoID_List(infoID_List);
    itemModel.searchInfoIds(data);
    emit curPathChanged();
}

QList<QString> MusicWidget::getSubPath(const QList<QString>& data) const
{
    return {};
}

QString MusicWidget::getCurListInfoID() const
{
    return curListInfoID;
}

void MusicWidget::setCurListInfoID(const QString& data)
{
    curListInfoID = data;
    emit curListInfoIDChanged();

    itemModel.setInfoIds(curListInfoID);

    setCurPlayIndex(YT_AudioOutput::getObject().source().toString());

    curPath = { curListInfoID };
    emit curPathChanged();
}

int MusicWidget::getCurPlayIndex() const
{
    return curPlayIndex;
}

void MusicWidget::setCurPlayIndex(const QString& data)
{
    // if (curListInfoID == Music_DataBase::getObject().playListInfoID)
    // {
    //     curPlayIndex = YT_AudioOutput::getObject().playIndex;
    // }
    // else
    // {
    //     int infoID = Music_DataBase::getObject().findItemInfo(data);
    //     curPlayIndex = itemModel.infoIds.indexOf(infoID);
    // }

    if (curListInfoID == MusicDatabase::obj().playFileInfoIdsName) {
        curPlayIndex = YT_AudioOutput::getObject().playIndex;
    } else {
        int info_id = MusicDatabase::obj().getFileInfo_ID(data);
        curPlayIndex = itemModel.infoIds.indexOf(info_id);
    }

    emit curPlayIndexChanged();
}
