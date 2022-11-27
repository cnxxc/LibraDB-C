#ifndef _COLLECTION_H
#define _COLLECTION_H

#include "defs.h"
#include "node.h"
#include "dal.h"
#include <string>
#include <vector>
class Collection
{
public:
	Collection(Dal* d,std::string n,PageNum r);
	void Put(std::string key,std::string value);
	std::vector<Node*> getNodes(std::vector<int> indexes);//根据从根结点开始每步选择的子结点序号得到查找路径
	Item* Find(std::string key);
public:
	std::string name;
	PageNum root;//根结点所在页号
	Dal* dal;
};

#endif
