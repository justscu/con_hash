//============================================================================
// Name        : con-hash.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <string.h>
#include <iostream>
using namespace std;

int HASH(string str);

void test();
void Test_AVL();
int main()
{
//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
//
//	cout << HASH("1111111111111") << endl;
//	cout << HASH("111111111111111111111111") << endl;

	//	Test_AVL();
	test();
	return 0;
}


int HASH(string str) {
	const int p = 16777619;
	int hash = (int) 2166136261L;

	char * ptr = (char*)&str[0];
	for (unsigned int i = 0; i < strlen(ptr); ++i) {
		hash = (hash ^ptr[i])*p;
	}

	hash += hash << 13;
	hash ^= hash >> 7;
	hash += hash << 3;
	hash ^= hash >> 17;
	hash += hash << 5;
	return hash;
}
