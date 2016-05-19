#include "minicrt.h"
//#include "windows.h"

int main(int argc,char* argv[])
{
	int i;
	FILE* fp;
	char** v = malloc(argc*sizeof(char*));
	for(i = 0; i < argc;++i) {
		v[i] = malloc(strlen(argv[i]) + 1);
		strcpy(v[i],argv[i]);
	}
	int size=sizeof(long);
	fp = fopen("test.txt","w");
	for(i = 0; i < argc; ++i) {
		long len = strlen(v[i]);
		int count=fwrite(&len,1,sizeof(long),fp);
		fwrite(v[i],1,len,fp);
	}
	fclose(fp);

	fp = fopen("test.txt","r");

	for(i =0; i < argc; ++i) {
		long len;
		char* buf;
		int count=fread(&len,1,sizeof(long),fp);
		buf = malloc(len+1);
		fread(buf,1,len,fp);
		buf[len] = '\0';
		printf("%d %s\n",len,buf);
		free(buf);
		free(v[i]);
	}
	fclose(fp);
}

/* Linux 
gcc -c -ggdb -fno-builtin -nostdlib -fno-stack-protector test.c
ld -static -e mini_crt_entry entry.o test.o minicrt.a -o test
ls -l test
*/
/*	Windows
cl /c /DWIN32 test.c
link test.obj minicrt.lib kernel32.lib /NODEFAULTLIB /entry:mini_crt_entry
dir test.exe
*/
