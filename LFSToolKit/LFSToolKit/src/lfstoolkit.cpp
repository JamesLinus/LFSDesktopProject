
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VISIBLE __attribute__((visibility("default")))
#define PROTECTED __attribute__((visibility("protected")))
#define HIDDEN __attribute__((visibility("hidden")))

int	testint=100;

void AAAouttest(char* name)
{
	printf("Hello World NOT to %s!\n",name);
}


void foo_outHW(char* name)
{
	printf("Hello World to %s\n",name);
}

class foo_testclass
{
	public:
		foo_testclass();
		~foo_testclass();
		int y;
};

foo_testclass::~foo_testclass()
{
}

foo_testclass::foo_testclass()
{
	y=12345;
}
