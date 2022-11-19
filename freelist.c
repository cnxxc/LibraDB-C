/*****************freelist.c***************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "defs.h"

const int metaPage=0;

Freelist* newFreelist()
{
	Freelist* freelist=(Freelist*)malloc(sizeof(Freelist));
	freelist->maxPage=metaPage;
	freelist->releasedSize=0;
	return freelist;
}

PageNum getNextPage(Freelist* freelist)
{
	if(freelist->releasedSize>0)
	{
		PageNum pageID=freelist->releasedPages[freelist->releasedSize-1];//静态数组无法释放元素，一定要同步更改releasedSize
		--freelist->releasedSize;
		return pageID;
	}
	++freelist->maxPage;
	return freelist->maxPage;
}

void releasePage(Freelist* freelist,PageNum pagenum)
{
	freelist->releasedPages[freelist->releasedSize++]=pagenum;
	return;
}

/*将Freelist的内容序列化到一个页
+---------------+--------------+---------------+
|	最大页号    |	空闲页数   | 	空闲页号   |
+---------------+--------------+---------------+
|	2Bytes		|	2Bytes	   |空闲页数*8Bytes|
+---------------+--------------+---------------+
*/
char* serializeFreelist(Freelist* freelist,char* buf)
{
	char* ret=buf;
	uint16_t mp=(uint16_t)(freelist->maxPage);
	memcpy(buf,&mp,2);
	buf+=2;
	uint16_t rs=(uint16_t)(freelist->releasedSize);
	memcpy(buf,&rs,2);
	buf+=2;
	for(size_t i=0;i<freelist->releasedSize;++i)
	{
		memcpy(buf,(freelist->releasedPages)+i,PAGENUMSIZE);
		buf+=4;
	}
	return ret;
}

void deserializeFreelist(Freelist* freelist,char* buf)
{
	memcpy(&(freelist->maxPage),buf,2);
	memcpy(&(freelist->releasedSize),buf+2,2);
	buf+=4;
	for(size_t i=0;i<freelist->releasedSize;++i)
	{
		memcpy((freelist->releasedPages)+i,buf,PAGENUMSIZE);
		buf+=PAGENUMSIZE;
	}
}
