#include "meta.h"
#include <unistd.h>
#include <string.h>

Meta::Meta(){}

void Meta::serialize(char* buf)
{
	uint64_t fp=(uint64_t)freelistPage;
	memcpy(buf,&fp,PageNumSize);
}

void Meta::deserialize(char* buf)
{
	memcpy(&freelistPage,buf,PageNumSize);
}

Meta::~Meta(){}
