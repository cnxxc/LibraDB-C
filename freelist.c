/*****************空闲链表***************/
#include <stdlib.h>
#include <malloc.h>
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
	freelist->releasedPages[++freelist->releasedSize]=pagenum;
	return;
}
