#ifndef _DEFS_H
#define _DEFS_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef uint64_t PageNum;
const int PageNumSize=8;	//页号用几Byte存储
const int MetaPageNum=0;	//Meta页号
const int NodeHeaderSize=3;	//Node序列化后头部大小1Byte（isLeaf）+2Byte（itemNum）
const int PageSize=getpagesize();
const int CounterSize=4;
const int CollectionSize=16;

#endif
