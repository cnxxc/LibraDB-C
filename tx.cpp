#include "tx.h"
#include "error.h"
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
        pthread_rwlock_unlock(&db->RWMutex);
        return;
    }

    dirtyNodes.clear();
    pagesToDelete.clear();
    for(PageNum p:allocatedPageNums)
    {
        db->dal->freelist->releasePage(p);
    }
    allocatedPageNums.clear();
    pthread_rwlock_unlock(&db->RWMutex);
}

void Tx::Commit()
{
    if(!write)
    {
        pthread_rwlock_unlock(&db->RWMutex);
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
    pthread_rwlock_unlock(&db->RWMutex);
}

Collection* Tx::getRootCollection()
{
    Collection* rootCollection=new Collection();
    rootCollection->root=db->dal->meta->root;//root是rootCollection的数据根结点，类比Collection.root,rootCollection代表整个Collection树而不是树的根结点
    rootCollection->tx=this;
    return rootCollection;
}

Collection* Tx::getCollection(const char* name)
{
    Collection* rootCollection=getRootCollection();
    Item* item=rootCollection->Find(name);//这里返回的其实是Collection对象

    if(item==NULL) return NULL;

    Collection* collection=new Collection();
    collection->deserialize(item);
    collection->tx=this;
    return collection;
}

Collection* Tx::createCollection(Collection* collection)
{
    collection->tx=this;
    Item* collectionBytes=collection->serialize();

    Collection* rootCollection=getRootCollection();
    rootCollection->Put(collection->name,collectionBytes->value);
    return collection;
}

Collection* Tx::createCollection(const char* name)
{
    if(!write) return NULL;

    Node* newCollectionPage=db->dal->writeNode(new Node());

    Collection* collection=new Collection();
    collection->name=name;
    collection->root=newCollectionPage->pageNum;
    return createCollection(collection);
}

int Tx::DeleteCollection(char* name)
{
    if(!write)
        return writeInsideReadTxErr;

    Collection* rootCollection=getRootCollection();
    return rootCollection->Remove(name);
}