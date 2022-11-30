#include "collection.h"
#include "node.h"
#include <string>
#include <vector>

class Dal;
Collection::Collection(Dal* d,std::string n,PageNum r):dal(d),name(n),root(r){}

void Collection::Put(std::string key,std::string value)
{
	Item* i=new Item(key,value);

	Node* r;
	if(root==0)//创建根结点
	{
		r=new Node(dal,{i},{});
		dal->writeNode(r);
		root=r->pageNum;
		return;
	}
	else
	{
		r=dal->getNode(root);
	}

	std::vector<int> ancestorIndexes;
	std::pair<int,Node*> in=r->findKey(i->key,false,ancestorIndexes);
	int insertionIndex=in.first;
	Node* nodeToInsertIn=in.second;
	nodeToInsertIn->dal=dal;

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
		Node* newRoot=new Node(dal,std::vector<Item*>{},std::vector<PageNum>{rootNode->pageNum});
		newRoot->split(rootNode,0);//根结点只有一个，因此下标为0

		newRoot=dal->writeNode(newRoot);
		root=newRoot->pageNum;
	}

	delete r;
}

std::vector<Node*> Collection::getNodes(std::vector<int> indexes)
{
	Node* r=dal->getNode(root);
	std::vector<Node*> nodes{r};
	Node* child=r;
	for(size_t i=1;i<indexes.size();++i)
	{
		child=dal->getNode(child->childNodes[indexes[i]]);
		nodes.push_back(child);
	}
	return nodes;
}

Item* Collection::Find(std::string key)
{
	Node* n=dal->getNode(root);
	std::vector<int> ancestorIndexes;
	std::pair<int,Node*> in=n->findKey(key,true,ancestorIndexes);
	int index=in.first;
	Node* containingNode=in.second;
	if(index==-1)
		return NULL;
	return containingNode->items[index];
}

void Collection::Remove(std::string key)
{
	Node* rootNode=dal->getNode(root);
	std::vector<int> ancestorsIndexes;
	std::pair<int,Node*> in=rootNode->findKey(key,true,ancestorsIndexes);
	int removeItemIndex=in.first;
	Node* nodeToRemoveFrom=in.second;
	
	if(removeItemIndex==-1) return;

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
}