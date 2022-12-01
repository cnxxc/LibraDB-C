#include "freelist.h"
#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include <vector>

Freelist::Freelist():maxPage(MetaPageNum){};

PageNum Freelist::getNextPage()
{
	if(!releasedPages.empty())
	{
		PageNum pageID=releasedPages.back();
		releasedPages.pop_back();
		return pageID;
	}
	return ++maxPage;
}

void Freelist::releasePage(PageNum pagenum)
{
	releasedPages.push_back(pagenum);
}

/*将Freelist的内容序列化到一个页
+---------------+--------------+---------------+
|	最大页号    |	空闲页数    | 	空闲页号   	 |
+---------------+--------------+---------------+
|	2Bytes		|	2Bytes	   |空闲页数*8Byte  |
+---------------+--------------+---------------+
*/
char* Freelist::serialize(char* buf)
{
	char* ret=buf;
	uint16_t mp=(uint16_t)maxPage;
	memcpy(buf,&mp,2);
	buf+=2;
	uint16_t rs=(uint16_t)releasedPages.size();
	memcpy(buf,&rs,2);
	buf+=2;
	for(size_t i=0;i<releasedPages.size();++i)
	{
		memcpy(buf,&releasedPages[i],PageNumSize);
		buf+=4;
	}
	return ret;
}

void Freelist::deserialize(char* buf)
{
	memcpy(&maxPage,buf,2);
	int releasedPagesCount;
	memcpy(&releasedPagesCount,buf+2,2);
	buf+=4;
	for(size_t i=0;i<releasedPagesCount;++i)
	{
		uint64_t p;
		memcpy(&p,buf,PageNumSize);
		releasedPages.push_back(p);
		buf+=PageNumSize;
	}
}

Freelist::~Freelist(){}
