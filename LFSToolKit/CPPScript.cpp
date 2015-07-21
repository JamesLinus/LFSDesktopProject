#if 0

#©keithhedger Mon 20 Jul 14:09:10 BST 2015 kdhedger68713@gmail.com

g++ "$0" -L/usr/local/lib -llfstoolkit||exit 1
./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif
//extern "C" char* outHW(char* name);
//extern "C" void AAAouttest(char* name);

#include <stdio.h>
extern "C" void LFSTK_outHW(char* name);
void AAAouttest(char* name);

extern int	LFSTK_testint;
int main(int argc, char **argv)
{
	printf("Hello World!\n");
	printf("Hello World! %i\n",LFSTK_testint);
	LFSTK_outHW((char*)"keith");
	AAAouttest((char*)"keith");
	return 0;
}