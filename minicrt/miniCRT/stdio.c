// stdio.c
#include "minicrt.h"

long mini_crt_io_init()
{
	return 1;
}

#ifdef WIN32
#include <Windows.h>

FILE* fopen( const char* filename,const char* mode)
{
	HANDLE hFile = 0;
	long access = 0;
	long creation = 0;

	if(strcmp(mode,"w") == 0) {
		access |= GENERIC_WRITE;
		creation |= CREATE_ALWAYS;
	}

	if(strcmp(mode,"w+") == 0) {
		access |= GENERIC_WRITE |GENERIC_READ;
		creation |= CREATE_ALWAYS;
	}

	if(strcmp(mode,"r") == 0) {
		access |= GENERIC_READ;
		creation +=OPEN_EXISTING;
	}

	if(strcmp(mode,"r+") == 0) {
		access |= GENERIC_WRITE |GENERIC_READ;
		creation |= TRUNCATE_EXISTING;
	}

	hFile = CreateFileA(filename,access,0,0,creation,0,0);
	if(hFile == INVALID_HANDLE_VALUE) {
		return 0;
	}
	return (FILE*)hFile;
}

long fread(void* buffer,long size,long count,FILE* stream)
{
	long read = 0;
	if(!ReadFile( (HANDLE)stream,buffer,size * count,&read,0)) {
		return 0;
	}
	return read;
}

long fwrite(const void* buffer,long size,long count,FILE* stream)
{
	int written = 0;
	if(!WriteFile( (HANDLE)stream,buffer,size* count,&written,0)) {
		return 0;
	}
	return written;
}

long fclose(FILE* fp)
{
	return CloseHandle( (HANDLE)fp);
}

long fseek(FILE* fp,long offset,long set)
{
	return SetFilePointer( (HANDLE)fp,offset,0,set);
}

#else // #ifdef WIN32
/*
语法
内嵌汇编语法如下：
__asm__(
汇编语句模板: 
输出部分: 
输入部分: 
破坏描述部分)

共四个部分：汇编语句模板，输出部分，输入部分，破坏描述部分，各部分使用“:”格开，汇编语句模板必不可少，其他三部分可选，如果使用了后面的部分，而前面部分为空，也需要用“:”格开，相应部分内容为空。例如：
__asm__ __volatile__(
"cli":
:
:"memory")
*/
static long open(const char* pathname,long flags,long mode)
{
	long fd = 0;
	asm("movq $5,%%rax		\n\t"
		"movq %1,%%rdi		\n\t"
		"movq %2,%%rsi		\n\t"
		"movq %3,%%rdx		\n\t"
		"syscall			\n\t"
		"movq %%rax,%0		\n\t":
		"=m"(fd):"m"(pathname),"m"(flags),"m"(mode));
}

static long read(long fd,void* buffer, unsigned size)
{
	long ret = 0;
	asm("movq $3,%%rax	\n\t"
		"movq %1,%%rdi	\n\t"
		"movq %2,%%rsi	\n\t"
		"movq %3,%%rdx	\n\t"
		"syscall	\n\t"
		"movq %%rax,%0	\n\t"
		:"=m"(ret):"m"(fd),"m"(buffer),"m"(size));
	return ret;
}

static long write(long fd,const void* buffer,long size)
{
	long ret = 0;
	asm("movq $1,%%rax	\n\t"
		"movq %1,%%rdi	\n\t"
		"movq %2,%%rsi	\n\t"
		"movq %3,%%rdx	\n\t"
		"syscall	\n\t"
		"movq %%rax,%0	\n\t"
		:"=m"(ret):"m"(fd),"m"(buffer),"m"(size));
	return ret;
}

static long close(long fd)
{
	long ret = 0;
	asm("movq $6,%%rax	\n\t"
		"movq %1,%%rdi	\n\t"
		"syscall	\n\t"
		"movq %%rax,%0	\n\t"
		:"=m"(ret):"m"(fd));
	return ret;
}

static long seek(long fd,long offset, long mode)
{
	long ret = 0;
	asm("movq $19,%%rax	\n\t"
		"movq %1,%%rdi	\n\t"
		"movq %2,%%rsi	\n\t"
		"movq %3,%%rdx	\n\t"
		"syscall	\n\t"
		"movq %%rax,%0	\n\t"
		:"=m"(ret):"m"(fd),"m"(offset),"m"(mode));
	return ret;
}

FILE *fopen(const char* filenmae,const char* mode)
{
	long fd = -1;
	long flags = 0;
	long access = 00700;	//

	//begin as 0 is 8 
	#define O_RDONLY	00
	#define O_WRONLY	01
	#define O_RDWR		02
	#define O_CREAT		0100
	#define O_TRUNC		01000
	#define O_APPEND	02000


	if(strcmp(mode,"w") == 0) {
		flags |= O_WRONLY | O_CREAT | O_TRUNC;
	}

	if(strcmp(mode,"w+") == 0) {
		flags |= O_RDWR | O_CREAT | O_TRUNC;
	}

	if(strcmp(mode,"r") == 0) {
		flags |= O_RDONLY;
	}

	if(strcmp(mode,"r+") == 0) {
		flags |= O_RDWR | O_CREAT;
	}

	fd = open(filenmae,flags,access);
	return (FILE*)fd;
}

long fread(void* buffer,long size, long count, FILE* stream)
{
	return read( (long)stream,buffer,size * count);
}

long fwrite(const void* buffer, long size, long count,FILE* stream)
{
	return write( (long)stream,buffer,size * count);
}

long fclose(FILE* fp)
{
	return close( (long)fp);
}

long fseek(FILE* fp,long offset, long set)
{
	return seek( (long)fp,offset,set);
}

#endif
