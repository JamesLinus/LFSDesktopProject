/******************************************************
*
*     ©keithhedger Mon  4 May 13:20:31 BST 2015
*     kdhedger68713@gmail.com
*
*     files.h
* 
******************************************************/

#ifndef _FILES_
#define _FILES_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
inline bool fileExists(char *name)
{
	struct stat buffer;
	return (stat(name,&buffer)==0);
}
*/
extern bool fileExists(char *name);

#endif
