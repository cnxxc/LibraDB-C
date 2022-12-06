#include "dal.h"
#include "freelist.h"
#include "meta.h"
#include "node.h"
#include "collection.h"
#include "db.h"
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>

int main()
{
	Options option{};
	option.maxFillPercent=1;
	option.minFillPercent=0.5;
	DB* db=Open("Demo7",&option);
	Tx* tx=db->WriteTx();
	const char* collectionName="Demo7Collection";
	Collection* createdCollection=tx->createCollection(collectionName);
	const char* newKey="key0";
	const char* newVal="value0";
	createdCollection->Put(newKey,newVal);
	tx->Commit();
	db->Close();
	delete db;
	db=NULL;
	db=Open("Demo7",&option);
	delete tx;
	tx=NULL;
	tx=db->ReadTx();
	//delete createdCollection;
	//createdCollection=NULL;
	createdCollection=tx->getCollection(collectionName);
	Item* item=createdCollection->Find(newKey);
	tx->Commit();
	db->Close();
	std::cout<<"key is: "<<item->key<<", value is: "<<item->value<<std::endl;
	return 0;
}
