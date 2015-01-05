#pragma once
#include <stdio.h>

// 声明打印函数
typedef void (*FUNPRINT) (const void* data);

template<class TYPE>
class AVL_tree
{
public:
	struct AVLNode {
		const TYPE& data;
		int hight; //高度
		unsigned int times; // 出现次数
		AVLNode* LChild;
		AVLNode* RChild;
	public:
		AVLNode(const TYPE& d):data(d), hight(0), times(1), LChild(NULL), RChild(NULL) {
		}
		~AVLNode() {}
	private:
		AVLNode(const AVLNode&);
		AVLNode& operator= (const AVLNode&);
	};

public:
	AVL_tree(FUNPRINT func):m_printFunc(func), m_pTop(NULL) {
		printf("AVL_tree() \n");
	}
	~AVL_tree() {
		printf("~AVL_tree() \n ");
		destroy(m_pTop);
	}
	bool Insert(const TYPE& data) {
		m_pTop = insert(m_pTop, data);
		return m_pTop == NULL;
	}
	bool Delete(const TYPE& data) {
		m_pTop = _delete(m_pTop, data);
		return true;
	}
	AVLNode* Find(const TYPE& data) {
		return find(m_pTop, data);
	}
	// 一致性hash的查找函数
	AVLNode* Find_conhash(const TYPE& data) {
		return find_conhash(m_pTop, data);
	}

	void Preorder_traversal() {
		preorder_traversal(m_pTop);
	}
	void Inorder_traversal() {
		inorder_traversal(m_pTop);
	}
	void Lastorder_traversal() {
		lastorder_traversal(m_pTop);
	}

private:
	FUNPRINT m_printFunc;
	AVLNode* m_pTop; // avl树的顶节点

private:
	// 插入，返回值为新的top节点
	AVLNode* insert(AVLNode* pTop, const TYPE& data) {
		if (pTop == NULL) {
			pTop = new AVLNode(data);
			if (pTop == NULL) {
				return NULL;
			}

			return pTop;
		}

		// 插入左子树
		if (pTop->data > data) {
			pTop->LChild = insert(pTop->LChild, data);
			// 失衡
			if (hight(pTop->LChild)-hight(pTop->RChild) == 2) {
				if (pTop->LChild->data > data) {
					pTop = r_Rotate(pTop); // (1)根节点的左子树的左子树插入，需右旋
				} else {
					pTop = lr_Rotate(pTop); // (2)根节点的左子树的右子树插入，需先左旋后右旋
				}
			}
		// 插入右子树
		} else if (pTop->data < data) {
			pTop->RChild = insert(pTop->RChild, data);
			// 失衡
			if (hight(pTop->LChild)-hight(pTop->RChild) == -2) {
				if (pTop->RChild->data < data) {
					pTop = l_Rotate(pTop); // (3)根节点的右子树的右子树，需左旋
				} else {
					pTop = rl_Rotate(pTop);	// (4)根节点的右子树的左子树，需先右旋后左旋
				}
			}
		} else {
			pTop->times++;
		}

		max(hight(pTop->LChild), hight(pTop->RChild));
		pTop->hight = max(hight(pTop->LChild), hight(pTop->RChild)) + 1;
		return pTop;
	}
	// 删除，返回值为新的top节点
	// @data为要删除的节点
	// out，将保存在AVL树上的数据传回
	AVLNode* _delete(AVLNode* pTop, const TYPE& data) {
		if (pTop == NULL)
			return NULL;
		// 找到了要删除的节点，引用次数>1，修改引用次数
		if (pTop->data == data && pTop->times > 1) { // -1 就可以了，
			--pTop->times;
		// 引用次数为1, 删除该节点
		} else if (pTop->data == data) {
			AVLNode* t = pTop;
			AVLNode* minR = getMinNode(pTop->RChild); // 右孩子最小节点
			if (minR == NULL) { // 右子树不存在，左子树肯定只有一个节点或没有节点
				pTop = pTop->LChild;
			} else { // 右子树存在
				if (pTop->RChild->LChild == NULL) { // 没有左子树
					pTop = pTop->RChild;
					pTop->LChild = t->LChild;
				} else {
					AVLNode* par = getParent(pTop->RChild, minR->data);
					par->LChild = NULL;
					minR->LChild = pTop->LChild;
					minR->RChild = pTop->RChild;
				}
				pTop = minR;
			}

			printf("delete: ");
			m_printFunc(&t->data);
			printf("\n");
			delete t;

		} else if (pTop->data > data) { // 在左子树上删
			pTop->LChild = _delete(pTop->LChild, data);
			if (hight(pTop->LChild) - hight(pTop->RChild) == -2) {
				if (hight(pTop->RChild->LChild) < hight(pTop->RChild->RChild)) {
					pTop = l_Rotate(pTop);
				} else {
					pTop = rl_Rotate(pTop);
				}
			}
		} else { // 在右子树上删
			pTop->RChild = _delete(pTop->RChild, data);
			if (hight(pTop->LChild) - hight(pTop->RChild) == 2) {
				if (hight(pTop->LChild->LChild) > hight(pTop->LChild->RChild)) {
					pTop = r_Rotate(pTop);
				} else {
					pTop = lr_Rotate(pTop);
				}
			}
		}

		if ( pTop != NULL ) {
			pTop->hight = max(hight(pTop->LChild), hight(pTop->RChild)) + 1;
		}
		return pTop;
	}

	// 查找(递归)，返回值为找到的节点
	AVLNode* find2(AVLNode* pTop, const TYPE& data) {
		if (pTop == NULL || pTop->data == data)
			return pTop;
		if (pTop->data > data) {
			return find2(pTop->LChild, data);
		} else {
			return find2(pTop->RChild, data);
		}
	}
	// 查找(非递归)
	AVLNode* find(AVLNode* pTop, const TYPE& data) {
		AVLNode* p = pTop;
		while(p != NULL) {
			if(p->data == data) {
				return p;
			} else if (p->data > data){
				p = p->LChild;
			} else {
				p = p->RChild;
			}
		}

		return NULL;
	}
	// 一致性hash需要的查找算法
	AVLNode* find_conhash(AVLNode* pTop, const TYPE& data) {
		AVLNode* p = pTop;
		while(p != NULL) {
			if(p->data == data) {
				return p;
			} else if (p->data > data){
				if (p->LChild != NULL)
					p = p->LChild;
				else
					return p;
			} else {
				if (p->RChild != NULL)
					p = p->RChild;
				else
					return p;
			}
		}

		return NULL;
	}

private:
//////////// 遍历 //////////////////////
	// 先序遍历: 先根节点 -> 左节点 -> 右节点
	void preorder_traversal(const AVLNode* pTop) {
		if (pTop == NULL)
			return;

		if (m_printFunc) {
			m_printFunc(&pTop->data);
		}
		if (pTop->LChild != NULL) {
			preorder_traversal(pTop->LChild);
		}

		if (pTop->RChild != NULL) {
			preorder_traversal(pTop->RChild);
		}
	}

	// 中序遍历: 先左节点 -> 根节点 -> 右节点
	void  inorder_traversal(const AVLNode* pTop) {
		if (pTop == NULL)
			return;

		if (pTop->LChild != NULL)
			inorder_traversal(pTop->LChild);
		if (m_printFunc) {
			m_printFunc(&pTop->data);
		}
		if (pTop->RChild != NULL)
			inorder_traversal(pTop->RChild);
	}

	// 后序遍历: 先左节点 -> 右节点 -> 根节点
	void lastorder_traversal(const AVLNode* pTop) {
		if (pTop == NULL)
			return;

		if (pTop->LChild != NULL)
			lastorder_traversal(pTop->LChild);
		if (pTop->RChild != NULL)
			lastorder_traversal(pTop->RChild);
		if (m_printFunc) {
			m_printFunc(&pTop->data);
		}
	}

private:
	inline int max(int a, int b) {
		return a>=b?a:b;
	}

	inline int hight(const AVLNode* pNode) {
		if (pNode == NULL)
			return -1;
		else
			return pNode->hight;
	}

	// 获取data的父节点(递归)
	inline AVLNode* getParent2(AVLNode* pTop, const TYPE& data) {
		if (pTop == NULL || pTop->data == data)
			return NULL;
		AVLNode* node = pTop;
		if ((node->LChild && node->LChild->data == data) || (node->RChild && node->RChild->data == data))
			return node;
		else if (node->LChild && node->data > data)
			return getParent2(node->LChild, data);
		else // if (node->RChild && node->data < data)
			return getParent2(node->RChild, data);
	}
	// 获取data的父节点(非递归)
	inline AVLNode* getParent(AVLNode* pTop, const TYPE& data) {
		if (pTop == NULL || pTop->data == data)
			return NULL;

		AVLNode* node = pTop;
		while(node) {
			if ((node->LChild && node->LChild->data == data) || (node->RChild && node->RChild->data == data)) {
				return node;
			} else if (node->data > data && node->LChild != NULL)
				node = node->LChild;
			else if (node->data < data && node->RChild != NULL)
				node = node->RChild;
			else
				return NULL;
		}

		return node;
	}

	inline AVLNode* getMinNode(AVLNode* pTop) {
		if (pTop != NULL) {
			while ( pTop->LChild != NULL ) {
				pTop = pTop->LChild;
			}
		}

		return pTop;
	}

	inline AVLNode* getMaxNode(AVLNode* pTop) {
		if (pTop != NULL) {
			while ( pTop->RChild != NULL ) {
				pTop = pTop->RChild;
			}
		}

		return pTop;
	}
	// 删除分配的资源
	void destroy(AVLNode* pTop) {
		if (pTop != NULL) {
			if (pTop->LChild != NULL) {
				destroy(pTop->LChild);
			}
			if (pTop->RChild != NULL) {
				destroy(pTop->RChild);
			}
			delete pTop;
		}
	}

private:
	///////// rotate
	// 右旋 -- 左-左插入情况下的旋转：即在根节点的左子树的左子树上插入节点，需右旋
	AVLNode* r_Rotate(AVLNode* pTop) {
		if (pTop == NULL)
			return NULL;
		AVLNode* pRet = pTop->LChild;
		pTop->LChild = pRet->RChild;
		pRet->RChild = pTop;

		pTop->hight = max(hight(pTop->LChild), hight(pTop->RChild))+1;
		pRet->hight = max(hight(pRet->LChild), pTop->hight)+1;
		return pRet;
	}
	// 左旋 -- 右-右插入情况下的旋转：即在根节点的右子树的右子树上插入节点，需左旋
	AVLNode* l_Rotate(AVLNode* pTop) {
		if (pTop == NULL)
			return NULL;
		AVLNode* pRet = pTop->RChild;
		pTop->RChild = pRet->LChild;
		pRet->LChild = pTop;

		pTop->hight = max(hight(pTop->LChild), hight(pTop->RChild))+1;
		pRet->hight = max(hight(pRet->LChild), pTop->hight)+1;
		return pRet;
	}
	// 先左旋后右旋 -- 左-右插入插入情况下的旋转：即在根节点的左子树的右子树上插入节点，需先左旋后右旋
	AVLNode* lr_Rotate(AVLNode* pTop) {
		if (pTop == NULL)
			return NULL;
		pTop->LChild = l_Rotate(pTop->LChild);
		return r_Rotate(pTop);
	}
	// 先右旋后左旋 -- 右-左插入情况下的旋转：即在根节点的右子树的左子树上插入节点，需先右旋后左旋
	AVLNode* rl_Rotate(AVLNode* pTop) {
		if (pTop == NULL)
			return NULL;
		pTop->RChild = r_Rotate(pTop->RChild);
		return l_Rotate(pTop);
	}
};
