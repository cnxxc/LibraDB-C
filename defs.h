#include <stdint.h>
#include <stdio.h>

#define PAGESIZE 4096

typedef uint64_t PageNum;
 

typedef struct{
    PageNum maxPage;
    PageNum releasedPages[100];
    PageNum releasedSize;   //releasedPages的有效大小
}Freelist;

typedef struct{
	FILE* file;
	//int pageSize;
    Freelist* freelist;
}Dal;

typedef struct{
	PageNum num;
    char data[PAGESIZE];
}Page;

/////////Dal//////////////
Dal* newDal(const char* path);
void closeFile(Dal* dal);
Page* allocateEmptyPage();
Page* readPage(Dal* dal,PageNum pgnum);
void writePage(Dal* dal,Page* page);

/////////Freelist///////////////////
Freelist* newFreelist();
PageNum getNextPage(Freelist* freelist);
void releasePage(Freelist* freelist,PageNum pagenum);
