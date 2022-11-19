#include "defs.h"
#include <unistd.h>
#include <string.h>

int main()
{
	Dal* dal=newDal("db.db");
	Page* p=allocateEmptyPage();
	p->num=getNextPage(dal->freelist);
	memcpy(p->data,"data\0",5);
	writePage(dal,p);
	writeFreelist(dal);
	closeFile(dal);
	dal=newDal("db.db");
	p=allocateEmptyPage();
	p->num=getNextPage(dal->freelist);
	memcpy(p->data,"data2\0",6);
	writePage(dal,p);
	PageNum pn=getNextPage(dal->freelist);
	releasePage(dal->freelist,pn);
	writeFreelist(dal);
}
