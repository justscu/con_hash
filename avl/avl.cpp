#include <stdio.h>
#include "avl.h"
///////////

void pf(const void* a) {
	printf("%2d ", *(int*)a);
}

void Test_AVL() {
	AVL_tree<int> avltree(&pf);

	//
	int data[] = {1,4,9,10,14,15,16,17,18,2,3,19,20,11,12,5,6,7,8,13, 21, 22, 24, 23, 6, 9};
	for (unsigned int i = 0; i < sizeof(data)/sizeof(int); ++i) {
		avltree.Insert(data[i]);
	}

	avltree.Preorder_traversal();printf("\n");
	printf("Inorder_traversal:");
	avltree.Inorder_traversal();
	printf("\n");

	avltree.Delete(6);
	avltree.Inorder_traversal();
	printf("\n");
	avltree.Delete(9);
	avltree.Inorder_traversal();
	printf("\n");

	avltree.Delete(6);
	avltree.Inorder_traversal();
	printf("\n");

	int b[] = {4, 5, 6, 7};
	for (unsigned int i = 0; i < sizeof(b)/sizeof(int); ++i) {
		if (avltree.Find(b[i]) != NULL) {
			printf("find [%d] \n", b[i]);
		} else {
			printf("not find [%d] \n", b[i]);
		}
	}
}

