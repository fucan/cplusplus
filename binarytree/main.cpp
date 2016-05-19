#include "binarytree.h"

int main()
{
	BinaryTree<int>  tree;
	for (int i = 0; i < 10; i++)
	{
		tree.AddNode(i);
	}
	tree.PreOrder();

}