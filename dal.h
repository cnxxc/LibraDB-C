#ifndef _DAL_H
#define _DAL_H

#include "meta.h"
#include "freelist.h"
#include "node.h"

class Options
{
public:
	int pageSize;
	float minFillPercent;
	float maxFillPercent;
};

class Node;

class Page
{
public:
	PageNum num;
	char* data;	
};

class Dal
{
public:
	Dal(const char* path,Options* options);
	Page* allocateEmptyPage();
	Page* readPage(PageNum pagenum);		//将文件中pagenum对应的内容反序列化为Page
	Meta* readMeta();
	Freelist* readFreelist();
	Page* writeFreelist();
	void writePage(Page* page);				//把文件相应位置的内容写入page
	Page* writeMeta();
	Node* getNode(PageNum pagenum);			//把pagenum页中的内容反序列化为Node
	Node* writeNode(Node* node);			//根据node页号写入文件相应位置
	float maxThreshold();
	bool isOverPopulated(Node* node);
	float minThreshold();
	int getSplitIndex(Node* node);
	void deleteNode(PageNum pagenum);
	~Dal();	
public:
	FILE* file;
	int pageSize;
	Meta* meta;
	Freelist* freelist;
	float minFillPercent;
	float maxFillPercent;
};

#endif
