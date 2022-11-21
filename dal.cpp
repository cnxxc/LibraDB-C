#include "defs.h"
#include "dal.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>

Dal::Dal(const char* path):pageSize(getpagesize())
{
	if(0==access(path,F_OK))
	{
		FILE* f=fopen(path,"r+");
		if(f==NULL)
		{
			fprintf(stderr,"File: %s open failed!\n",path);
			exit(1);
		}
		file=f;
		meta=readMeta();
		Freelist* fl=readFreelist();
		freelist=fl;
	}	
	else
	{
		FILE* f=fopen(path,"w");
		if(f==NULL)
		{
			fprintf(stderr,"File: %s open failed!\n",path);
			exit(1);
		}
		file=f;
		freelist=new Freelist();
		meta=new Meta();
		meta->freelistPage=freelist->getNextPage();
		writeFreelist();
		writeMeta();
	}
}

Page* Dal::allocateEmptyPage()
{
	Page* page=new Page();
	page->data=(char*)calloc(pageSize,sizeof(char));
	return page;	
}

//将文件pagenum页的内容读到Page中
Page* Dal::readPage(PageNum pagenum)
{
	Page *page=allocateEmptyPage();
	int offset=pagenum*pageSize;
	int ret=fseek(file,offset,SEEK_SET);
	if(ret!=0)
	{
		fprintf(stderr,"fseek file failed!\n");
		exit(1);
	}
	fread(page->data,sizeof(char),pageSize,file);
	return page;
}

//将文件MetaPageNum页的内容读到Meta中
Meta* Dal::readMeta()
{
	Page* p=readPage(MetaPageNum);
	Meta* meta=new Meta();
	meta->deserialize(p->data);
	return meta;
}

Freelist* Dal::readFreelist()
{
	Page* page=readPage(meta->freelistPage);
	Freelist* freelist=new Freelist();
	freelist->deserialize(page->data);
	return freelist;
}

Page* Dal::writeFreelist()
{
	Page* p=allocateEmptyPage();
	p->num=meta->freelistPage;
	freelist->serialize(p->data);
	writePage(p);
	meta->freelistPage=p->num;
	return p;
}

void Dal::writePage(Page* page)
{
	int offset=page->num*pageSize;
	int ret=fseek(file,offset,SEEK_SET);
	if(ret!=0)
	{
		fprintf(stderr,"fseek file failed!\n");
		exit(1);
	}
	fwrite(page->data,sizeof(char),pageSize,file);
	return;
}

Page* Dal::writeMeta()
{
	Page* p=allocateEmptyPage();
	p->num=MetaPageNum;
	meta->serialize(p->data);
	writePage(p);
	return p;
}

Dal::~Dal()
{
	if(file==NULL)
	{
		fprintf(stderr,"File did not exists!\n");
		exit(1);
	}
	int ret=fclose(file);
	if(ret!=0)
	{
		fprintf(stderr,"File close failed!\n");
		exit(1);
	}
}
