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



extern fileInfo	*fileInfoPtr;
extern int		desktopFilesCnt;

extern char		*diskInfoPath;
extern char		*cachePath;
extern char		*prefsPath;
extern int		savedFileCount;

//extern args		diskData[];
//extern char		*diskName;
//extern char		*diskUUID;
//extern int		diskXPos;
//extern int		diskYPos;
//extern char		*diskType;

extern hsearch_data	hashtab;

int fileExists(char *name);
void makeImage(char *imagepath,char *destname,diskIconStruct *hashdata);
char* pathToIcon(char* name,const char* catagory);
void makeDiskInfofile(char* diskfilepath,char* label,char* uuid,int x,int y,char* type);
void getSavedDiskData(void);
void readDesktopFile(const char* name);
void printString(char* str);
void printInt(int v);

#endif
