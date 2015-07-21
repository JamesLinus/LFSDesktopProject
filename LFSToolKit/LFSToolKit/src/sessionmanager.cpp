/*
  sessionmanager.cpp
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PROTECTED __attribute__((visibility("protected")))

int	testint=100;

void AAAouttest(char* name)
{
	printf("Hello World NOT to %s!\n",name);
}


extern "C" void outHW(char* name)
{
	printf("Hello World to %s\n",name);
}
