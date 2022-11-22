#include "dal.h"
#include "freelist.h"
#include "meta.h"
#include "page.h"
#include "node.h"
#include <string.h>

int main()
{
	Dal* dal=new Dal("./mainTest");
	
	Node* node=dal->getNode(dal->meta->root);
	node->dal=dal;
}
