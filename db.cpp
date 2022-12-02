#include "db.h"
#include <unistd.h>

DB* Open(const char* path,Options* options)
{
    options->pageSize=getpagesize();
    Dal* dal=new Dal(path,options);

    DB* db=new DB(dal);
    return db;
}

DB::DB(Dal* d):dal(d)
{
    pthread_rwlock_init(RWMutex,NULL);
}

void DB::Close()
{
    delete dal;
}