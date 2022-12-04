#ifndef _TX_H
#define _TX_H

#include "defs.h"
#include "node.h"
#include "db.h"
#include "collection.h"
#include <unordered_map>
#include <vector>

class Node;
class DB;

class Tx
{
public:
    Tx(DB* d,bool w);
    Node* writeNode(Node* node);
    Node* getNode(PageNum pagenum);
    void Rollback();
    void Commit();
    Collection* getRootCollection();
    Collection* getCollection(char* name);
public:
    std::unordered_map<PageNum,Node*> dirtyNodes;//修改过的页面
    std::vector<PageNum> pagesToDelete;
    std::vector<PageNum> allocatedPageNums;//新分配的页面
    bool write;//是否可写（非只读）
    DB* db;
};

#endif