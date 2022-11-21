#ifndef _DAL_H
#define _DAL_H

#include "meta.h"
#include "freelist.h"
#include "node.h"

class Page
{
public:
	PageNum num;
	char* data;	
};

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
	Node* getNode(PageNum pagenum);
	~Dal();	
public:
	FILE* file;
	int pageSize;
	Meta* meta;
	Freelist* freelist;
};

#endif
