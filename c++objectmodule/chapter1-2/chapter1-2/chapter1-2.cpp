// chapter1-2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

class A {
public:
	virtual ~A() {

	}
	//int a;
};
class X :public A{
public:
	X() {

	}
	virtual ~X() {

	}

	virtual void foo() {

	}
};


int _tmain(int argc, _TCHAR* argv[])
{
	X x;
	return 0;
}

