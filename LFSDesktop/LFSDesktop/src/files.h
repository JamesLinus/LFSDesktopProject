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

extern char		*diskInfoPath;
extern char		*cachePath;
extern Pixmap	diskIconsPixmap[20][2];
extern Pixmap	diskIconsPixmapMask[20][2];

int fileExists(char *name);
void makeImage(char *imagepath,char *destname,int disktype);

#endif
