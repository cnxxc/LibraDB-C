#include "meta.h"
#include <unistd.h>
#include <string.h>

Meta::Meta(){}

void Meta::serialize(char* buf)
{	
	uint64_t rt=(uint64_t)root;
	memcpy(buf,&rt,PageNumSize);

	uint64_t fp=(uint64_t)freelistPage;
	memcpy(buf+PageNumSize,&fp,PageNumSize);
}

void Meta::deserialize(char* buf)
{
	memcpy(&root,buf,PageNumSize);
	memcpy(&freelistPage,buf+PageNumSize,PageNumSize);
}

Meta::~Meta(){}
