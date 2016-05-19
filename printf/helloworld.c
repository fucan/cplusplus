void printf(char* fmt,...)
{
	_strwrite(fmt);
	return;
}

void _strwrite(char* str)
{
	char* p_strdst=(char*)(0xb8000);
	while(*str)
	{
		*p_strdst=*str++;
		p_strdst+=2;
	}
	return;
}

void main()
{
	printf("hello world!\n");
	return;
}