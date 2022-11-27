#include "dal.h"
#include "freelist.h"
#include "meta.h"
#include "node.h"
#include "collection.h"
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>

int main()
{
	Options* options=new Options();
	options->pageSize=getpagesize();
	options->minFillPercent=0.0125;
	options->maxFillPercent=0.025;
	Dal* dal=new Dal("./mainTest",options);
	Collection* c=new Collection(dal,"collection1",dal->meta->root);
	c->Put("Key1","Value1");
	c->Put("Key2","Value2");
	c->Put("Key3","Value3");
	c->Put("Key4","Value4");
	c->Put("Key5","Value5");
	c->Put("Key6","Value6");
	Item* item=c->Find("Key1");
	std::cout<<"key is: "<<item->key<<",value is: "<<item->value<<std::endl;
	delete dal;
	return 0;
}
