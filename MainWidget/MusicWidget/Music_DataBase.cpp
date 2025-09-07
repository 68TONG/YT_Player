#include "Music_DataBase.h"

#include <QLocale>
#include <QCollator>

Music_DataBase::Music_DataBase()
    : YT_DataBase<Music_ItemInfo, Music_ListInfo>("./YT_OutData/Music_ItemInfo.json", "./YT_OutData/Music_ListInfo.json")
{
    if (listInfo_Hash.isEmpty())
        listInfo_Hash.insert(listInfo_Hash.count(), Music_ListInfo{(int)listInfo_Hash.count(), "默认列表", QList<int>()});
    if (findListInfo("播放列表") < 0)
        listInfo_Hash.insert(listInfo_Hash.count(), Music_ListInfo{(int)listInfo_Hash.count(), "播放列表", QList<int>()});
}

Music_DataBase &Music_DataBase::getObject()
{
    static Music_DataBase obj;
    return obj;
}

int Music_DataBase::eraseListInfo(int info_id)
{
    if (listInfo_Hash.find(info_id) == listInfo_Hash.end()) return -1;
    if (listInfo_Hash[info_id].Name == "播放列表") return -1;

    int sic = YT_DataBase::eraseListInfo(info_id);
    if (listInfo_Hash.count() == 1) {
        listInfo_Hash.insert(listInfo_Hash.count(), Music_ListInfo{(int)listInfo_Hash.count(), "默认列表", QList<int>()});
    }
    return sic;
}

int Music_DataBase::addListInfo_ItemPath(int list_id, const QString &path)
{
    int item_id = addItemInfo_Path(path);
    if (item_id < 0) return item_id;
    addListInfo_ItemID(list_id, item_id);
    return item_id;
}

int Music_DataBase::addItemInfo_Path(const QString &path)
{
    int info_id = findItemInfo(path);
    if (info_id < 0) {
        Music_ItemInfo info;
        int sic = info.fromMusicFile(path);
        if (sic < 0) return -1;
        info_id = addItemInfo(info);
    }
    return info_id;
}

int Music_DataBase::updateItemInfo(const QString &path)
{
    int info_id = findItemInfo(path);
    if (info_id < 0) return info_id;

    Music_ItemInfo info(info_id, path);
    if (info.Path.isEmpty()) return -true;

    itemInfo_Hash[info.ID] = info;
    is_save = true;
    return info.ID;
}

void Music_DataBase::sortListInfo(int info_id, const QString &sort_index)
{
    if (isListInfo(info_id) == false) return ;
    sortItemInfo(listInfo_Hash[info_id].ID_List, sort_index);
    is_save = true;
}

void Music_DataBase::sortItemInfo(QList<int> &ID_List, const QString &sort_index)
{
    if(sort_index == PROPERTY_ID){
        std::reverse(ID_List.begin(), ID_List.end());
        return ;
    }

    // if(sort_index == MusicItem_Info::getEnumName(MusicItem_Info::duration_Enum)){
    //     std::stable_sort(info_id_list.begin(), info_id_list.end(), [this](const int &v1, const int &v2){
    //         return ItemInfo_list[v1].duration > ItemInfo_list[v2].duration;
    //     });
    //     return info_id_list;
    // }

    QCollator collator;
    QMap<int, QString> sort_container;
    // collator.setNumericMode(true);
    collator.setLocale(QLocale(QLocale::Chinese, QLocale::China));
    for (auto &&it : ID_List) {
        auto data = itemInfo_Hash[it].musicData[sort_index].toArray();
        if(data.isEmpty()) {
            sort_container.insert(it, "");
        } else {
            sort_container.insert(it, data.at(0).toString());
        }
    }
    std::stable_sort(ID_List.begin(), ID_List.end(), [this, sort_container, collator](const int &v1, const int &v2){
        return (collator.compare(sort_container[v1], sort_container[v2]) < 0);
    });
}

Music_ListInfo &Music_DataBase::getPlayListInfo()
{
    return listInfo_Hash[findListInfo("播放列表")];
}

QList<QString> Music_DataBase::getListInfo_NameALL()
{
    QList<QString> ret_;
    for (auto &&info : listInfo_Hash) {
        ret_.append(info.Name);
    }
    return ret_;
}

QList<QString> Music_DataBase::getListInfo_HeaderALL(int info_id)
{
    if (isListInfo(info_id) == false) return {};
    return getListInfo_HeaderALL(listInfo_Hash[info_id].ID_List);
}

QList<QString> Music_DataBase::getListInfo_HeaderALL(const QList<int> &info_ids)
{
    QSet<QString> set_;
    QList<QString> list_;
    for (auto &&it : info_ids) {
        const auto &musicData = itemInfo_Hash[it].musicData;
        for (auto musicData_it = musicData.begin(); musicData_it != musicData.end(); musicData_it++) {
            set_.insert(musicData_it.key());
        }
    }
    std::copy(set_.begin(), set_.end(), std::back_inserter(list_));
    return list_;
}

QList<QString> Music_DataBase::getListInfo_HeaderDataALL(int info_id, QString index)
{
    if (isListInfo(info_id) == false) return {};
    return getListInfo_HeaderDataALL(listInfo_Hash[info_id].ID_List, index);
}

QList<QString> Music_DataBase::getListInfo_HeaderDataALL(const QList<int> &info_ids, QString index)
{
    QSet<QString> set_;
    QList<QString> list_;
    for (auto &&it : info_ids) {
        const auto &headerDataArray = itemInfo_Hash[it].musicData[index].toArray();
        for (auto &&headerDataArray_it : headerDataArray) {
            auto headerData = headerDataArray_it.toString("");
            if (headerData.isEmpty()) continue;
            set_.insert(headerData);
        }
    }
    std::copy(set_.begin(), set_.end(), std::back_inserter(list_));
    return list_;
}

QList<int> Music_DataBase::getItemInfo_KMP(const QString &kmp_data, const QList<QString> &kmp_indexs)
{
    QList<int> list_;
    for(const auto &info : itemInfo_Hash){
        for(const auto &kmp_index : kmp_indexs){
            const auto &array = info.musicData[kmp_index].toArray();
            for(const auto &array_it : array){
                if(KMP_String(array_it.toString(""), kmp_data) < 0) continue;
                list_.append(info.ID);
            }
        }
    }
    return list_;
}

QList<int> Music_DataBase::getItemInfo_KMP(const QString &kmp_data, const QList<QString> &kmp_indexs, int info_id)
{
    if (isListInfo(info_id) == false) return {};

    QList<int> list_;
    const auto &ID_List = listInfo_Hash[info_id].ID_List;
    for(const auto &id : ID_List){
        const auto &info = itemInfo_Hash[id];
        for(const auto &kmp_index : kmp_indexs){
            const auto &array = info.musicData[kmp_index].toArray();
            for(const auto &array_it : array){
                if(KMP_String(array_it.toString(""), kmp_data) < 0) continue;
                list_.append(info.ID);
            }
        }
    }
    return list_;
}

QList<int> Music_DataBase::getItemInfo_Filter(const QString &data, const QList<QString> &indexs, const QList<int> &ID_List)
{
    QList<int> list_;
    for (const auto &id : ID_List) {
        const auto &info = itemInfo_Hash[id];
        for (const auto &index : indexs) {
            const auto &array = info.musicData[index].toArray();
            for (const auto &array_it : array) {
                if(KMP_String(array_it.toString(""), data) < 0) continue;
                // if (array_it.toString("") != data) continue;
                list_.append(info.ID);
            }
        }
    }
    return list_;
}

QList<int> Music_DataBase::getItemInfo_Search(const QString &data, const QList<int> &ID_List) const
{
    QList<int> list_;
    for (const auto &id : ID_List) {
        const auto &info = itemInfo_Hash[id];
        for (const auto &musicData_it: info.musicData) {
            const auto &array = musicData_it.toArray();
            for (const auto &array_it : array) {
                if(KMP_String(array_it.toString(""), data) < 0) continue;
                list_.append(info.ID);
            }
        }
    }
    return list_;
}

QList<int> Music_DataBase::getItemInfo_Search(const QString &data, const QList<QString> &indexs, const QList<int> &ID_List) const
{
    QList<int> list_;
    for (const auto &id : ID_List) {
        const auto &info = itemInfo_Hash[id];
        for (const auto &index : indexs) {
            const auto &array = info.musicData[index].toArray();
            for (const auto &array_it : array) {
                if(KMP_String(array_it.toString(""), data) < 0) continue;
                list_.append(info.ID);
            }
        }
    }
    return list_;
}


