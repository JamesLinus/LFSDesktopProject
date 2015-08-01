/******************************************************
*
*     ©keithhedger Sun  3 May 19:46:41 BST 2015
*     kdhedger68713@gmail.com
*
*     prefs.cpp
* 
******************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "prefs.h"
#include "wind.h"

enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

args	wmPrefs[]=
{
	{"focusedfore",TYPESTRING,&fontColours[FOCUSEDFORE]},
	{"focusedback",TYPESTRING,&fontColours[FOCUSEDBACK]},
	{"fore",TYPESTRING,&fontColours[FORE]},
	{"back",TYPESTRING,&fontColours[BACK]},
	{"placement",TYPEINT,&placement},
	{"titlefont",TYPESTRING,&titleFont},
	{"desktops",TYPEINT,&numberOfDesktops},
	{"liveupdate",TYPEINT,&liveUpdate},
	{"theme",TYPESTRING,&theme.pathToTheme},
	{"termcommand",TYPESTRING,&terminalCommand},
	{NULL,0,NULL}
};

args	themeRC[]=
{
	{"button_offset",TYPEINT,&theme.buttonOffset},
	{"button_spacing",TYPEINT,&theme.buttonSpacing},
	{"active_text_color",TYPESTRING,&fontColours[FOCUSEDFORE]},
	{"inactive_text_color",TYPESTRING,&fontColours[FORE]},
	{NULL,0,NULL}
};

void saveVarsToFile(const char* filepath,args* dataptr,const char *fs)
{
	FILE*	fd=NULL;
	int		cnt=0;

	if(filepath[0]=='-')
		fd=stdout;
	else
		fd=fopen(filepath,"w");

	if(fd!=NULL)
		{
			while(dataptr[cnt].name!=NULL)
				{
					switch(dataptr[cnt].type)
						{
						case TYPEINT:
							fprintf(fd,"%s%s%i\n",dataptr[cnt].name,fs,*(int*)dataptr[cnt].data);
							break;
						case TYPESTRING:
							if(*(char**)(dataptr[cnt].data)!=NULL)
								fprintf(fd,"%s%s%s\n",dataptr[cnt].name,fs,*(char**)(dataptr[cnt].data));
							break;
						case TYPEBOOL:
							fprintf(fd,"%s%s%i\n",dataptr[cnt].name,fs,(int)*(bool*)dataptr[cnt].data);
							break;
						}
					cnt++;
				}
			fclose(fd);
		}
}

bool loadVarsFromFile(char* filepath,args* dataptr,const char *fs)
{
	CHECKPOINT
	FILE*	fd=NULL;
	char	buffer[2048];
	int		cnt;
	char*	strarg=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					if(strlen(buffer)>1)
						buffer[strlen(buffer)-1]=0;
					strarg=strstr(buffer,fs);
					if(strarg!=NULL)
						{
							*strarg=0;
							strarg++;
							cnt=0;
							while(dataptr[cnt].name!=NULL)
								{
									if(strcmp(buffer,dataptr[cnt].name)==0)
										{
											switch(dataptr[cnt].type)
												{
													case TYPEINT:
														*(int*)dataptr[cnt].data=atoi(strarg);
														break;
													case TYPESTRING:
														if(*(char**)(dataptr[cnt].data)!=NULL)
															free(*(char**)(dataptr[cnt].data));
														sscanf(strarg," %m[^\n]s",(char**)dataptr[cnt].data);
														break;
													case TYPEBOOL:
														*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
														break;
												}
										}
									cnt++;
								}
						}
				}
			fclose(fd);
			return(true);
		}
	return(false);
}
