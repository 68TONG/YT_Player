#pragma once

#include <DB_Database.h>

class DB_DatabaseInstance : public DB_Database<DB_FileInfo, DB_ListInfo>
{
    DB_DatabaseInstance();
    ~DB_DatabaseInstance();
public:
    static DB_DatabaseInstance& obj();
};

