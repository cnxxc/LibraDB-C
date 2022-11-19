/////////meta.c/////////
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include "defs.h"

Meta* newEmptyMeta()
{
	Meta* meta=(Meta*)malloc(sizeof(Meta));
	return meta;
}

void serializeMeta(Meta* meta,char* buf)
{
	uint64_t fp=(uint64_t)meta->freelistPage;
	memcpy(buf,&fp,PAGENUMSIZE);
}

void deserializeMeta(Meta* meta,char* buf)
{
	memcpy(&(meta->freelistPage),buf,PAGENUMSIZE);
}
