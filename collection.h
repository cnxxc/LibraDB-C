#ifndef _COLLECTION_H
#define _COLLECTION_H

#include "defs.h"
#include "node.h"
#include "dal.h"
#include "tx.h"
#include <string>
#include <vector>


class Node;
class Item;
class Tx;

//Collection的结构也是一棵B树，根Collection由数个item组成，每个item的key是collection.name，value是collection.root。
//每个Collection是一个数据库表，root是表数据的根结点
class Collection
{
public:
	Collection();
	Collection(const char* n,PageNum r);
	int Put(const char* key,const char* value);
	std::vector<Node*> getNodes(std::vector<int> indexes);//根据从根结点开始每步选择的子结点序号得到查找路径
	Item* Find(const char* key);
	int Remove(const char* key);
	Item* serialize();
	void deserialize(Item* item);
public:
	const char* name;
	PageNum root;//数据根结点所在页号
	uint64_t counter;
	Tx* tx;
};

#endif
