#include "DB_DatabaseInstance.h"

DB_DatabaseInstance::DB_DatabaseInstance()
{
    {}
}

DB_DatabaseInstance::~DB_DatabaseInstance()
{
    {}
}

DB_DatabaseInstance& DB_DatabaseInstance::obj()
{
    static DB_DatabaseInstance obj;
    return obj;
}
