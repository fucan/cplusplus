//new_delete.cpp

#ifndef WIN32
typedef unsigned long size_t;
#endif
extern "C" void* malloc(unsigned long);
extern "C" void free(void*);

//对象的构造和析构是在new/delete之前/之后由编译器负责产生相应代码进行调用
//new/delete仅仅负责堆空间的申请和释放，不负责构造和析构
void* operator new(size_t size)
{
	return malloc(size);
}

void operator delete(void* p)
{
	free(p);
}

void* operator new[](size_t size)
{
	return malloc(size);
}

void operator delete[](void* p)
{
	free(p);
}

//虚析构函数的派生类必须要第二个参数
//Windows 下需要加这个函数
void operator delete(void*p,size_t size)
{
	free(p);
}