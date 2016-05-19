//entry.c
#include "minicrt.h"

#ifdef WIN32
#include <Windows.h>
#endif
extern int main(long argc,char* argv[]);
void exit(long);

static void crt_fatal_error(const char* msg)
{
	//printf("fatal error:%s",msg);
	exit(1);
}

void mini_crt_entry (void)
{
	//init
	long ret;
#ifdef WIN32
	long flag = 0;
	long argc = 0;
	char* argv[16];	// max 16
	char* cl = GetCommandLineA();
	//printf("%s\n",cl);
	//parse commandline
	argv[0] = cl;
	argc++;
	char* p=cl;
	while(*p) {
		if(*p == '\"') {
			*p = ' ';
		}
		p++;
	}
	while(*cl) {
		if(*cl ==' ' && flag == 0) {
			if(*(cl+1) && *(cl+1) != ' ') {
				//printf("%d\n",*(cl+1));
				argv[argc] = cl+1;
				argc++;
			}
			*cl = '\0';	//使用\0替换空格
		}
		cl++;
	}
#else
	long argc;
	char** argv;
	char* ebp_reg = 0;
	// ebp_reg = %ebp
	asm("movq %%rbp,%0 \n":"=r"(ebp_reg));

	argc = *(long*)(ebp_reg+8);
	argv = (char**)(ebp_reg+16);
#endif
	if(!mini_crt_heap_init()) {
		crt_fatal_error("heap initialize failed");
	}

	if(!mini_crt_io_init()) {
		crt_fatal_error("IO initialize failed");
	}

	do_global_ctors();
	
	ret=main(argc,argv);
	
	//end
	exit(ret);
}

void exit(long exitCode)
{
	mini_crt_call_exit_routine();

	#ifdef WIN32
		ExitProcess(exitCode);
	#else
		asm("movq %0,%%rbx \n\t"
			"movq $1,%%rax \n\t"
			"int $0x80	\n\t"
			"hlt		\n\t"::"m"(exitCode));
	#endif
}