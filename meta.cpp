#include "meta.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

Meta::Meta(){}

void Meta::serialize(char* buf)
{	
	int pos=0;
	memcpy(buf,&MagicNumber,MagicNumberSize);
	pos+=MagicNumberSize;

	uint64_t rt=(uint64_t)root;
	memcpy(buf+pos,&rt,PageNumSize);
	pos+=PageNumSize;

	uint64_t fp=(uint64_t)freelistPage;
	memcpy(buf+pos,&fp,PageNumSize);
}

void Meta::deserialize(char* buf)
{
	int pos=0;
	int magicNumberRes;
	memcpy(&magicNumberRes,buf,MagicNumberSize);
	pos+=MagicNumberSize;
	if(magicNumberRes!=MagicNumber)
		exit(1);

	memcpy(&root,buf+pos,PageNumSize);
	pos+=PageNumSize;

	memcpy(&freelistPage,buf+pos,PageNumSize);
}

Meta::~Meta(){}
