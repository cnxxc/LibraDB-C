#include "tx.h"
#include <pthread.h>

Tx::Tx(DB* d,bool w):db(d),write(w){}

Node* Tx::writeNode(Node* node)
{
    dirtyNodes[node->pageNum]=node;
    node->tx=this;
    return node;
}

Node* Tx::getNode(PageNum pagenum)
{
    if(dirtyNodes.count(pagenum)!=0)
        return dirtyNodes[pagenum];
    
    Node* node=db->dal->getNode(pagenum);
    node->tx=this;
    return node;
}

void Tx::Rollback()
{
    if(!write)
    {
        pthread_rwlock_unlock(db->RWMutex);
        return;
    }

    dirtyNodes.clear();
    pagesToDelete.clear();
    for(PageNum p:allocatedPageNums)
    {
        db->dal->freelist->releasePage(p);
    }
    allocatedPageNums.clear();
    pthread_rwlock_unlock(db->RWMutex);
}

void Tx::Commit()
{
    if(!write)
    {
        pthread_rwlock_unlock(db->RWMutex);
        return;
    }

    for(auto& m:dirtyNodes)
    {
        db->dal->writeNode(m.second);
    }

    for(PageNum pagenum:pagesToDelete)
    {
        db->dal->deleteNode(pagenum);
    }
    db->dal->writeFreelist();

    dirtyNodes.clear();
    pagesToDelete.clear();
    allocatedPageNums.clear();
    pthread_rwlock_unlock(db->RWMutex);
}

Collection* Tx::getRootCollection()
{
    Collection* rootCollection=new Collection();
    rootCollection->root=db->dal->meta->root;
    rootCollection->tx=this;
    return rootCollection;
}

Collection* Tx::getCollection(char* name)
{

}