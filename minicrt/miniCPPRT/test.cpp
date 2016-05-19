// test.cpp

#include "iostream"
#include "string"

using namespace std;

int main(int argc,char* argv[])
{
	string* msg = new string("hello world");
	cout << *msg <<endl;
	delete msg;
	printf("%s\n","fucanf");
	return 0;
}

/* Linux 
g++ -c -nostdinc++ -fno-rtti -fno-exceptions -ggdb -fno-builtin -nostdlib -fno-stack-protector test.cpp
ld -static -e mini_crt_entry entry.o crtbegin.o test.o minicrt.a crtend.o -o test
ls -l test
*/
/*	Windows
cl /c /DWIN32 /GR- test.cpp
link test.obj minicrt.lib kernel32.lib /NODEFAULTLIB /entry:mini_crt_entry
dir test.exe
*/