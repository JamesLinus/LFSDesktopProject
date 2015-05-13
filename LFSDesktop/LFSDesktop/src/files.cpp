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

#include "prefs.h"
#include "graphics.h"

char	*diskInfoPath;
char	*cachePath;
char	*prefsPath;

char	*diskName;
char	*diskUUID;
int		diskXPos;
int		diskYPos;
char	*diskType;

struct hsearch_data	hashtab;

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

char* pathToIcon(char* name)
{
	char	*command;
	FILE	*fp;
	char	buffer[2048];
	char	*retstr=NULL;

	asprintf(&command,"find \"/usr/share/icons/%s\" \"%s/.icons/%s\" -iname \"*%s.png\"  2>/dev/null|sort -nr -t \"x\"  -k 2.1|head -n1",iconTheme,getenv("HOME"),iconTheme,name);

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

void makeDiskInfofile(char* diskfilepath,char* label,char* uuid,int x,int y,char* type)
{
	char	*filepath;

	diskName=label;
	diskUUID=uuid;
	diskXPos=x;
	diskYPos=y;
	diskType=type;

	if(diskfilepath==NULL)
		{
			asprintf(&filepath,"%s/%s",diskInfoPath,uuid);
			saveVarsToFile(filepath,diskData);
			free(filepath);
		}
	else
		saveVarsToFile(diskfilepath,diskData);
}




