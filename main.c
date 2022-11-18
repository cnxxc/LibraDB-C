#include "defs.h"
#include <unistd.h>
#include <string.h>

int main()
{
	Dal* dal=newDal("db.db");
	Page* page=allocateEmptyPage();
	const char* p="data";
	memcpy(page->data,p,4);
	page->data[4]='\0';
	writePage(dal,page);
	//FILE* f=fopen("txt","a+");
	//char s[]="1234";
	//fwrite(s,sizeof(char),4,f);
	return 0;
}
