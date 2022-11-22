#include "dal.h"
#include "freelist.h"
#include "meta.h"
#include "node.h"
#include <string.h>
#include <string>
#include <iostream>

int main()
{
	Dal* dal=new Dal("./mainTest");
	
	Node* node=dal->getNode(dal->meta->root);
	node->dal=dal;
	std::pair<int,Node*> bn=node->findKey(node,std::string("Key1"));
	Item* item=bn.second->items[bn.first];
	std::cout<<"key is: "+item->key+" ,value is: "+item->value<<std::endl;
	delete dal;
}
