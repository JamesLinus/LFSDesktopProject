/******************************************************
*
*     ©keithhedger Fri 22 May 14:47:56 BST 2015
*     kdhedger68713@gmail.com
*
*     globals.cpp
* 
******************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"

int			errLine;
const char	*errFile;
const char	*errFunc;

//save/load file
//global file data
char		*fileDiskLabel=NULL;
char		*fileDiskMime=NULL;
char		*fileDiskPath=NULL;
char		*fileDiskUUID=NULL;
char		*fileDiskType=NULL;
char		*fileCustomIcon=NULL;
int			fileDiskXPos=-1;
int			fileDiskYPos=-1;

int			foundDiskNumber=-1;
bool		isDisk=false;
bool		debugDeskFlag=false;

int			xCnt;
int			yCnt;

args		globalFileData[]=
{
	{"label",TYPESTRING,&fileDiskLabel},
	{"mime",TYPESTRING,&fileDiskMime},
	{"path",TYPESTRING,&fileDiskPath},
	{"uuid",TYPESTRING,&fileDiskUUID},
	{"type",TYPESTRING,&fileDiskType},
	{"icon",TYPESTRING,&fileCustomIcon},
	{"xpos",TYPEINT,&fileDiskXPos},
	{"ypos",TYPEINT,&fileDiskYPos},

	{NULL,0,NULL}
};


bool shapeset=false;

int			deskIconsCnt=0;
int			deskIconsMaxCnt=30;
deskIcons	*deskIconsArray;

char		*rootDev=NULL;
