/******************************************************
*
*     ©keithhedger Mon  4 May 13:20:31 BST 2015
*     kdhedger68713@gmail.com
*
*     files.cpp
* 
******************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <search.h>

#include "globals.h"
#include "prefs.h"
#include "graphics.h"
#include "disks.h"
#include "files.h"


char		*diskInfoPath;
char		*cachePath;
char		*prefsPath;
int			savedFileCount;

//char		*diskName=NULL;
//char		*diskUUID=NULL;
//int			diskXPos=-1;
//int			diskYPos=-1;
//char		*diskType=NULL;

//diskInfo	*disksDataPtr;
fileInfo	*fileInfoPtr;

struct hsearch_data	hashtab;

//args			diskData[]=
//{
//	{"diskname",TYPESTRING,&diskName},
//	{"diskuuid",TYPESTRING,&diskUUID},
//	{"diskx",TYPEINT,&diskXPos},
//	{"disky",TYPEINT,&diskYPos},
//	{"type",TYPESTRING,&diskType},/
//
//	{NULL,0,NULL}
//};


int fileExists(char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}

char* defaultIcon(char *theme,char *name)
{
	char	*command;
	FILE	*fp;
	char	buffer[2048];
	char	*retstr=NULL;

	asprintf(&command,"find \"/usr/share/icons/%s\" \"%s/.icons/%s\" -iname \"*harddisk*.png\"  2>/dev/null|sort -nr -t \"x\"  -k 2.1|head -n1",theme,getenv("HOME"),theme,name);

	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,2048,fp);
			sscanf(buffer,"%as",&retstr);
			pclose(fp);
		}
	if((retstr==NULL) || (strlen(retstr)==0))
		{
			if(retstr!=NULL)
				free(retstr);
			return(defaultIcon((char*)"gnome",name));
		}
	return(retstr);
}

char* pathToIcon(char* name,const char* catagory)
{
	char	*command;
	FILE	*fp;
	char	buffer[2048];
	char	*retstr=NULL;

	asprintf(&command,"find \"/usr/share/icons/%s\" \"%s/.icons/%s\" -iname \"*%s.png\"  2>/dev/null|grep -i \"%s\"|sort -nr -t \"x\"  -k 2.1|head -n1",iconTheme,getenv("HOME"),iconTheme,name,catagory);

	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,2048,fp);
			sscanf(buffer,"%as",&retstr);
			pclose(fp);
		}
	if((retstr==NULL) || (strlen(retstr)==0))
		{
			if(retstr!=NULL)
				free(retstr);
			return(defaultIcon(iconTheme,name));
		}
	return(retstr);
}

void makeImage(char *imagepath,char *destname,diskIconStruct *hashdata)
{
	hashdata->cairoImage=cairo_image_surface_create_from_png(imagepath);
	hashdata->scale=(double)iconSize/cairo_image_surface_get_width(hashdata->cairoImage);
}

#if 0
void makeDiskInfofile(char* diskfilepath,char* label,char* uuid,int x,int y,char* type)
{
	char	*filepath;

	diskName=label;
	diskUUID=uuid;
	diskXPos=x;
	diskYPos=y;
	diskType=type;;

	if(diskfilepath==NULL)
		{
			asprintf(&filepath,"%s/%s",diskInfoPath,uuid);
			saveVarsToFile(filepath,diskData);
			free(filepath);
		}
	else
		saveVarsToFile(diskfilepath,diskData);

	diskName=NULL;
	diskUUID=NULL;
	diskXPos=-1;
	diskYPos=-1;
	diskType=NULL;
}
#endif
/*
extern char		*fileDiskLabel=NULL;
extern char		*fileDiskMime=NULL;
extern char		*fileDiskPath=NULL;
extern char		*fileDiskUUID=NULL;
extern char		*fileDiskType=NULL;
extern int		fileDiskXPos=-1;
extern int		fileDiskYPos=-1;
*/
void saveInfofile(int where,char* label,char* mime,char* path,char* uuid,char* type,int x, int y)
{
	char	*filepath;

	fileDiskLabel=label;
	fileDiskMime=mime;
	fileDiskPath=path;
	fileDiskUUID=uuid;
	fileDiskType=type;
	fileDiskXPos=x;
	fileDiskYPos=y;

	if(where==DISKFOLDER)
		{
			asprintf(&filepath,"%s/%s",diskInfoPath,uuid);
			DEBUGSTR(filepath);
			DEBUGVAL(x);
			DEBUGVAL(y);
			saveVarsToFile(filepath,globalFileData);
		}
	else
		{
		DEBUGSTR("XXXXXXXXXXXXXXXXXX");
			asprintf(&filepath,"%s/%s",cachePath,label);
			saveVarsToFile(filepath,globalFileData);
		}
	free(filepath);

	fileDiskLabel=NULL;
	fileDiskMime=NULL;
	fileDiskPath=NULL;
	fileDiskType=NULL;
	fileDiskType=NULL;
	fileDiskXPos=-1;
	fileDiskYPos=-1;
}

void getSavedDiskData(void)
{
	FILE	*fp;
	char	buffer[BUFFERSIZE];
	int		cnt;

	if(saved!=NULL)
		{
			for(int j=0;j<savedFileCount;j++)
				{
					if(saved[j].uuid!=NULL)
						free(saved[j].uuid);
				}
			free(saved);
			saved=NULL;
		}

	sprintf(buffer,"ls -1 %s|wc -l",diskInfoPath);
	fp=popen(buffer,"r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,BUFFERSIZE,fp);
			savedFileCount=atoi(buffer);
			pclose(fp);
		}

	saved=(saveDisks*)calloc(savedFileCount,sizeof(saveDisks));
	cnt=0;
	sprintf(buffer,"find %s -mindepth 1",diskInfoPath);
	fp=popen(buffer,"r");
	if(fp!=NULL)
		{
		DEBUGSTR(buffer);
			while(fgets(buffer,BUFFERSIZE,fp))
				{
					buffer[strlen(buffer)-1]=0;
					//loadVarsFromFile(buffer,diskData);
					loadVarsFromFile(buffer,globalFileData);
					DEBUGSTR(fileDiskUUID);
					//saved[cnt].uuid=strdup(diskUUID);
					saved[cnt].uuid=strdup(fileDiskUUID);
					//saved[cnt].x=diskXPos;
					saved[cnt].x=fileDiskXPos;
					//saved[cnt].y=diskYPos;
					saved[cnt].y=fileDiskYPos;
					//xySlot[diskXPos][diskYPos]=1;
					xySlot[fileDiskXPos][fileDiskYPos]=1;
					cnt++;
				}
			pclose(fp);
		}
}

void getDesktopFiles(void)
{
	char	buffer[2048];
	FILE	*fp;

	
}

void readDesktopFile(const char* name)
{
	FILE	*fr;
	char	buffer[2048];

	snprintf(buffer,2047,"%s/%s",cachePath,name);
	fr=fopen(buffer,"r");
	if(fr!=NULL)
		{
			fscanf(fr,"label	%as\n",&fileInfoPtr[desktopFilesCnt].label);
			fscanf(fr,"mime	%as\n",&fileInfoPtr[desktopFilesCnt].mime);
			fscanf(fr,"path	%as\n",&fileInfoPtr[desktopFilesCnt].path);
			fscanf(fr,"uuid	%*s\n",&fileInfoPtr[desktopFilesCnt].path);
			fscanf(fr,"type	%*s\n",&fileInfoPtr[desktopFilesCnt].path);
			fscanf(fr,"xpos	%i\n",&fileInfoPtr[desktopFilesCnt].x);
			fscanf(fr,"ypos	%i\n",&fileInfoPtr[desktopFilesCnt].y);
			desktopFilesCnt++;
			fclose(fr);
		}
}

void printString(char* str)
{
	printf("File: %s,Func: %s,Line: %i\n",errFile,errFunc,errLine);
	printf(">>%s<<\n",str);
}

void printInt(int v)
{
	printf("File: %s,Func: %s,Line: %i\n",errFile,errFunc,errLine);
	printf(">>%i<<\n",v);
}

