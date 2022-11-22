#ifndef _NODE_H
#define _NODE_H

#include "defs.h"
#include <string>
#include <vector>

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
	Node();
	bool isLeaf();
	char* serialize(char* buf);
	void deserialize(char* buf);
	std:pair<bool,int> findKeyInNode(std::string& key);
	~Node();

public:
	Dal* dal;
	PageNum pageNum
	std::vector<Item*> items;
	std::vector<PageNum> childNodes;
};

#endif
