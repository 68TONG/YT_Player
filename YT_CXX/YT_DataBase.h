#ifndef YT_DATABASE_H
#define YT_DATABASE_H

#include <YT_GeneralH.h>

#define PROPERTY_ID "ID"
#define PROPERTY_Name "Name"
#define PROPERTY_Path "Path"
#define PROPERTY_ID_List "ID_List"

class YT_ItemInfo {
public:
    YT_ItemInfo() {}
    YT_ItemInfo(int ID, QString Path)
        : ID(ID), Path(Path) {}

    virtual QJsonObject toJsonObject() {
        QJsonObject json;
        json.insert(PROPERTY_ID, ID);
        json.insert(PROPERTY_Path, Path);
        return json;
    }
    virtual void fromJsonObject(const QJsonObject &json) {
        ID = json.value(PROPERTY_ID).toInt(-1);
        Path = json.value(PROPERTY_Path).toString("");
    }

    int ID = -1;
    QString Path = "";
};

class YT_ListInfo {
public:
    YT_ListInfo() {}
    YT_ListInfo(int ID, QString Name, QList<int> ID_List)
        : ID(ID), Name(Name), ID_List(ID_List) {}

    virtual QJsonObject toJsonObject()
    {
        QJsonObject json;

        json.insert(PROPERTY_ID, ID);
        json.insert(PROPERTY_Name, Name);

        QJsonArray json_ID_List;
        for(auto &&it : ID_List) json_ID_List.push_back(QJsonValue(it));
        json.insert(PROPERTY_ID_List, json_ID_List);

        return json;
    }
    virtual void fromJsonObject(const QJsonObject &json)
    {
        ID = json.value(PROPERTY_ID).toInt(-1);
        Name = json.value(PROPERTY_Name).toString("");
        QJsonArray json_ID_List = json.value(PROPERTY_ID_List).toArray();
        for(auto &&it : json_ID_List) ID_List.push_back(it.toInt());
    }

    int ID = -1;
    QString Name = "";
    QList<int> ID_List = {};
};

template<typename ItemInfo_T = YT_ItemInfo, typename ListInfo_T = YT_ListInfo>
class YT_DataBase
{
    static_assert(std::is_base_of<YT_ItemInfo, ItemInfo_T>::value, "ItemInfo_T must inherit from Base");
    static_assert(std::is_base_of<YT_ListInfo, ListInfo_T>::value, "ListInfo_T must inherit from Base");
protected:
    YT_DataBase(QString itemFile_Path, QString listFile_Path)
        : itemFile_Path(itemFile_Path)
        , listFile_Path(listFile_Path)
    {
        initItem_DataFile();
        initList_DataFile();
    }

    ~YT_DataBase()
    {
        // if(is_save == false) return ;
        saveItem_DataFile();
        saveList_DataFile();
    }
public:
    // ItemInfo_T
    bool isItemInfo(int info_id)
    {
        return (itemInfo_Hash.find(info_id) != itemInfo_Hash.end());
    }

    ItemInfo_T getItemInfo(int info_id)
    {
        if(itemInfo_Hash.find(info_id) != itemInfo_Hash.end()){
            return itemInfo_Hash[info_id];
        }
        return ItemInfo_T();
    }

    int addItemInfo(ItemInfo_T &info)
    {
        if(eraseItemInfo_Ids.isEmpty()){
            info.ID = itemInfo_Hash.count();
        } else {
            info.ID = eraseItemInfo_Ids.front();
            eraseItemInfo_Ids.pop_front();
        }

        is_save = true;
        itemInfo_Hash.insert(info.ID, info);
        return info.ID;
    }

    int setItemInfo(const ItemInfo_T &info)
    {
        if(itemInfo_Hash.find(info.ID) == itemInfo_Hash.end()) return -1;

        itemInfo_Hash[info.ID] = info;
        is_save = true;
        return info.ID;
    }

    int eraseItemInfo(int info_id)
    {
        if(itemInfo_Hash.find(info_id) == itemInfo_Hash.end()) return -1;

        for(auto &list_info : listInfo_Hash){
            eraseList_Info_Item_ID(list_info.ID, info_id);
        }

        itemInfo_Hash.remove(info_id);
        eraseItemInfo_Ids.push_back(info_id);

        is_save = true;
        return 0;
    }

    int findItemInfo(const QString &path)
    {
        auto &&data = QDir(path).canonicalPath();
        for (auto &&info : itemInfo_Hash) {
            if (data == info.Path) return info.ID;
            // qDebug() << QDir(path).canonicalPath() << QDir(info.Path).canonicalPath();
            // if (data == QDir(info.Path).canonicalPath()) return info.ID;
        }
        return -1;
    }

    // ListInfo_T
    bool isListInfo(int info_id)
    {
        return (listInfo_Hash.find(info_id) != listInfo_Hash.end());
    }

    ListInfo_T getListInfo(int info_id)
    {
        if(listInfo_Hash.find(info_id) != listInfo_Hash.end()){
            return listInfo_Hash[info_id];
        }
        return ListInfo_T();
    }

    ListInfo_T *getListInfoPtr(int info_id)
    {
        if (listInfo_Hash.find(info_id) != listInfo_Hash.end()) {
            return &listInfo_Hash[info_id];
        }
        return NULL;
    }

    int addListInfo(ListInfo_T &info)
    {
        if (info.Name == "") return -1;
        if (findListInfo(info.Name) >= 0) return -2;

        is_save = true;
        info.ID = listInfo_Hash.count();
        listInfo_Hash.insert(info.ID, info);
        return info.ID;
    }

    int setListInfo(const ListInfo_T &info)
    {
        if (listInfo_Hash.find(info.ID) == listInfo_Hash.end()) return -1;

        listInfo_Hash[info.ID] = info;
        is_save = true;
        return 0;
    }

    int eraseListInfo(int info_id)
    {
        if (listInfo_Hash.find(info_id) == listInfo_Hash.end()) return -1;

        int info_count = listInfo_Hash.count();
        for (int i = info_id; i < (info_count - 1); i++) {
            auto &info = listInfo_Hash[i+1];
            info.ID = i;

            listInfo_Hash[i] = info;
        }
        listInfo_Hash.remove((info_count - 1));
        is_save = true;
        return 0;
    }

    int findListInfo(const QString &name)
    {
        for (auto &&info : listInfo_Hash) {
            if(name == info.Name) return info.ID;
        }
        return -1;
    }

    QList<int> getListInfo_ID_ALL()
    {
        return listInfo_Hash.keys();
    }

    QString getListInfo_Name(int info_id)
    {
        if(listInfo_Hash.find(info_id) != listInfo_Hash.end()){
            return listInfo_Hash[info_id].Name;
        }
        return QString("");
    }

    // ItemInfo_T ListInfo_T
    int addListInfo_ItemID(int list_id, int item_id)
    {
        if(listInfo_Hash.find(list_id) == listInfo_Hash.end()){
            return -1;
        }
        if(itemInfo_Hash.find(item_id) == itemInfo_Hash.end()){
            return -2;
        }

        auto &ID_List = listInfo_Hash[list_id].ID_List;
        if(ID_List.contains(item_id)) return 0;

        is_save = true;
        ID_List.push_back(item_id);
        return 0;
    }

    int eraseListInfo_ItemID(int list_id, int item_id)
    {
        if(listInfo_Hash.find(list_id) == listInfo_Hash.end()){
            return -1;
        }

        auto &ID_List = listInfo_Hash[list_id].ID_List;
        for(auto it = ID_List.begin();it != ID_List.end();){
            if(*it == item_id){
                it = ID_List.erase(it);
                is_save = true;
            } else it++;
        }

        return 0;
    }

    int eraseListInfo_ItemIndex(int list_id, int item_index)
    {
        if(listInfo_Hash.find(list_id) == listInfo_Hash.end()){
            return -1;
        }

        auto &ID_List = listInfo_Hash[list_id].ID_List;
        if(item_index < 0 || item_index >= ID_List.count()) return -1;

        ID_List.remove(item_index);
        is_save = true;

        return 0;
    }
private:
    int initItem_DataFile()
    {
        const QJsonObject json = readJsonObjectFromFile(itemFile_Path);
        if (json.isEmpty()) return -1;

        QJsonArray erase_list = json.value(EraseItemInfo_Ids_Index).toArray();
        for (auto &&it : erase_list) {
            eraseItemInfo_Ids.push_back(it.toInt());
        }

        for (auto &&it : json) {
            auto info_json = it.toObject();
            if(info_json.isEmpty()) continue;

            ItemInfo_T info;
            info.fromJsonObject(info_json);
            itemInfo_Hash.insert(info.ID, info);
        }
        return 0;
    }

    int initList_DataFile()
    {
        const QJsonObject json = readJsonObjectFromFile(listFile_Path);
        if (json.isEmpty()) return -1;

        for (auto &&it : json) {
            auto info_json = it.toObject();
            if(info_json.isEmpty()) continue;

            ListInfo_T info;
            info.fromJsonObject(info_json);
            listInfo_Hash.insert(info.ID, info);
        }
        return 0;
    }

    int saveItem_DataFile()
    {
        QJsonObject json;
        QJsonArray erase_list;
        for (;eraseItemInfo_Ids.isEmpty() == false;) {
            erase_list.append(QJsonValue(eraseItemInfo_Ids.front()));
            eraseItemInfo_Ids.pop_front();
        }
        json.insert(EraseItemInfo_Ids_Index, erase_list);

        for (auto &info : itemInfo_Hash) {
            auto info_json = info.toJsonObject();
            json.insert(QString::number(info.ID), info_json);
        }

        writeJsonObjectToFile(itemFile_Path, json);
        return 0;
    }

    int saveList_DataFile()
    {
        QJsonObject json;
        for (auto &info : listInfo_Hash) {
            auto info_json = info.toJsonObject();
            json.insert(QString::number(info.ID), info_json);
        }

        writeJsonObjectToFile(listFile_Path, json);
        return 0;
    }
protected:

    bool is_save = false;
    QQueue<int> eraseItemInfo_Ids;
    QMap<int, ListInfo_T> listInfo_Hash;
    QHash<int, ItemInfo_T> itemInfo_Hash;

    const QString itemFile_Path;
    const QString listFile_Path;
    const QString EraseItemInfo_Ids_Index = "EraseItem_Info_Ids";
};

#endif // YT_DATABASE_H
