/* b.c */
int shared = 1;
int g_init = 2;
void swap(int* a,int* b )
{
    *a^=*b^=*a^=*b;
}

int add(int a,int b)
{
    shared=2;
    return a+b;
}
