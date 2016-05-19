class HelloWorld
{
public:
	HelloWorld();
	~HelloWorld();
};

HelloWorld Hw;
HelloWorld::HelloWorld()
{
	volatile	int i;
}
HelloWorld::~HelloWorld()
{

}

int main()
{
	int ret;
	char msg[] ="Hello\n";
	__asm__ volatile (
		"call *%%esi"
		: "=a" (ret)
		: "a" (4),
		"S" (0xffffe400),
		"b" ((long) 1),
		"c" ((long) msg),
		"d" ((long) sizeof(msg)));
	return 0;
}