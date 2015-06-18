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
#include <search.h>

#include "globals.h"
#include "graphics.h"

extern char		*diskInfoPath;
extern char		*cachePath;
extern char		*prefsPath;
extern char		*desktopPath;

extern hsearch_data	hashtab;

int fileExists(char *name);
void makeImage(char *imagepath,diskIconStruct *hashdata);
char* pathToIcon(char* name,const char* catagory);
void makeDiskInfofile(char* diskfilepath,char* label,char* uuid,int x,int y,char* type);
void readDesktopFile(const char* name);
void saveInfofile(int where,char* label,char* mime,char* path,char* uuid,char* type,int x, int y,int iconnum);
void getFreeSlot(int *x,int *y);
char* getMimeType(char *filepath);
char* oneLiner(const char* fmt,...);
void debugFunc(const char *fmt, ...);

#endif
