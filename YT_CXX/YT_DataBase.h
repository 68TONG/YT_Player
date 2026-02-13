#ifndef YT_DATABASE_H
#define YT_DATABASE_H

#include <QLocale>
#include <QCollator>
#include <YT_Info.h>
#include <YT_Func.h>
#include <YT_GeneralH.h>

#define YT_INFO_PROPERTY_ID "ID"
#define YT_INFO_PROPERTY_Name "Name"
#define YT_INFO_PROPERTY_Path "Path"
#define YT_INFO_PROPERTY_Tags "Tags"
#define YT_INFO_PROPERTY_SortTag "SortTag"
#define YT_INFO_PROPERTY_SelectTag "SelectTag"
#define YT_INFO_PROPERTY_ID_List "ID_List"

class YT_ItemInfo
{
public:
    class YT_InfoTag : public QJsonObject
    {
    public:
        using QJsonObject::operator=;
        using QJsonObject::QJsonObject;
        explicit YT_InfoTag(const QJsonObject& o) : QJsonObject(o) {}
        explicit YT_InfoTag(QJsonObject&& o) noexcept : QJsonObject(std::move(o)) {}

        QString firstTag(const QString& index) const
        {
            auto data = this->value(index);
            if (data.isArray())
                return data.toArray().at(0).toString();
            return data.toString();
        }
        QList<QString> listTag(const QString& index) const
        {
            auto data = this->value(index);
            if (data.isArray())
            {
                auto list = data.toArray();
                QList<QString> ret_list = {};
                for (auto it : list)
                {
                    ret_list.append(it.toString());
                }
                return ret_list;
            }
            else if (data.isString())
            {
                return { data.toString() };
            }
            return {};
        }
    };

    YT_ItemInfo() {}
    YT_ItemInfo(int ID, QString Path)
        : ID(ID), Path(Path) {
    }

    virtual QJsonObject toJsonObject()
    {
        QJsonObject json;
        json.insert(YT_INFO_PROPERTY_ID, ID);
        json.insert(YT_INFO_PROPERTY_Path, Path);
        json.insert(YT_INFO_PROPERTY_Tags, Tags);
        return json;
    }
    virtual void fromJsonObject(const QJsonObject& json)
    {
        ID = json.value(YT_INFO_PROPERTY_ID).toInt(-1);
        Path = json.value(YT_INFO_PROPERTY_Path).toString("");
        Tags = json.value(YT_INFO_PROPERTY_Tags).toObject();
    }

    int ID = -1;
    QString Path = {};
    YT_InfoTag Tags = {};
};

class YT_ListInfo
{
public:
    YT_ListInfo() {}
    YT_ListInfo(QString Name, QList<int> ID_List)
        : Name(Name), ID_List(ID_List) {
    }

    virtual QJsonObject toJsonObject()
    {
        QJsonObject json;

        json.insert(YT_INFO_PROPERTY_Name, Name);
        json.insert(YT_INFO_PROPERTY_SortTag, SortTag);

        QJsonArray json_SelectTag;
        for (auto it : SelectTag)
            json_SelectTag.push_back(QJsonValue(it));
        json.insert(YT_INFO_PROPERTY_SelectTag, json_SelectTag);

        QJsonArray json_ID_List;
        for (auto it : ID_List)
            json_ID_List.push_back(QJsonValue(it));
        json.insert(YT_INFO_PROPERTY_ID_List, json_ID_List);

        return json;
    }
    virtual void fromJsonObject(const QJsonObject& json)
    {
        Name = json.value(YT_INFO_PROPERTY_Name).toString("");
        SortTag = json.value(YT_INFO_PROPERTY_SortTag).toString("");

        QJsonArray json_SelectTag = json.value(YT_INFO_PROPERTY_SelectTag).toArray();
        for (auto it : json_SelectTag)
            SelectTag.push_back(it.toString());

        QJsonArray json_ID_List = json.value(YT_INFO_PROPERTY_ID_List).toArray();
        for (auto it : json_ID_List)
            ID_List.push_back(it.toInt());
    }

    QString Name = {};
    QString SortTag = {};
    QList<QString> SelectTag = {};
    QList<int> ID_List = {};
};

template <typename ItemInfo_T = YT_ItemInfo, typename ListInfo_T = YT_ListInfo>
class YT_DataBase
{
    static_assert(std::is_base_of<YT_ItemInfo, ItemInfo_T>::value, "ItemInfo_T must inherit from Base");
    static_assert(std::is_base_of<YT_ListInfo, ListInfo_T>::value, "ListInfo_T must inherit from Base");

protected:
    YT_DataBase(QString itemFile_Path, QString listFile_Path)
        : itemFile_Path(itemFile_Path), listFile_Path(listFile_Path)
    {
        initItem_DataFile();
        initList_DataFile();
    }

    virtual ~YT_DataBase()
    {
        saveItem_DataFile();
        saveList_DataFile();
    }

public:
    // ItemInfo_T
    virtual bool isItemInfo(int info_id)
    {
        return (itemInfo_Hash.find(info_id) != itemInfo_Hash.end());
    }

    virtual ItemInfo_T getItemInfo(int info_id)
    {
        if (itemInfo_Hash.find(info_id) != itemInfo_Hash.end())
        {
            return itemInfo_Hash[info_id];
        }
        return ItemInfo_T();
    }

    virtual int addItemInfo(QString path)
    {
        int info_id = findItemInfo(path);
        if (info_id >= 0)
            return info_id;

        ItemInfo_T info(-1, path);
        return addItemInfo(info);
    }

    virtual int addItemInfo(ItemInfo_T& info)
    {
        if (eraseItemInfo_Ids.isEmpty())
        {
            info.ID = itemInfo_Hash.count();
        }
        else
        {
            info.ID = eraseItemInfo_Ids.front();
            eraseItemInfo_Ids.pop_front();
        }

        itemInfo_Hash.insert(info.ID, info);
        return info.ID;
    }

    virtual int setItemInfo(const ItemInfo_T& info)
    {
        if (itemInfo_Hash.find(info.ID) == itemInfo_Hash.end())
            return -1;

        itemInfo_Hash[info.ID] = info;
        return info.ID;
    }

    virtual int eraseItemInfo(int info_id)
    {
        if (itemInfo_Hash.find(info_id) == itemInfo_Hash.end())
            return -1;

        for (auto& list_info : listInfo_Hash)
        {
            eraseListInfo_ItemID(list_info.Name, info_id);
        }

        itemInfo_Hash.remove(info_id);
        eraseItemInfo_Ids.push_back(info_id);
        return 0;
    }

    virtual int findItemInfo(QString path) const
    {
        auto&& data = QDir(path).canonicalPath();
        for (auto&& info : itemInfo_Hash)
        {
            if (data == info.Path)
                return info.ID;
            // qDebug() << QDir(path).canonicalPath() << QDir(info.Path).canonicalPath();
            // if (data == QDir(info.Path).canonicalPath()) return info.ID;
        }
        return -1;
    }

    virtual QList<int> searchItemInfo(QString tag_data, const QList<int>& ID_List) const
    {
        QList<int> list_;
        for (const auto& id : ID_List)
        {
            const auto& info = itemInfo_Hash[id];
            for (const auto& it_tag : info.Tags)
            {
                bool is = false;
                const auto& tag_data_list = jsonListData(it_tag);
                for (const auto& it : tag_data_list)
                {
                    if (KMP_String(it.toString(), tag_data) < 0)
                        continue;

                    list_.append(info.ID);
                    is = true;
                    break;
                }
                if (is)
                    break;
            }
        }
        return list_;
    }

    virtual QList<int> searchItemInfo(QString tag_data, const QList<QString>& tag_headers, const QList<int>& ID_List) const
    {
        QList<int> list_;
        for (const auto& id : ID_List)
        {
            const auto& info = itemInfo_Hash[id];
            for (const auto& tag_header : tag_headers)
            {
                bool is = false;
                const auto& tag_data_list = jsonListData(info.Tags[tag_header]);
                for (const auto& it : tag_data_list)
                {
                    if (KMP_String(it.toString(), tag_data) < 0)
                        continue;

                    list_.append(info.ID);
                    is = true;
                    break;
                }
                if (is)
                    break;
            }
        }
        return list_;
    }

    // ListInfo_T
    virtual bool isListInfo(QString info_id)
    {
        return (listInfo_Hash.find(info_id) != listInfo_Hash.end());
    }

    virtual ListInfo_T getListInfo(QString info_id)
    {
        if (listInfo_Hash.find(info_id) != listInfo_Hash.end())
        {
            return listInfo_Hash[info_id];
        }
        return ListInfo_T();
    }

    virtual ListInfo_T* getListInfoPtr(QString info_id)
    {
        if (listInfo_Hash.find(info_id) != listInfo_Hash.end())
        {
            return &listInfo_Hash[info_id];
        }
        return nullptr;
    }

    virtual int addListInfo(ListInfo_T& info)
    {
        if (info.Name == "")
            return -1;
        if (listInfo_Hash.find(info.Name) != listInfo_Hash.end())
            return -2;

        listInfo_Hash.insert(info.Name, info);
        return 0;
    }

    virtual int setListInfo(const ListInfo_T& info)
    {
        if (listInfo_Hash.find(info.Name) == listInfo_Hash.end())
            return -1;

        listInfo_Hash[info.Name] = info;
        return 0;
    }

    virtual int setListInfo_Name(const QString &info_id, const QString &name)
    {
        if (listInfo_Hash.find(info_id) == listInfo_Hash.end())
            return -1;

        auto info = listInfo_Hash[info_id];
        info.Name = name;

        listInfo_Hash.remove(info_id);
        listInfo_Hash.insert(info.Name, info);
        return 0;
    }

    virtual int eraseListInfo(QString info_id)
    {
        if (listInfo_Hash.find(info_id) == listInfo_Hash.end())
            return -1;

        listInfo_Hash.remove(info_id);
        return 0;
    }

    virtual void sortListInfo(QString info_id, const QString& sort_data)
    {
        if (listInfo_Hash.find(info_id) == listInfo_Hash.end())
            return;
        if (sort_data != YT_INFO_PROPERTY_ID)
        {
            listInfo_Hash[info_id].SortTag = sort_data;
        }
        sortListInfo(listInfo_Hash[info_id].ID_List, sort_data);
    }

    virtual void sortListInfo(QList<int>& ID_List, const QString& sort_data)
    {
        if (sort_data == YT_INFO_PROPERTY_ID)
        {
            std::reverse(ID_List.begin(), ID_List.end());
            return;
        }

        // if(sort_index == MusicItem_Info::getEnumName(MusicItem_Info::duration_Enum)){
        //     std::stable_sort(info_id_list.begin(), info_id_list.end(), [this](const int &v1, const int &v2){
        //         return ItemInfo_list[v1].duration > ItemInfo_list[v2].duration;
        //     });
        //     return info_id_list;
        // }

        // QCollator collator;
        // collator.setCaseSensitivity(Qt::CaseInsensitive);
        // collator.setNumericMode(true);
        // collator.setLocale(QLocale(QLocale::Chinese, QLocale::China));
        QMap<int, QString> sort_container;
        for (auto&& it : ID_List)
        {
            auto tag_data = itemInfo_Hash[it].Tags.firstTag(sort_data);
            auto data = YT_Func::getObject().getChinesePinYin(tag_data);
            sort_container.insert(it, data);
        }
        std::stable_sort(ID_List.begin(), ID_List.end(), [this, sort_container](const int& v1, const int& v2) -> bool
            { return QString::compare(sort_container[v1], sort_container[v2], Qt::CaseInsensitive) < 0; });
    }

    virtual QList<QString> getListInfo_NameALL()
    {
        return listInfo_Hash.keys();
    }

    virtual QList<QString> getListInfo_TagHeaders(QString info_id)
    {
        if (listInfo_Hash.find(info_id) == listInfo_Hash.end())
            return {};
        return getListInfo_TagHeaders(listInfo_Hash[info_id].ID_List);
    }

    virtual QList<QString> getListInfo_TagHeaders(const QList<int>& info_ids)
    {
        QSet<QString> set_;
        QList<QString> list_;
        for (const auto& it : info_ids)
        {
            const auto& data = itemInfo_Hash[it].Tags;
            for (auto it_data = data.begin(); it_data != data.end(); it_data++)
            {
                set_.insert(it_data.key());
            }
        }
        std::copy(set_.begin(), set_.end(), std::back_inserter(list_));
        return list_;
    }

    virtual QList<QString> getListInfo_TagDatas(QString info_id, QString index)
    {
        if (listInfo_Hash.find(info_id) == listInfo_Hash.end())
            return {};
        return getListInfo_TagDatas(listInfo_Hash[info_id].ID_List, index);
    }

    virtual QList<QString> getListInfo_TagDatas(const QList<int>& info_ids, QString index)
    {
        QSet<QString> set_;
        QList<QString> list_;
        for (const auto& it : info_ids)
        {
            const auto& data = itemInfo_Hash[it].Tags.listTag(index);
            for (auto&& it_data : data)
            {
                if (it_data.isEmpty())
                    continue;
                set_.insert(it_data);
            }
        }
        std::copy(set_.begin(), set_.end(), std::back_inserter(list_));
        return list_;
    }

    // ItemInfo_T ListInfo_T
    virtual int addListInfo_ItemID(QString list_id, int item_id)
    {
        if (listInfo_Hash.find(list_id) == listInfo_Hash.end())
        {
            return -1;
        }
        if (itemInfo_Hash.find(item_id) == itemInfo_Hash.end())
        {
            return -2;
        }

        auto& ID_List = listInfo_Hash[list_id].ID_List;
        if (ID_List.contains(item_id))
            return 0;

        ID_List.push_back(item_id);
        return 0;
    }

    virtual int addListInfo_ItemPath(QString list_id, QString path)
    {
        int item_id = addItemInfo(path);
        if (item_id < 0)
            return item_id;
        if (addListInfo_ItemID(list_id, item_id) < 0)
            return -1;
        return item_id;
    }

    virtual int eraseListInfo_ItemID(QString list_id, int item_id)
    {
        if (listInfo_Hash.find(list_id) == listInfo_Hash.end())
        {
            return -1;
        }

        auto& ID_List = listInfo_Hash[list_id].ID_List;
        for (auto it = ID_List.begin(); it != ID_List.end();)
        {
            if (*it == item_id)
            {
                it = ID_List.erase(it);
            }
            else
                it++;
        }

        return 0;
    }

    virtual int eraseListInfo_ItemIndex(QString list_id, int item_index)
    {
        if (listInfo_Hash.find(list_id) == listInfo_Hash.end())
        {
            return -1;
        }

        auto& ID_List = listInfo_Hash[list_id].ID_List;
        if (item_index < 0 || item_index >= ID_List.count())
            return -1;

        ID_List.remove(item_index);
        return 0;
    }

private:
    int initItem_DataFile()
    {
        const QJsonObject json = readJsonObjectFromFile(itemFile_Path);
        if (json.isEmpty())
            return -1;

        QJsonArray erase_list = json.value(EraseItemInfo_Ids_Index).toArray();
        for (auto&& it : erase_list)
        {
            eraseItemInfo_Ids.push_back(it.toInt());
        }

        for (auto&& it : json)
        {
            auto info_json = it.toObject();
            if (info_json.isEmpty())
                continue;

            ItemInfo_T info;
            info.fromJsonObject(info_json);
            itemInfo_Hash.insert(info.ID, info);
        }
        return 0;
    }

    int initList_DataFile()
    {
        const QJsonObject json = readJsonObjectFromFile(listFile_Path);
        if (json.isEmpty())
            return -1;

        for (auto&& it : json)
        {
            auto info_json = it.toObject();
            if (info_json.isEmpty())
                continue;

            ListInfo_T info;
            info.fromJsonObject(info_json);
            listInfo_Hash.insert(info.Name, info);
        }
        return 0;
    }

    int saveItem_DataFile()
    {
        QJsonObject json;
        QJsonArray erase_list;
        for (; eraseItemInfo_Ids.isEmpty() == false;)
        {
            erase_list.append(QJsonValue(eraseItemInfo_Ids.front()));
            eraseItemInfo_Ids.pop_front();
        }
        json.insert(EraseItemInfo_Ids_Index, erase_list);

        for (auto& info : itemInfo_Hash)
        {
            auto info_json = info.toJsonObject();
            json.insert(QString::number(info.ID), info_json);
        }

        writeJsonObjectToFile(itemFile_Path, json);
        return 0;
    }

    int saveList_DataFile()
    {
        QJsonObject json;
        for (auto& info : listInfo_Hash)
        {
            auto info_json = info.toJsonObject();
            json.insert(info.Name, info_json);
        }

        writeJsonObjectToFile(listFile_Path, json);
        return 0;
    }

protected:
    QQueue<int> eraseItemInfo_Ids;
    QHash<int, ItemInfo_T> itemInfo_Hash;
    QHash<QString, ListInfo_T> listInfo_Hash;

    const QString itemFile_Path;
    const QString listFile_Path;
    const QString EraseItemInfo_Ids_Index = "EraseItem_Info_Ids";
};

#endif // YT_DATABASE_H
