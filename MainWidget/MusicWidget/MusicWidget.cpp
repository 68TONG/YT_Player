#include "MusicWidget.h"

#include "YT_AudioOutput.h"
#include <QRandomGenerator>

MusicWidget::MusicWidget(QQuickItem *parent)
    : QQuickItem{parent}
    , itemModel(*this)
    , listModel(*this)
{
    this->setFlag(QQuickItem::ItemHasContents, true);

    itemModel.setInfoID_List(curListInfoID);
    curPath = {Music_DataBase::getObject().getListInfo_Name(curListInfoID)};

    connect(this, &MusicWidget::curListInfoIDChanged, this, [this](){
        itemModel.setInfoID_List(curListInfoID);

        setCurPlayIndex(YT_AudioOutput::getObject().source().toString());

        curPath = {Music_DataBase::getObject().getListInfo_Name(curListInfoID)};
        emit curPathChanged();
    });
    connect(&itemModel, &MusicItemModel::infoID_ListChanged, this, [this](){
        setCurPlayIndex(YT_AudioOutput::getObject().source().toString());

        itemHeaderModel = Music_DataBase::getObject().getListInfo_HeaderALL(itemModel.infoID_List);
        emit itemHeaderModelChanged();
    });
    connect(&YT_AudioOutput::getObject(), &YT_AudioOutput::sourceChanged, this, [this](const QUrl &media){
        setCurPlayIndex(media.toString());
    });
}

void MusicWidget::playMusic(int ids_index)
{
    auto &play_info = Music_DataBase::getObject().getPlayListInfo();
    const auto playModel = YT_AudioOutput::getObject().playModel;
    if (play_info.ID == curListInfoID) {
        YT_AudioOutput::getObject().playIndex = ids_index - 1;
    } else if (playModel == "qrc:/Resource_UI/shuffle.png") {
        const auto &infoID_List = itemModel.infoID_List;

        play_info.ID_List.clear();
        play_info.ID_List.append(infoID_List[ids_index]);
        for (int i = 0; i < infoID_List.count(); ++i) {
            int index = QRandomGenerator::global()->bounded(infoID_List.count());
            play_info.ID_List.append(infoID_List[index]);
        }
        YT_AudioOutput::getObject().playIndex = 0 - 1;
    } else if (playModel == "qrc:/Resource_UI/single_loop.png") {
        play_info.ID_List = {itemModel.infoID_List[ids_index]};
        YT_AudioOutput::getObject().playIndex = 0 - 1;
    } else if (playModel == "qrc:/Resource_UI/list_loop.png") {
        play_info.ID_List = itemModel.infoID_List;
        YT_AudioOutput::getObject().playIndex = ids_index - 1;
    }

    YT_AudioOutput::getObject().playNextMusic();
}

void MusicWidget::addPlayMusic(const QString &path)
{
    int info_id = Music_DataBase::getObject().addItemInfo_Path(path);
    auto &play_info = Music_DataBase::getObject().getPlayListInfo();
    int insert_index = qMax(YT_AudioOutput::getObject().playIndex + 1, 0);
    play_info.ID_List.insert(insert_index, info_id);
}

void MusicWidget::setCurPath(const QString &data)
{
    static const QRegularExpression re(R"([/\\])");
    auto curPath = data.split(re);
    curPath.removeAll("");
    setCurPath(curPath);
}

void MusicWidget::setCurPath(const QList<QString> &data)
{
    if (data.isEmpty()) return ;
    const int info_id = Music_DataBase::getObject().findListInfo(data[0]);
    if (info_id < 0) return ;

    setCurListInfoID(info_id);

    for (int i = 1; i < data.count(); i++) {
        static const QRegularExpression re(R"(\(([^)]+)\)([^)]+))");
        QRegularExpressionMatch match = re.match(data[i]);
        if (match.hasMatch()) {
            addPath_FilterInfo(match.captured(2), match.captured(1));
            // qDebug() << match;
        } else {
            addPath_SearchInfo(data[i]);
            // qDebug() << data[i];
        }
    }
}

void MusicWidget::addPath_FilterInfo(const QString &data, const QString &header)
{
    curPath.append("(" + header + ")" + data);
    auto &&infoID_List = Music_DataBase::getObject().getItemInfo_Search(data, {header}, itemModel.infoID_List);
    itemModel.setInfoID_List(infoID_List);
    emit curPathChanged();
}

void MusicWidget::addPath_SearchInfo(const QString &data)
{
    curPath.append(data);
    auto &&infoID_List = Music_DataBase::getObject().getItemInfo_Search(data, itemModel.infoID_List);
    itemModel.setInfoID_List(infoID_List);
    emit curPathChanged();
}

QList<QString> MusicWidget::getSubPath(const QList<QString> &data) const
{
    return itemHeaderModel;
}

int MusicWidget::getCurListInfoID() const
{
    return curListInfoID;
}

void MusicWidget::setCurListInfoID(const int &data)
{
    curListInfoID = data;
    emit curListInfoIDChanged();
}

int MusicWidget::getCurPlayIndex() const
{
    return curPlayIndex;
}

void MusicWidget::setCurPlayIndex(const QString &path)
{
    const auto &info = Music_DataBase::getObject().getPlayListInfo();
    if (curListInfoID == info.ID) {
        curPlayIndex = YT_AudioOutput::getObject().playIndex;
    } else {
        int info_id = Music_DataBase::getObject().findItemInfo(path);
        curPlayIndex = itemModel.infoID_List.indexOf(info_id);
    }
    emit curPlayIndexChanged();
}
