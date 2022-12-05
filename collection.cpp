#include "collection.h"
#include "node.h"
#include "error.h"
#include <string>
#include <vector>
#include <malloc.h>
#include <string.h>

Collection::Collection(){}

Collection::Collection(char* n,PageNum r):name(n),root(r){}

int Collection::Put(char* key,char* value)
{
	if(!tx->write) return writeInsideReadTxErr;

	Item* i=new Item(key,value);

	Node* r;
	if(root==0)//创建根结点
	{
		r=tx->writeNode(new Node({i},{}));
		root=r->pageNum;
		return 0;
	}
	else
	{
		r=tx->getNode(root);
	}

	std::vector<int> ancestorIndexes;
	std::pair<int,Node*> in=r->findKey(i->key,false,ancestorIndexes);
	int insertionIndex=in.first;
	Node* nodeToInsertIn=in.second;
	//nodeToInsertIn->dal=dal;

	if(!nodeToInsertIn->items.empty()&&insertionIndex<nodeToInsertIn->items.size()&&key==nodeToInsertIn->items[insertionIndex]->key)
		nodeToInsertIn->items[insertionIndex]=i;
	else
		nodeToInsertIn->addItem(i,insertionIndex);
	nodeToInsertIn->writeNode();

	std::vector<Node*> ancestors=getNodes(ancestorIndexes);

	for(int i=ancestors.size()-2;i>=0;--i)
	{
		Node* pnode=ancestors[i];
		Node* node=ancestors[i+1];
		int nodeIndex=ancestorIndexes[i+1];
		if(node->isOverPopulated())
			pnode->split(node,nodeIndex);
	}

	Node* rootNode=ancestors[0];
	if(rootNode->isOverPopulated())
	{
		Node* newRoot=new Node(std::vector<Item*>{},std::vector<PageNum>{rootNode->pageNum});
		newRoot->split(rootNode,0);//根结点只有一个，因此下标为0

		newRoot=tx->writeNode(newRoot);
		root=newRoot->pageNum;
	}

	delete r;
	return 0;
}

std::vector<Node*> Collection::getNodes(std::vector<int> indexes)
{
	Node* r=tx->getNode(root);
	std::vector<Node*> nodes{r};
	Node* child=r;
	for(size_t i=1;i<indexes.size();++i)
	{
		child=tx->getNode(child->childNodes[indexes[i]]);
		nodes.push_back(child);
	}
	return nodes;
}

Item* Collection::Find(char* key)
{
	Node* n=tx->getNode(root);
	std::vector<int> ancestorIndexes;
	std::pair<int,Node*> in=n->findKey(key,true,ancestorIndexes);
	int index=in.first;
	Node* containingNode=in.second;
	if(index==-1)
		return NULL;
	return containingNode->items[index];
}

int Collection::Remove(char* key)
{
	Node* rootNode=tx->getNode(root);
	std::vector<int> ancestorsIndexes;
	std::pair<int,Node*> in=rootNode->findKey(key,true,ancestorsIndexes);
	int removeItemIndex=in.first;
	Node* nodeToRemoveFrom=in.second;
	
	if(removeItemIndex==-1) 
		return removeNotExistKeyErr;

	if(nodeToRemoveFrom->isLeaf())
	{
		nodeToRemoveFrom->removeItemFromLeaf(removeItemIndex);
	}
	else
	{
		std::vector<int> affectedNodes=nodeToRemoveFrom->removeItemFromInternal(removeItemIndex);
		for(int i=0;i<affectedNodes.size();++i)
		{
			ancestorsIndexes.push_back(affectedNodes[i]);
		}
	}

	std::vector<Node*> ancestors=getNodes(ancestorsIndexes);

	for(int i=ancestors.size()-2;i>=0;--i)
	{
		Node* pNode=ancestors[i];
		Node* node=ancestors[i+1];
		if(node->isUnderPopulated())
		{
			pNode->rebalanceRemove(node,ancestorsIndexes[i+1]);
		}
	}

	rootNode=ancestors[0];
	if(rootNode->items.empty()&&!rootNode->childNodes.empty())
		root=ancestors[1]->pageNum;

	return 0;
}

Item* Collection::serialize()
{
	char* b=(char*)malloc(CollectionSize*sizeof(char));
	int leftPos=0;
	memcpy(b+leftPos,&root,PageNumSize);
	leftPos+=PageNumSize;
	memcpy(b+leftPos,&counter,CounterSize);
	leftPos+=CounterSize;
	return new Item{name,b};
}

void Collection::deserialize(Item* item)
{
	memcpy(name,item->key,strlen(item->key));
	if(strlen(item->value)!=0)
	{
		int leftPos=0;
		memcpy(&root,item->value+leftPos,PageNumSize);
		leftPos+=PageNumSize;
		memcpy(&counter,item->value+leftPos,CounterSize);
		leftPos+=CounterSize;
	}
}