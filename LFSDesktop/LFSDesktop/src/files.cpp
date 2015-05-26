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
char		*desktopPath;
int			savedFileCount;

fileInfo	*fileInfoPtr;

struct hsearch_data	hashtab;

int fileExists(char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}

char	findbuffer[2048];
char	findbufferfind[2048];
void findIcon(char *theme,const char *name,const char *catagory)
{
	char	*command;
	FILE	*fp;

	sprintf(findbufferfind,"find \"/usr/share/icons/%s\" \"%s/.icons/%s\" -iname \"*%s*.png\" 2>/dev/null|grep -i \"%s\"|sort -nr -t \"x\"  -k 2.1|head -n1",theme,getenv("HOME"),theme,name,catagory);
//debugstr(findbuffer);
	fp=popen(findbufferfind,"r");
	if(fp!=NULL)
		{
			findbuffer[0]=0;
			fgets(findbuffer,2048,fp);
			findbuffer[strlen(findbuffer)-1]=0;
			pclose(fp);
		}
}

char* defaultIcon(char *theme,char *name,const char *catagory)
{
	char		*ret=NULL;
	const char	*defaultname=NULL;
	char	*command;
	FILE	*fp;
	char	buffer[2048];
	char	retstr[1024];

//	debugstr(name);
//	debugstr(theme);
new is bad
	if(strstr(name,"text")!=NULL)
		{
			defaultname="text-plain";
			findIcon(theme,defaultname,catagory);
			if(strlen(findbuffer)>0)
				return(strdup(findbuffer));
			else
				return(strdup("/usr/share/icons/gnome/256x256/mimetypes/text-x-generic.png"));
		}

	if(strstr(name,"shellscript")!=NULL)
		{
			defaultname="text-x-script";
			findIcon(theme,defaultname,catagory);
			if(strlen(findbuffer)>0)
				return(strdup(findbuffer));
			else
				return(strdup("/usr/share/icons/gnome/256x256/mimetypes/text-x-script.png"));
		}

	if(strstr(name,"empty")!=NULL)
		{
			defaultname="empty";
			findIcon(theme,defaultname,"mimetype");
			if(strlen(findbuffer)>0)
				return(strdup(findbuffer));
			else
				return(strdup("/usr/share/icons/gnome/256x256/mimetypes/empty.png"));
		}

	if(strstr(name,"audio")!=NULL)
		{
			defaultname="audio-x-generic";
			findIcon(theme,defaultname,catagory);
			if(strlen(findbuffer)>0)
				return(strdup(findbuffer));
			else
				return(strdup("/usr/share/icons/gnome/256x256/mimetypes/audio-x-generic.png"));
		}

	if(strstr(name,"image")!=NULL)
		{
			defaultname="image-x-generic";
			findIcon(theme,defaultname,catagory);
			if(strlen(findbuffer)>0)
				return(strdup(findbuffer));
			else
				return(strdup("/usr/share/icons/gnome/256x256/mimetypes/image-x-generic.png"));
		}
	if(strstr(name,"harddisk")!=NULL)
		{
			defaultname="harddisk";
			findIcon(theme,defaultname,"devices");
			if(strlen(findbuffer)>0)
				return(strdup(findbuffer));
			else
				return(strdup("/usr/share/icons/gnome/256x256/devices/drive-harddisk.png"));
		}


	if(strstr(name,"harddisk-usb")!=NULL)
		{
			defaultname="media-usb";
			findIcon(theme,defaultname,"devices");
			if(strlen(findbuffer)>0)
				return(strdup(findbuffer));
			else
				return(strdup("/usr/share/icons/gnome/256x256/devices/drive-removable-media.png"));
		}

	if(strstr(name,"flash")!=NULL)
		{
			defaultname="media-flash";
			findIcon(theme,defaultname,"devices");
			if(strlen(findbuffer)>0)
				return(strdup(findbuffer));
			else
				return(strdup("/usr/share/icons/gnome/256x256/devices/media-flash.png"));
		}

	if(strstr(name,"zip")!=NULL)
		{
			defaultname="x-archive";
			findIcon(theme,defaultname,"mimetypes");
			if(strlen(findbuffer)>0)
				return(strdup(findbuffer));
			else
				return(strdup("/usr/share/icons/gnome/256x256/mimetypes/package-x-generic.png"));
		}


	return(strdup("/usr/share/icons/gnome/256x256/mimetypes/empty.png"));		
}

char* defaultIconxx(char *theme,char *name,const char *catagory)
{
	char	*command;
	FILE	*fp;
	char	buffer[2048];
	char	*retstr=NULL;

	asprintf(&command,"find \"/usr/share/icons/%s\" \"%s/.icons/%s\" -iname \"*%s*.png\" 2>/dev/null|sort -nr -t \"x\"  -k 2.1|head -n1",theme,getenv("HOME"),theme,name);

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
			if(strcmp(catagory,"devices")!=0)
				return(defaultIcon((char*)"gnome","text-x-generic","mimetypes"));
			else
				return(defaultIcon((char*)"gnome","harddisk","devices"));

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
			retstr=defaultIcon(iconTheme,name,catagory);
		}
	return(retstr);
}

void makeImage(char *imagepath,char *destname,diskIconStruct *hashdata)
{
	hashdata->cairoImage=cairo_image_surface_create_from_png(imagepath);
	hashdata->scale=(double)iconSize/cairo_image_surface_get_width(hashdata->cairoImage);
}

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
			saveVarsToFile(filepath,globalFileData);
		}
	else
		{
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

int getSaveDiskNumber(char* uuid)
{
	for(int j=0;j<savedFileCount;j++)
		{
			if(strcmp(saved[j].uuid,uuid)==0)
				return(j);
		}
	return(-1);
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
			while(fgets(buffer,BUFFERSIZE,fp))
				{
					buffer[strlen(buffer)-1]=0;
					loadVarsFromFile(buffer,globalFileData);
					saved[cnt].uuid=strdup(fileDiskUUID);
					saved[cnt].x=fileDiskXPos;
					saved[cnt].y=fileDiskYPos;
					xySlot[fileDiskXPos][fileDiskYPos]=1;
					cnt++;
				}
			pclose(fp);
		}
}

void getFreeSlot(int *x,int *y)
{
	for(int yy=0; yy<yCnt; yy++)
		{
			for(int xx=0; xx<xCnt; xx++)
				{
					if(xySlot[xx][yy]==0)
						{
							*x=xx;
							*y=yy;
							xySlot[xx][yy]=1;
							return;
						}
				}
		}
}

/*
	{"label",TYPESTRING,&fileDiskLabel},
	{"mime",TYPESTRING,&fileDiskMime},
	{"path",TYPESTRING,&fileDiskPath},
	{"uuid",TYPESTRING,&fileDiskUUID},
	{"type",TYPESTRING,&fileDiskType},
	{"xpos",TYPEINT,&fileDiskXPos},
	{"ypos",TYPEINT,&fileDiskYPos},

*/
void readDesktopFile(const char* name)
{
	FILE	*fr;
	char	buffer[2048];

	snprintf(buffer,2047,"%s/%s",cachePath,name);
	fr=fopen(buffer,"r");
	if(fr!=NULL)
		{
			loadVarsFromFile(buffer,globalFileData);
			fileInfoPtr[desktopFilesCnt].label=fileDiskLabel;
			fileInfoPtr[desktopFilesCnt].mime=fileDiskMime;
			fileInfoPtr[desktopFilesCnt].path=fileDiskPath;
			fileInfoPtr[desktopFilesCnt].x=fileDiskXPos;
			fileInfoPtr[desktopFilesCnt].y=fileDiskYPos;
			fileDiskLabel=NULL;
			fileDiskMime=NULL;
			fileDiskPath=NULL;
			desktopFilesCnt++;
			if(desktopFilesCnt==desktopFilesCntMax)
				{
					fileInfoPtr=(fileInfo*)realloc(fileInfoPtr,(desktopFilesCntMax+20)*sizeof(fileInfo));
					for(int j=desktopFilesCnt;desktopFilesCnt+20;j++)
						{
							fileInfoPtr[j].label=NULL;
							fileInfoPtr[j].mime=NULL;
							fileInfoPtr[j].path=NULL;
							fileInfoPtr[j].uuid=NULL;
							fileInfoPtr[j].type=NULL;
							fileInfoPtr[j].x=-1;
							fileInfoPtr[j].y=-1;
						}
					desktopFilesCntMax+=20;
				}
			fclose(fr);
		}
}

void refreshDesktopFiles(void)
{
	char	buffer[4096];
	char	buffer2[4096];
	char	*ptr;
	FILE	*fp;
	FILE	*fm;

	for(int j=1;j<desktopFilesCntMax;j++)
		{
			if(fileInfoPtr[j].label!=NULL)
				{
					free(fileInfoPtr[j].label);
					fileInfoPtr[j].label=NULL;
				}	
			if(fileInfoPtr[j].mime!=NULL)
				{
					free(fileInfoPtr[j].mime);
					fileInfoPtr[j].mime=NULL;
				}	
			if(fileInfoPtr[j].path!=NULL)
				{
					free(fileInfoPtr[j].path);
					fileInfoPtr[j].path=NULL;
				}	
			if(fileInfoPtr[j].icon!=NULL)
				{
					free(fileInfoPtr[j].icon);
					fileInfoPtr[j].icon=NULL;
				}	
			if(fileInfoPtr[j].uuid!=NULL)
				{
					free(fileInfoPtr[j].uuid);
					fileInfoPtr[j].uuid=NULL;
				}	
			if(fileInfoPtr[j].type!=NULL)
				{
					free(fileInfoPtr[j].type);
					fileInfoPtr[j].type=NULL;
				}	
		}

	desktopFilesCnt=1;

	sprintf(buffer,"find %s -mindepth 1",desktopPath);

	fp=popen(buffer,"r");
	while(fgets(buffer,4096,fp))
		{
			buffer[strlen(buffer)-1]=0;
			ptr=strrchr(buffer,'/');
			ptr++;
			sprintf(buffer2,"%s/%s",cachePath,ptr);
			if(fileExists(buffer2)==-1)
				{
					fileInfoPtr[desktopFilesCnt].path=strdup(buffer);
					ptr=strrchr(buffer,'/');
					ptr++;
					fileInfoPtr[desktopFilesCnt].label=strdup(ptr);
					sprintf(buffer2,"xdg-mime query filetype '%s'",fileInfoPtr[desktopFilesCnt].path);
					fm=popen(buffer2,"r");
					fgets(buffer,4096,fm);
					pclose(fm);
					buffer[strlen(buffer)-1]=0;
					ptr=strchr(buffer,'/');
					while(ptr!=NULL)
						{
							*ptr='-';
							ptr=strchr(buffer,'/');
						}
					ptr=strstr(buffer,"text-x-shellscript");
					if(ptr==NULL)
						fileInfoPtr[desktopFilesCnt].mime=strdup(buffer);
					else
						fileInfoPtr[desktopFilesCnt].mime=strdup("application-x-shellscript");
					getFreeSlot(&fileInfoPtr[desktopFilesCnt].x,&fileInfoPtr[desktopFilesCnt].y);
					saveInfofile(CACHEFOLDER,fileInfoPtr[desktopFilesCnt].label,fileInfoPtr[desktopFilesCnt].mime,fileInfoPtr[desktopFilesCnt].path,NULL,NULL,fileInfoPtr[desktopFilesCnt].x,fileInfoPtr[desktopFilesCnt].y);
					desktopFilesCnt++;
				}
			else
				{
					readDesktopFile(ptr);
				}
		}
	pclose(fp);
}

void rescanDesktop(void)
{
 refreshDesktopFiles();
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

