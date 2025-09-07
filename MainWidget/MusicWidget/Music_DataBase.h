#ifndef MUSIC_DATABASE_H
#define MUSIC_DATABASE_H

#include <Music_DataInfo.h>

class Music_DataBase : public YT_DataBase<Music_ItemInfo, Music_ListInfo>
{
    Music_DataBase();
public:
    static Music_DataBase &getObject();

    int eraseListInfo(int info_id);

    int addListInfo_ItemPath(int list_id, const QString &path);

    int addItemInfo_Path(const QString &path);
    int updateItemInfo(const QString &path);
    void sortListInfo(int info_id, const QString &sort_index);
    void sortItemInfo(QList<int> &ID_List, const QString &sort_index);

    Music_ListInfo &getPlayListInfo();

    QList<QString> getListInfo_NameALL();
    QList<QString> getListInfo_HeaderALL(int info_id);
    QList<QString> getListInfo_HeaderALL(const QList<int> &info_ids);
    QList<QString> getListInfo_HeaderDataALL(int info_id, QString index);
    QList<QString> getListInfo_HeaderDataALL(const QList<int> &info_ids, QString index);

    QList<int> getItemInfo_KMP(const QString &kmp_data, const QList<QString> &kmp_indexs);
    QList<int> getItemInfo_KMP(const QString &kmp_data, const QList<QString> &kmp_indexs, int info_id);
    QList<int> getItemInfo_Filter(const QString &data, const QList<QString> &indexs, const QList<int> &ID_List);

    QList<int> getItemInfo_Search(const QString &data, const QList<int> &ID_List) const;
    QList<int> getItemInfo_Search(const QString &data, const QList<QString> &indexs, const QList<int> &ID_List) const;
private:
};

#endif // MUSIC_DATABASE_H
