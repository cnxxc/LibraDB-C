#include "node.h"
#include "defs.h"
#include <string.h>
#include <string>
#include <vector>
#include <initializer_list>

Item::Item(std::string& k,std::string& v):key(k),value(v){}

Item::~Item(){}

Node::Node(){}

Node::Node(std::vector<Item*> iv,std::vector<PageNum> pv):items(iv),childNodes(pv){}

bool Node::isLeaf()
{
	return childNodes.empty();
}

/*
+-----------+-----------+---------------------------+-----------+-------------------+
|	ifLeaf	|	ItemNum	|	childNodes+offset		|  	...		|		Items		|	
+-----------+-----------+---------------------------+-----------+-------------------+
|	1Byte	|	2Bytes	|  (k+1)*(8Bytes+2Bytes)	|			|	k*(klen+vlen+2)	|
+-----------+-----------+---------------------------+-----------+-------------------+
*/
char* Node::serialize(char* buf)
{
	char* leftPos=buf;
	int rightPos=dal->pageSize-1;
	uint64_t bitSetVar=0;
	bool is_Leaf=isLeaf();
	if(is_Leaf)
	{
		bitSetVar=1;
	}
	*leftPos++=(char)bitSetVar;
	uint16_t itemSize=items.size();
	memcpy(leftPos,&itemSize,2);
	leftPos+=2;

	for(size_t i=0;i<items.size();++i)
	{
		Item* item=items[i];
		if(!is_Leaf)
		{
			PageNum childNode=childNodes[i];
			memcpy(leftPos,&childNode,PageNumSize);//左边存子结点页号
			leftPos+=PageNumSize;
		}

		size_t kLen=item->key.size();
		size_t vLen=item->value.size();

		uint16_t offset=(uint16_t)(rightPos-kLen-vLen-2);//右边存子结点信息（key长度、key、value长度、value）
		memcpy(leftPos,&offset,2);
		leftPos+=2;

		rightPos-=vLen;
		const char* v=item->value.data();
		memcpy(buf+rightPos,v,vLen);

		--rightPos;
		buf[rightPos]=(char)vLen;

		rightPos-=kLen;
		const char* k=item->key.data();
		memcpy(buf+rightPos,k,kLen);

		--rightPos;
		buf[rightPos]=(char)kLen;
	}

	if(!is_Leaf)
	{
		PageNum lastChildNode=childNodes.back();
		memcpy(leftPos,&lastChildNode,PageNumSize);
	}

	return buf;
}

void Node::deserialize(char* buf)
{
	char* leftPos=buf;
	bool isLeaf=(uint16_t)buf[0];
	uint16_t c;
	memcpy(&c,buf+1,2);
	uint16_t itemsCount=(uint16_t)c;
	leftPos+=3;

	for(size_t i=0;i<itemsCount;++i)
	{
		if(!isLeaf)
		{
			PageNum pn;
			memcpy(&pn,leftPos,PageNumSize);
			leftPos+=PageNumSize;
			childNodes.push_back(pn);
		}

		uint16_t offset;
		memcpy(&offset,leftPos,2);
		leftPos+=2;

		uint16_t kLen=0;
		memcpy(&kLen,buf+offset,1);
		++offset;

		std::string key(buf+offset,kLen);
		offset+=kLen;

		uint16_t vLen=0;
		memcpy(&vLen,buf+offset,1);
		++offset;

		std::string value(buf+offset,vLen);
		offset+=vLen;

		Item* item=new Item(key,value);
		items.push_back(item);
	}

	if(!isLeaf)
	{
		PageNum pn;
		memcpy(&pn,leftPos,PageNumSize);
		childNodes.push_back(pn);
	}
}

std::pair<bool,int> Node::findKeyInNode(std::string key)
{
	for(size_t i=0;i<items.size();++i)
	{
		if(items[i]->key==key)
		{
			return {true,i};
		}
		if(items[i]->key>key)//找到第一个比待查key大的Item
		{
			return {false,i};
		}
	}
	return {false,items.size()};//待查找的key比最后一个Item还大
}

std::pair<int,Node*> Node::findKeyHelper(std::string key,bool exact,std::vector<int>& ancestorsIndexes)
{
	std::pair<bool,int> bi=findKeyInNode(key);
	bool wasFound=bi.first;
	int index=bi.second;
	if(wasFound) return {index,this};

	if(isLeaf())
	{
		if(exact) return {-1,NULL};
		return {index,this};
	}

	ancestorsIndexes.push_back(index);//保存查找路径
	Node* nextChild=getNode(childNodes[index]);
	return nextChild->findKeyHelper(key,exact,ancestorsIndexes);
}

std::pair<int,Node*> Node::findKey(std::string key,bool exact,std::vector<int>& ancestorIndexes)
{
	ancestorIndexes=std::vector<int>{0};
	std::pair<int,Node*> in=findKeyHelper(key,exact,ancestorIndexes);
	int index=in.first;
	Node* node=in.second;
	return {index,node};
}

Node* Node::getNode(PageNum pagenum)
{
	return dal->getNode(pagenum);
}

int Node::addItem(Item* item,int insertionIndex)
{
	if(items.size()==insertionIndex)
	{
		items.push_back(item);
		return insertionIndex;
	}

	items.insert(items.begin()+insertionIndex,item);
	return insertionIndex;
}

Node* Node::writeNode()
{
	return dal->writeNode(this);
}

int Node::elementSize(int i)
{
	int size=0;
	size+=items[i]->key.size();
	size+=items[i]->value.size();
	size+=PageNumSize;
	return size;
}

int Node::nodeSize()
{
	int size=0;
	size+=NodeHeaderSize;
	for(size_t i=0;i<items.size();++i)
	{
		size+=elementSize(i);
	}

	size+=PageNumSize;//子结点数比item数多1
	return size;
}

//this是nodeToSplit的父结点,nodeToSplitIndex是nodeToSplit在this的childNodes中的下标
void Node::split(Node* nodeToSplit,int nodeToSplitIndex)
{
	int splitIndex=nodeToSplit->dal->getSplitIndex(nodeToSplit);

	Item* middleItem=nodeToSplit->items[splitIndex];
	Node* newNode;

	if(nodeToSplit->isLeaf())
	{
		newNode=new Node(std::vector<Item*>{nodeToSplit->items.begin()+splitIndex+1,nodeToSplit->items.end()},std::vector<PageNum>{});
		newNode->writeNode();
		nodeToSplit->items=std::vector<Item*>{nodeToSplit->items.begin(),nodeToSplit->items.begin()+splitIndex};
	}
	else
	{
		newNode=new Node(std::vector<Item*>{nodeToSplit->items.begin()+splitIndex+1,nodeToSplit->items.end()},std::vector<PageNum>{nodeToSplit->childNodes.begin()+splitIndex+1,nodeToSplit->childNodes.end()});
		newNode->writeNode();
		items=std::vector<Item*>{nodeToSplit->items.begin(),nodeToSplit->items.begin()+splitIndex};
		childNodes=std::vector<PageNum>{nodeToSplit->childNodes.begin(),nodeToSplit->childNodes.begin()+splitIndex+1};
	}
	this->addItem(middleItem,nodeToSplitIndex);//把分离位置的Item移到根结点上
	//把新分出来的结点加到子结点数组相应位置
	if(childNodes.size()==nodeToSplitIndex+1)
	{
		childNodes.push_back(newNode->pageNum);
	}
	else
	{
		childNodes.insert(childNodes.begin()+nodeToSplitIndex+1,newNode->pageNum);		
	}

	writeNodes({this,nodeToSplit});
	delete newNode;
}

void Node::writeNodes(std::initializer_list<Node*> nodeList)
{
	for(Node* n:nodeList)
	{
		n->writeNode();
	}
}

bool Node::isOverPopulated()
{
	return dal->isOverPopulated(this);
}

Node::~Node(){}
