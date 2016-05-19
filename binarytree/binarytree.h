//the link: http://zh.wikipedia.org/zh-cn/%E4%BA%8C%E5%8F%89%E6%A0%91

#include <iostream>
using namespace std;

//node
template<class T>
struct TreeNode
{
	T data;
	TreeNode<T> *lchild, *rchild;

	//optional args
	/*TreeNode(T nodeValue = T(),TreeNode<T>*leftNode = NULL.TreeNode<T> *rightNode = NULL )
	:data(nodeValue),lchild(leftNode),rchild(rightNode){}*/
};

//create binary tree
template <class T>
class BinaryTree
{
public:
	TreeNode<T> *root;
	BinaryTree()
	{
		root = NULL;
	}
	void AddNode(T&);
	void PreOrder();
	void _PreOrder(TreeNode<T>*);   //before
	/*void Inorder(TreeNode<T>*);    //middle
	void PostOrder(TreeNode<T>*);  //last
	int GetNode(TreeNode<T>*);   //count the Node num
	int depth(TreeNode<T>*);    //get the tree deep
	int GetLeafNum(TreeNode<T>*);      //
	int GetNodeNum(TreeNode<T>*);      //
	void destroy(TreeNode<T>*);
	*/
};

template<class T>
void BinaryTree<T>::AddNode(T& x)
{
	TreeNode<T> *node = new TreeNode<T>();
	node->data = x;
	node->rchild = node->lchild = NULL;
	if (NULL == root)
	{
		root = node;
	}
	else
	{
		TreeNode<T> *back;
		TreeNode<T> *p = root;
		while (NULL != p)
		{
			back = p;
			if (p->data > x)
			{
				p = p->lchild;
			}
			else
			{
				p = p->rchild;
			}
			if (back->data > x)
			{
				back->lchild = node;
			}
			else
			{
				back->rchild = node;
			}
		}
		
	}
}

template <class T>
void BinaryTree<T>::PreOrder()
{
	if (NULL != root)
	{
		_PreOrder(root);
	}
}

template<class T>
void BinaryTree<T>::_PreOrder(TreeNode<T>* p)
{
	if (NULL != p)
	{
		cout << p->data << " ";
		_PreOrder(p->lchild);
		_PreOrder(p->rchild);
	}
}


