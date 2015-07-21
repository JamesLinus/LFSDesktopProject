/*
	lfstoolkit.cpp
	©keithhedger Tue 21 Jul 20:42:36 BST 2015 kdhedger68713@gmail.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int	LFSTK_testint=100;

void AAAouttest(char* name)
{
	printf("Hello World NOT to %s!\n",name);
}


extern "C" void LFSTK_outHW(char* name)
{
	printf("Hello World to %s\n",name);
}
