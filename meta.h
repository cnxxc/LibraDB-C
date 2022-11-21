#ifndef _META_H
#define _META_H

#include "defs.h"

class Meta
{
public:
	Meta();
	void serialize(char* buf);
	void deserialize(char* buf);
	~Meta();
public:
	PageNum freelistPage;
};

#endif
