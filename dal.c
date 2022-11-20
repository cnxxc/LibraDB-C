/***************dal.c***********/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include "defs.h"

Dal* newDal(const char* path)
{
	Dal *dal=(Dal*)malloc(sizeof(Dal));
	dal->pageSize=getpagesize();
	dal->meta=newEmptyMeta();
	if(0==access(path,F_OK))
	{
		FILE* f=fopen(path,"r+");
		if(f==NULL)
		{
			fprintf(stderr,"File: %s open failed!\n",path);
			exit(1);
		}
		dal->file=f;
		Meta* m=readMeta(dal);
		dal->meta=m;
		Freelist* fl=readFreelist(dal);
		dal->freelist=fl;
	}
	else{
		FILE* f=fopen(path,"w");
		if(f==NULL)
		{
			fprintf(stderr,"File: %s open failed!\n",path);
			exit(1);
		}
		dal->file=f;
		//dal->pageSize=pagesize;
		dal->freelist=newFreelist();
		dal->meta->freelistPage=getNextPage(dal->freelist);
		writeFreelist(dal);
		writeMeta(dal);
	}
	return dal;
}

void closeFile(Dal* dal)
{
	if(dal->file==NULL)
	{
		fprintf(stderr,"File did not exists!\n");
		exit(1);
	}
	int ret=fclose(dal->file);
	if(ret!=0)
	{
		fprintf(stderr,"File close failed!\n");
		exit(1);
	}
	free(dal);
	return;
}

Page* allocateEmptyPage(Dal* dal)
{
	Page* page=(Page*)malloc(sizeof(Page));
	page->data=(char*)calloc(dal->pageSize,sizeof(char));
	return page;
}

//从dal文件写入page
Page* readPage(Dal* dal,PageNum pgnum)
{
	Page *page=allocateEmptyPage(dal);
	int offset=pgnum*dal->pageSize;
	int ret=fseek(dal->file,offset,SEEK_SET);
	if(ret!=0)
	{
		fprintf(stderr,"fseek file failed!\n");
		exit(1);
	}
	fread(page->data,sizeof(char),dal->pageSize,dal->file);
	return page;
}

//将page的内容写入dal文件
void writePage(Dal* dal,Page* page)
{
	int offset=page->num*dal->pageSize;
	int ret=fseek(dal->file,offset,SEEK_SET);
	if(ret!=0)
	{
		fprintf(stderr,"fseek file failed!\n");
		exit(1);
	}
	fwrite(page->data,sizeof(char),dal->pageSize,dal->file);
	return;
}

Page* writeFreelist(Dal* dal)
{
	Page* p=allocateEmptyPage(dal);
	p->num=dal->meta->freelistPage;
	serializeFreelist(dal->freelist,p->data);
	writePage(dal,p);
	dal->meta->freelistPage=p->num;
	return p;
}

Page* writeMeta(Dal* dal)
{
	Page* p=allocateEmptyPage(dal);
	p->num=METAPAGENUM;
	serializeMeta(dal->meta,p->data);
	writePage(dal,p);
	return p;
}

Meta* readMeta(Dal* dal)
{
	Page* p=readPage(dal,METAPAGENUM);
	Meta* meta=newEmptyMeta();
	deserializeMeta(meta,p->data);
	return meta;
}

Freelist* readFreelist(Dal* dal)
{
	Page* page=readPage(dal,dal->meta->freelistPage);
	Freelist* freelist=newFreelist();
	deserializeFreelist(freelist,page->data);
	return freelist;
}
