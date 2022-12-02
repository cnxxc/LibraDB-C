#ifndef _TX_H
#define _TX_H

#include "defs.h"
#include "node.h"
#include "db.h"
#include <unordered_map>
#include <vector>

class tx
{
public:
    tx(DB* d,bool w);
public:
    std::unordered_map<PageNum,Node*> dirtyNodes;
    std::vector<PageNum> pagesToDelete;
    std::vector<PageNum> allocatedPageNums;
    bool write;
    DB* db;
};

#endif