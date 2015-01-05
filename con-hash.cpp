#include <iostream>
#include <string.h>
#include <map>
#include <set>
#include "md5.h"
#include <stdio.h>
#include <stdlib.h>
#include "avl/avl.h"
using namespace std;


class HashFunc
{
public:
	virtual ~HashFunc() {}
	virtual int hash_value(const char* str) = 0; //计算hash值，32bit
};

class MD5HashFunc: public HashFunc
{
public:
	virtual int hash_value(const char* str)
	{
		long hash = 0;
		char digest[16];
		md5_string((const unsigned char*)str, digest, strlen(str));
		for (int i = 0; i < 4; ++i) {
			hash += (digest[i*4+3]&0xFF << 24)
					| (digest[i*4+2]&0xFF << 16)
					| (digest[i*4+1]&0xFF << 8)
					| (digest[i*4]&0xFF);
		}

		return hash;
	}
};

// real node
class RNode
{
public:
	RNode(const char* ident, int vNodeNum, void* data): m_vNodeNum(vNodeNum), m_data(data) {
		memset(m_ident, 0x00, sizeof(m_ident));
		strncpy(m_ident, ident, strlen(ident));
		printf("%s RNode() \n", m_ident);
	}
	~RNode() {
		printf("%s ~RNode() \n", m_ident);
	}

	const char* get_ident() const {
		return m_ident;
	}
	const int get_nodenum() const {
		return m_vNodeNum;
	}
	void* get_data() {
		return m_data;
	}
	void set_data(void* data) {
		m_data = data;
	}

private:
	RNode(const RNode&);
	RNode operator= (const RNode&);

private:
	char m_ident[128]; // 标识串
	int m_vNodeNum; // 可以对应的vNode数目
	void* m_data; // 节点数据
};

// virtual node
class VNode
{
public:
	VNode(const int hash, const RNode* node): m_hash(hash), m_rNode(node){
//		printf("VNode() \n");
	}
	~VNode() {
//		printf("~VNode() \n");
	}

	void set_rNode(const RNode* node) {
		m_rNode = node;
	}

	const RNode* get_rNode() const {
		return m_rNode;
	}

	long int get_hash() const {
		return m_hash;
	}
public:
	// 重载运算符，以便支持 AVL树
	bool operator < (const VNode& v) {
		return m_hash < v.m_hash;
	}
	bool operator > (const VNode& v) {
			return m_hash > v.m_hash;
		}
	bool operator == (const VNode& v) {
		return m_hash == v.m_hash;
	}
	bool operator >= (const VNode& v) {
		return m_hash >= v.m_hash;
	}

private:
	const int m_hash;
	const RNode* m_rNode; // a pointer to Real node
};


void pfunc(const void* v) {
	VNode & pv = *(VNode*)v;
	printf("[%9d]-vNode[%p]-rNode[%p]: identR[%s]\n", (int)pv.get_hash(), (void*)&pv, pv.get_rNode(), pv.get_rNode()->get_ident());
}


class CContHash
{
public:
	CContHash(HashFunc* func):
		m_func(func),
		m_avltree(pfunc)
	{
	}
	~CContHash() {
		Exit();
	}

	bool add_rNode(const RNode* pRNode) {
		int pLen = strlen(pRNode->get_ident());
		char buf[256] = {0};
		strncpy(buf, pRNode->get_ident(), pLen);
		const int num = pRNode->get_nodenum();
		for (int i = 0; i < num; ++i) {
			sprintf(buf+pLen, "%d", i);
			int hash = m_func->hash_value(buf);
			VNode* pVNode = new VNode(hash, pRNode);
			printf("pVNode new[%p] \n", pVNode);
			m_avltree.Insert(*pVNode);
			m_set.insert(pVNode);
		}
		return true;
	}
	bool del_rNode(const RNode* pRNode) {
		int pLen = strlen(pRNode->get_ident());
		char buf[256] = {0};
		strncpy(buf, pRNode->get_ident(), pLen);
		const int num = pRNode->get_nodenum();
		for(int i = 0; i < num; ++i) {
			sprintf(buf+pLen, "%d", i);
			int hash = m_func->hash_value(buf);
			VNode node(hash, NULL);
			AVL_tree<VNode&>::AVLNode* p = m_avltree.Find(node);
			if (p != NULL) {
				m_avltree.Delete(node);
			}

		}
		return true;
	}

	const RNode* router (const char* str) {
		int hash = m_func->hash_value(str);
		VNode node(hash, NULL);
		const AVL_tree<VNode&>::AVLNode* p = m_avltree.Find_conhash(node);
		if (p != NULL) {
			return p->data.get_rNode();
		}

		return NULL;
	}

	void Print() {
		m_avltree.Inorder_traversal();
	}

	void Exit() {
		std::set<VNode*>::iterator it = m_set.begin();
		for (; it != m_set.end(); ++it) {
			delete *it;
		}
	}
private:
	HashFunc* m_func; // hash函数
	AVL_tree<VNode&> m_avltree; // 使用avl树进行查找
	std::set<VNode*> m_set;
};

void test() {
	MD5HashFunc hashfun;
	RNode* pRNode1 = new RNode("192.168.1.1", 5, NULL);
	RNode* pRNode2 = new RNode("192.168.1.2", 5, NULL);
	RNode* pRNode3 = new RNode("192.168.1.3", 5, NULL);
	RNode* pRNode4 = new RNode("192.168.1.4", 6, NULL);
	RNode* pRNode5 = new RNode("192.168.1.5", 5, NULL);

	CContHash co(&hashfun);
	co.add_rNode(pRNode1);
	co.add_rNode(pRNode2);
	co.add_rNode(pRNode3);
	co.add_rNode(pRNode4);
	co.add_rNode(pRNode5);

	co.Print();
	printf("------------\n");
	co.del_rNode(pRNode4);
	co.Print();
	delete pRNode4;

	for (int i = 0; i < 30; ++i) {
		char buf[8] = {0};
		sprintf(buf, "#%.2d: ", i);
		const RNode* p = co.router(buf);
		int hash = hashfun.hash_value(buf);
		printf("[%s] [%9d] [%s] \n", buf, hash, p->get_ident());
	}

	delete pRNode1;
	delete pRNode2;
	delete pRNode3;

	delete pRNode5;
}
