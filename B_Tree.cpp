/*---------------------------------------
B树的创建和插入
---------------------------------------*/

#include <iostream>
#include <queue>
#define t 2              //B树的度
using namespace std;

/*---------------------------------------
定义B树结点
---------------------------------------*/
struct B_Tree_Node
{
	int key[2 * t - 1];     //关键字
	B_Tree_Node *p[2 * t];  //孩子指针
	bool is_leaf;           //是否为叶结点 
	int num;                //关键字个数
};


/*-------------------------------------------
模拟向磁盘写入结点
-------------------------------------------*/
void Disk_Write(B_Tree_Node *node)
{
	cout << "Write nodes into disk: ";
	for(int i = 0; i < node->num; i++)
	{
		cout << node->key[i] << " ";
	}
	cout << endl;
}


/*----------------------------------------
模拟从磁盘读取结点
----------------------------------------*/
void Disk_Read(B_Tree_Node *node)
{
	cout << "Read nodes from disk: ";
	for(int i = 0; i < node->num; i++)
	{
		cout << node->key[i] << " ";
	}
	cout << endl;
}


/*---------------------------------------------------
搜索B树
---------------------------------------------------*/
B_Tree_Node* B_Tree_Search(B_Tree_Node *r, int k)
{
	int i = 0;
	while(i < r->num && k > r->key[i])
		i++;
	if(i < r->num && k == r->key[i])
		return r;
	else if(r->is_leaf)
		return NULL;
	else
	{
		Disk_Read(r->p[i]);
		return B_Tree_Search(r->p[i], k);
	}
}

/*------------------------------------------------------
创建一颗空的B树
------------------------------------------------------*/
B_Tree_Node *B_Tree_Create()
{
	B_Tree_Node *r = new B_Tree_Node;
	if(r == NULL)
		return NULL;

	for(int i = 0; i < 2 * t - 1; i++)   //关键字为0
		r->key[i] = 0;
	for(int i = 0; i < 2 * t; i++)       //孩子指针为空 
		r->p[i] = NULL;

	r->num = 0;                          //关键字个数为0
	r->is_leaf = true;                   //为叶节点

    Disk_Write(r);
	return r;
}

/*----------------------------------------------------------------
分裂B树中的结点
----------------------------------------------------------------*/
void B_Tree_Split(B_Tree_Node *parent, int pos)
{
	B_Tree_Node *child1 = parent->p[pos];    //要分裂的满子结点
	B_Tree_Node *child2 = new B_Tree_Node;   //新子结点

    //将child1的t-1个关键字和t个孩子分给child2
	child2->num = t - 1;                     
	child2->is_leaf = child1->is_leaf;       
	for(int i = 0; i < t - 1; i++)
	{
		child2->key[i] = child1->key[i+t];
	}
	if(!child1->is_leaf)
	{
		for(int i = 0; i < t; i++)
		{
			child2->p[i] = child1->p[i+t];
		}
	}

	child1->num = t - 1;     //调整child1的关键字个数

    //将child2插入为parent的孩子
	for(int i = parent->num ; i >= pos + 1; i--)   
	{
		parent->p[i+1] = parent->p[i];
	}
	parent->p[pos+1] = child2;

	for(int i = parent->num - 1; i >= pos; i--)
	{
		parent->key[i+1] = parent->key[i];
	}
	parent->key[pos] = child1->key[t-1];
	parent->num += 1;

	Disk_Write(child1);
	Disk_Write(child2);
	Disk_Write(parent);
}

/*--------------------------------------------------------------------
根结点不为满时，递归插入关键字
--------------------------------------------------------------------*/
void B_Tree_Insert_Nonfull(B_Tree_Node *r, int k)
{
	int i = r->num - 1;

    /*处理r为叶结点的情况,直接将k插入合适的位置*/
	if(r->is_leaf)
	{
		while(i >= 0 && r->key[i] > k)
		{
			r->key[i+1] = r->key[i];
			i--;
		}
		r->key[i+1] = k;
		r->num += 1;

		Disk_Write(r);
	}

	else
	{
		while(i >= 0 && r->key[i] > k)   //决定向x的哪个子结点递归下降
		{
			i--;
		}
		i++;
        Disk_Read(r->p[i]);

		if(r->p[i]->num == 2 * t - 1)    //保证程序不会降在一个满结点上
		{
			B_Tree_Split(r, i);
			if(r->key[i] < k)
				i++;
		}
		B_Tree_Insert_Nonfull(r->p[i], k);  //递归地将k插入合适的子树中
	}
}


/*---------------------------------------------------------
向B树插入关键字
---------------------------------------------------------*/
B_Tree_Node* B_Tree_Insert(B_Tree_Node *r, int k)
{
	if(r->num == 2 * t - 1)      //处理根结点r为满的情况
	{
		B_Tree_Node *s = new B_Tree_Node;
		if(s == NULL)
			return NULL;

		s->is_leaf = false;
		s->num = 0;
		s->p[0] = r;
		B_Tree_Split(s, 0);
		B_Tree_Insert_Nonfull(s, k);
		return s;
	}
	else
	{
		B_Tree_Insert_Nonfull(r, k);
		return r;
	}
}

/*-------------------------------------------------
按层序遍历B树并打印
-------------------------------------------------*/
void Print_B_Tree(B_Tree_Node *r)
{
    if(r == NULL)
    {
    	return;
    }

    queue<B_Tree_Node*> q;
    q.push(r);
    int len = q.size();
    
    while(!q.empty())
    {
    	while(len--)
    	{
    	    B_Tree_Node *tmp = q.front();
    	    cout << " [ ";
    	    for(int i = 0; i < tmp->num; i++)
    	    {
    		    cout << tmp->key[i] << " ";
    	    }
    	    cout << " ] ";
    	    q.pop();

    	    if(!tmp->is_leaf)
    	    {
    	        for(int i = 0; i <= tmp->num; i++)
    	        {
    		        if(tmp->p[i])
    		        {
    			        q.push(tmp->p[i]);
    		        }
    	        }
    	    }
    	}

    	cout << endl;
    	len = q.size();
    }	
}

/*--------------------------------------------------------
测试程序
--------------------------------------------------------*/
int main(int argc, char const *argv[])
{
	B_Tree_Node *r = B_Tree_Create();

	int a[] = {18, 31, 12, 10, 15, 48, 
		45, 47, 50, 52, 23, 30, 20};

	for(int i = 0; i < 13; i++)
	{
		r = B_Tree_Insert(r, a[i]);
	}

	Print_B_Tree(r);

	return 0;
}