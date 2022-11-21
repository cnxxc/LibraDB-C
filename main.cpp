#include "dal.h"
#include "freelist.h"
#include "meta.h"
#include "page.h"
#include <string.h>

int main()
{
	Dal* dal=new Dal("db.db");
	Page* p=dal->allocateEmptyPage();
	p->num=dal->freelist->getNextPage();
	memcpy(p->data,"data\0",5);
	dal->writePage(p);
	dal->writeFreelist();
	delete dal;
	dal=new Dal("db.db");
	p=dal->allocateEmptyPage();
	p->num=dal->freelist->getNextPage();
	memcpy(p->data,"data2\0",6);
	dal->writePage(p);
	PageNum pn=dal->freelist->getNextPage();
	dal->freelist->releasePage(pn);
	dal->writeFreelist();
	return 0;
}
