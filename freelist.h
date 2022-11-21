#ifndef _FREELIST_H
#define _FREELIST_H

#include "defs.h"
#include <vector>
class Freelist
{
public:
	Freelist();
	PageNum getNextPage();
	void releasePage(PageNum pagenum);
	char* serialize(char* buf);
	void deserialize(char* buf);
	~Freelist();
private:
	PageNum maxPage;
	std::vector<PageNum> releasedPages;
};

#endif
