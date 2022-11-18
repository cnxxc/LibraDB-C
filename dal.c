/***************数据访问层***********/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include "defs.h"

Dal* newDal(const char* path)
{
	Dal *dal=(Dal*)malloc(sizeof(Dal));
	FILE* f=fopen(path,"w+");
	if(f==NULL)
	{
		fprintf(stderr,"File: %s open failed!\n",path);
		exit(1);
	}
	dal->file=f;
	//dal->pageSize=pagesize;
	dal->freelist=newFreelist();
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
	return;
}

Page* allocateEmptyPage()
{
	Page* page=(Page*)malloc(sizeof(Page));
	page->num=0;
	return page;
}

//从dal文件写入page
Page* readPage(Dal* dal,PageNum pgnum)
{
	Page *page=allocateEmptyPage();
	int offset=pgnum*PAGESIZE;
	int ret=fseek(dal->file,offset,SEEK_SET);
	if(ret!=0)
	{
		fprintf(stderr,"fseek file failed!\n");
		exit(1);
	}
	fread(page->data,sizeof(char),PAGESIZE,dal->file);
	return page;
}

void writePage(Dal* dal,Page* page)
{
	int offset=page->num*PAGESIZE;
	int ret=fseek(dal->file,offset,SEEK_SET);
	if(ret!=0)
	{
		fprintf(stderr,"fseek file failed!\n");
		exit(1);
	}
	fwrite(page->data,sizeof(char),PAGESIZE,dal->file);
	return;
}
