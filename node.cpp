#include "node.h"
#include <string.h>

Item::Item(std::string& k,std::string& v):key(k),value(v){}

Item::~Item(){}

Node::Node(){}

bool Node::isLeaf()
{
	return childNodes.empty();
}

/*
+-----------+-----------+---------------+-----------+-------------------+
|	ifLeaf	|	ItemNum	|	childNodes	|  nothing	|		Items		|	
+-----------+-----------+---------------+-----------+-------------------+
|	1Byte	|	2Bytes	|  (k+1)*8Bytes	|			|	k*(klen+vlen+2)	|
+-----------+-----------+---------------+-----------+-------------------+
*/
char* Node::serialize(char* buf)
{
	char* leftPos=buf;
	int rightPos=dal->pageSize-1;
	uint64_t bitSetVar=0;
	bool isLeaf=isLeaf();
	if(isLeaf)
	{
		bitSetVar=1;
	}
	buf[leftPos++]=(char)bitSetVar;
	memcpy(leftPos,(uint16_t)items.size(),2);
	leftPos+=2;

	for(size_t i=0;i<items.size();++i)
	{
		Item* item=items[i];
		if(!isLeaf)
		{
			PageNum childNode=childNodes[i];
			memcpy(leftPos,&childNode,PageNumSize);//左边存子结点页号
			leftPos+=PageNumSize;
		}

		size_t kLen=item->key.size();
		size_t vLen=item->value.size();

		uint16_t offset=(uint16_t)(rightPos-kLen-vLen-2);//右边存子结点信息（key长度、key、value长度、value）
		memcpy(leftPos,&offset,2);
		leftPos+=2；

		rightPos-=vLen;
		memcpy(buf+rightPos,item->value,vLen);

		--rightPos;
		buf[rightPos]=(char)vLen;

		rightPos-=kLen;
		memcpy(buf+rightPos,item->key,kLen);

		--rightPos;
		buf[rightPos]=(char)kLen;
	}

	if(!isLeaf)
	{
		PageNum lastChildNode=childNodes.back();
		memcpy(leftPos,&lastChildNode,PageNumSize);
	}

	return buf;
}

void Node::deserialize(char* buf)
{
	
}

Node::~Node(){}
