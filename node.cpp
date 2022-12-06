#include "node.h"
#include "defs.h"
#include <string>
#include <vector>
#include <initializer_list>

Item::Item(const char* k,const char* v):key(k),value(v){}

Item::~Item(){}

bool isFirst(int index)
{
	return index==0;
}

bool isLast(int index,Node* parentNode)
{
	return index==parentNode->items.size();
}

//p是根结点，b是underflow结点，a是b的左结点，bNodeIndex是bNode在p的childNode中的下标
void rotateRight(Node* aNode,Node* pNode,Node* bNode,int bNodeIndex)
{
	//将左子树的尾Item移到根结点相应位置
	Item* aNodeItem=aNode->items.back();
	aNode->items.pop_back();
	int pNodeItemIndex=bNodeIndex-1;
	if(isFirst(bNodeIndex))
		pNodeItemIndex=0;
	Item* pNodeItem=pNode->items[pNodeItemIndex];
	pNode->items[pNodeItemIndex]=aNodeItem;

	//把根结点移出的Item移到右子树的头部
	bNode->items.insert(bNode->items.begin(),pNodeItem);

	//调整a和b的子结点
	if(!aNode->isLeaf())
	{
		PageNum childNodeToShift=aNode->childNodes.back();
		aNode->childNodes.pop_back();
		bNode->childNodes.insert(bNode->childNodes.begin(),childNodeToShift);
	}
}

//p是根结点，a是underflow结点，b是a的右结点，aNodeIndex是aNode在p的childNode中的下标
void rotateLeft(Node* aNode,Node* pNode,Node* bNode,int aNodeIndex)
{
	//将右子树的头Item移到根结点相应位置
	Item* bNodeItem=bNode->items.front();
	bNode->items.erase(bNode->items.begin());
	int pNodeItemIndex=aNodeIndex;
	if(isLast(aNodeIndex,pNode))
		pNodeItemIndex=pNode->items.size()-1;
	Item* pNodeItem=pNode->items[pNodeItemIndex];
	pNode->items[pNodeItemIndex]=bNodeItem;

	//把根结点移出的Item移到右子树的头部
	aNode->items.push_back(pNodeItem);

	//调整a和b的子结点
	if(!bNode->isLeaf())
	{
		PageNum childNodeToShift=bNode->childNodes.front();
		bNode->childNodes.erase(bNode->childNodes.begin());
		aNode->childNodes.push_back(childNodeToShift);
	}
}

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
	int rightPos=PageSize-1;
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

		size_t kLen=strlen(item->key);
		size_t vLen=strlen(item->value);

		uint16_t offset=(uint16_t)(rightPos-kLen-vLen-2);//右边存子结点信息（key长度、key、value长度、value）
		memcpy(leftPos,&offset,2);
		leftPos+=2;

		rightPos-=vLen;
		memcpy(buf+rightPos,item->value,vLen);

		--rightPos;
		buf[rightPos]=(char)vLen;

		rightPos-=kLen;
		memcpy(buf+rightPos,item->key,kLen);

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

		//std::string key(buf+offset,kLen);
		char* key=(char*)malloc(kLen*sizeof(char)); 
		memcpy(key,buf+offset,kLen);
		*(key+kLen)=0;
		offset+=kLen;

		uint16_t vLen=0;
		memcpy(&vLen,buf+offset,1);
		++offset;

		char* value=(char*)malloc(vLen*sizeof(char)); 
		memcpy(value,buf+offset,vLen);
		*(value+vLen)=0;
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

std::pair<bool,int> Node::findKeyInNode(const char* key)
{
	for(size_t i=0;i<items.size();++i)
	{
		if(strcmp(items[i]->key,key)==0)
		{
			return {true,i};
		}
		if(strcmp(items[i]->key,key)>0)//找到第一个比待查key大的Item
		{
			return {false,i};
		}
	}
	return {false,items.size()};//待查找的key比最后一个Item还大
}

std::pair<int,Node*> Node::findKeyHelper(const char* key,bool exact,std::vector<int>& ancestorsIndexes)
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

std::pair<int,Node*> Node::findKey(const char* key,bool exact,std::vector<int>& ancestorIndexes)
{
	ancestorIndexes=std::vector<int>{0};
	std::pair<int,Node*> in=findKeyHelper(key,exact,ancestorIndexes);
	int index=in.first;
	Node* node=in.second;
	return {index,node};
}

Node* Node::getNode(PageNum pagenum)
{
	return tx->getNode(pagenum);
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
	return tx->writeNode(this);

}

int Node::elementSize(int i)
{
	int size=0;
	size+=strlen(items[i]->key);
	size+=strlen(items[i]->value);
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
	int splitIndex=nodeToSplit->tx->db->dal->getSplitIndex(nodeToSplit);

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
	return tx->db->dal->isOverPopulated(this);
}

void Node::removeItemFromLeaf(int index)
{
	items.erase(items.begin()+index);
	writeNode();
}

std::vector<int> Node::removeItemFromInternal(int index)
{
	std::vector<int> affectedNodes;
	affectedNodes.push_back(index);

	Node* aNode=getNode(childNodes[index]);

	//用被删Item左子树上的最大Item替换空位（迭代到叶子结点，即小于被删Item的最大Item）
	while(!aNode->isLeaf())
	{
		int traversingIndex=childNodes.size()-1;
		aNode=getNode(aNode->childNodes[traversingIndex]);
		affectedNodes.push_back(traversingIndex);
	}

	items[index]=aNode->items.back();
	aNode->items.pop_back();
	writeNodes({this,aNode});

	return affectedNodes;
}

//将bNode元素合并到它的左子树上
void Node::merge(Node* bNode,int bNodeIndex)
{
	//bNode的左兄弟
	Node* aNode=getNode(childNodes[bNodeIndex-1]);

	//aNode和bNode的公共父Item
	Item* pNodeItem=items[bNodeIndex-1];
	items.erase(items.begin()+bNodeIndex-1);
	aNode->items.push_back(pNodeItem);

	for(int i=0;i<bNode->items.size();++i)
		aNode->items.push_back(bNode->items[i]);
	if(!aNode->isLeaf())
	{
		for(int i=0;i<bNode->childNodes.size();++i)
			aNode->childNodes.push_back(bNode->childNodes[i]);
	}

	writeNodes({aNode,this});
	tx->db->dal->deleteNode(bNode->pageNum);
}

bool Node::canSpareAnElement()
{
	int splitIndex=tx->db->dal->getSplitIndex(this);
	if(splitIndex==-1) return false;
	return true;
}

//this是unbalancedNode的父结点
void Node::rebalanceRemove(Node* unbalancedNode,int unbalancedNodeIndex)
{
	Node* pNode=this;

	//unbalancedNode不是最左边的结点则右旋
	if(unbalancedNodeIndex!=0)
	{
		Node* leftNode=getNode(childNodes.back());
		if(leftNode->canSpareAnElement())
		{
			rotateRight(leftNode,pNode,unbalancedNode,unbalancedNodeIndex);
			writeNodes({leftNode,pNode,unbalancedNode});
			return;
		}
	}

	//unbalancedNode不是最右边的结点则左旋
	if(unbalancedNodeIndex!=pNode->childNodes.size()-1)
	{
		Node* rightNode=getNode(childNodes[unbalancedNodeIndex+1]);
		if(rightNode->canSpareAnElement())
		{
			rotateLeft(unbalancedNode,pNode,rightNode,unbalancedNodeIndex);
			writeNodes({unbalancedNode,pNode,rightNode});
			return;
		}
	}

	//unbalancedNode在最左边且其右兄弟无法提供左旋，则与右兄弟合并
	if(unbalancedNodeIndex==0)
	{
		Node* rightNode=getNode(childNodes[unbalancedNodeIndex+1]);
		return pNode->merge(rightNode,unbalancedNodeIndex+1);
	}

	//unbalancedNode不在最左边且其右兄弟无法提供左旋，则与左兄弟合并
	return pNode->merge(unbalancedNode,unbalancedNodeIndex);
}

bool Node::isUnderPopulated()
{
	return tx->db->dal->isUnderPopulated(this);
}

Node::~Node(){}

