#ifndef _NODE_H
#define _NODE_H

#include "defs.h"
#include "dal.h"
#include <string>
#include <vector>
#include <initializer_list>
class Dal;

class Item
{
public:
	Item(std::string& k,std::string& v);
	~Item();
public:
	std::string key;
	std::string value;
};


//B树每个结点的子结点数比Item数小1，k个Item分割出k+1个区间（子结点），决定下一步到哪个子树
class Node
{
public:
	Node(Dal* d);
	Node(Dal* d,std::vector<Item*> iv,std::vector<PageNum> pv);		//用items和childNodes构造
	bool isLeaf();
	char* serialize(char* buf);
	void deserialize(char* buf);
	std::pair<bool,int> findKeyInNode(std::string key);			//返回是否存在、应该在的位置
	std::pair<int,Node*> findKeyHelper(std::string key,bool exact,std::vector<int>& ancestorsIndexes);//exact参数为false时，若找不到key，会返回其应该插入的位置
	std::pair<int,Node*> findKey(std::string key,bool exact,std::vector<int>& ancestorIndexes);	//返回所在结点中的下标（不存在为-1）、所在结点，ancestorIndexes是从根结点开始的搜索路径上的Node下标
	Node* getNode(PageNum pagenum);
	Node* writeNode();
	int addItem(Item* item,int insertionIndex);
	int elementSize(int i);//下标为i的Item大小+下标为i的子结点页号大小
	int nodeSize();
	void split(Node* nodeToSplit,int nodeToSplitIndex);
	void writeNodes(std::initializer_list<Node*> nodeList);
	bool isOverPopulated();
	void removeItemFromLeaf(int index);
	std::vector<int> removeItemFromInternal(int index);//删除非叶子结点上的Item，返回从删除Item左子树开始的每层被影响的子结点的下标（不含被删Item所在结点的下标）
	void merge(Node* bNode,int bNodeIndex);
	bool canSpareAnElement();//是否可以分割（大小高于下限）
	void rebalanceRemove(Node* unbalancedNode,int unbalancedNodeIndex);
	bool isUnderPopulated();
	~Node();

public:
	Dal* dal;
	PageNum pageNum;
	std::vector<Item*> items;
	std::vector<PageNum> childNodes;
};

#endif
