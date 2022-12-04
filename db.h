#ifndef _DB_H
#define _DB_H

#include <pthread.h>
#include "dal.h"
#include "tx.h"

class Tx;
class Dal;

class DB
{
public:
    DB(Dal* d);
    void Close();
    Tx* ReadTx();
    Tx* WriteTx();
public:
    pthread_rwlock_t* RWMutex;
    Dal* dal;
};

#endif