/* Lib.c */
#include <stdio.h>
//gcc -fPIC -shared -o Lib.so Lib.c
void foobar(int i)
{
	printf("Printing from Lib.so %d\n", i);
   //sleep(-1);
}

int add(int a,int b)
{
	return a+b;
}

