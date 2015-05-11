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

char* pathToIcon(char* name)
{
	char	*command;
	FILE	*fp;
	char	buffer[2048];
	char	*retstr=NULL;

	asprintf(&command,"find /usr/share/icons/%s %s/.icons/%s -iname \"*%s*.png\"  2>/dev/null|sort -nr -t \"x\"  -k 2.1|head -n1",iconTheme,getenv("HOME"),iconTheme,name);

	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			fgets(buffer,2048,fp);
			sscanf(buffer,"%as",&retstr);
			pclose(fp);
		}
	return(retstr);
}

void makeImage(char *imagepath,char *destname,diskIconStruct *hashdata)
{
	hashdata->cairoImage=cairo_image_surface_create_from_png(imagepath);
	hashdata->scale=(double)iconSize/cairo_image_surface_get_width(hashdata->cairoImage);
}




