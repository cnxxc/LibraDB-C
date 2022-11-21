#ifndef _DAL_H
#define _DAL_H

#include "meta.h"
#include "freelist.h"
#include "page.h"

class Dal
{
public:
	Dal(const char* path);
	Page* allocateEmptyPage();
	Page* readPage(PageNum pagenum);
	Meta* readMeta();
	Freelist* readFreelist();
	Page* writeFreelist();
	void writePage(Page* page);
	Page* writeMeta();
	~Dal();	
public:
	FILE* file;
	int pageSize;
	Meta* meta;
	Freelist* freelist;
};

#endif
