#ifndef _DB_H
#define _DB_H

#include <pthread.h>
#include "dal.h"

class DB
{
public:
    DB(Dal* d);
    void Close();

public:
    pthread_rwlock_t* RWMutex;
    Dal* dal;
};

#endif