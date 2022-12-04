#ifndef _COLLECTION_H
#define _COLLECTION_H

#include "defs.h"
#include "node.h"
#include "dal.h"
#include "tx.h"
#include <string>
#include <vector>

//Collection的结构也是一棵B树，根Collection由数个item组成，每个item的key是collection.name，value是collection.root。
//每个item代表一个collection。
class Collection
{
public:
	Collection();
	Collection(char* n,PageNum r);
	int Put(char* key,char* value);
	std::vector<Node*> getNodes(std::vector<int> indexes);//根据从根结点开始每步选择的子结点序号得到查找路径
	Item* Find(std::string key);
	void Remove(std::string key);
	Item* Find(std::string key);//返回的item实际上代表一个collection的信息
	Item* serialize();
	void deserialize(Item* item);
public:
	char* name;
	PageNum root;//根结点所在页号
	uint64_t counter;
	Tx* tx;
};

#endif
