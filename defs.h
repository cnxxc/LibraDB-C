///////def.h///////////////
#include <stdint.h>
#include <stdio.h>

#define PAGESIZE 4096	//页大小
#define METAPAGENUM 0	//Meta页号
#define	PAGENUMSIZE 8	//页号用几位存储

typedef uint64_t PageNum;
 
typedef struct{
    PageNum maxPage;
    PageNum releasedPages[100];
    PageNum releasedSize;   //releasedPages的有效大小
}Freelist;

typedef struct{
	PageNum freelistPage; 
}Meta;

typedef struct{
	FILE* file;
	//int pageSize;页大小4096KB
	Meta* meta;
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
Page* writeFreelist(Dal* dal);
Page* writeMeta(Dal* dal);
Meta* readMeta(Dal* dal);
Freelist* readFreelist(Dal* dal);

/////////Freelist///////////////////
Freelist* newFreelist();
PageNum getNextPage(Freelist* freelist);
void releasePage(Freelist* freelist,PageNum pagenum);
char* serializeFreelist(Freelist* freelist,char* buf);
void deserializeFreelist(Freelist* freelist,char* buf);

////////////Meta///////////////
Meta* newEmptyMeta();
void serializeMeta(Meta* meta,char* buf);
void deserializeMeta(Meta* meta,char* buf);
