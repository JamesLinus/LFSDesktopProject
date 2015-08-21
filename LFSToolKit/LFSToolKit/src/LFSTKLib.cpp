/*
 *
 * ©K. D. Hedger. Fri 21 Aug 12:28:57 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTK_lib.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "LFSTK_lib.h"

LFSTK_lib::~LFSTK_lib()
{
}

LFSTK_lib::LFSTK_lib()
{

args			lfsToolKitGlobals[]=
{
//window
	{"window_normal",TYPESTRING,&(this->globalWindowColours[NORMALCOLOUR])},
	{"window_prelight",TYPESTRING,&(this->globalWindowColours[PRELIGHTCOLOUR])},
	{"window_active",TYPESTRING,&(this->globalWindowColours[ACTIVECOLOUR])},
	{"window_inactive",TYPESTRING,&(this->globalWindowColours[INACTIVECOLOUR])},
//button
	{"button_normal",TYPESTRING,&(this->globalButtonColours[NORMALCOLOUR])},
	{"button_prelight",TYPESTRING,&(this->globalButtonColours[PRELIGHTCOLOUR])},
	{"button_active",TYPESTRING,&(this->globalButtonColours[ACTIVECOLOUR])},
	{"button_inactive",TYPESTRING,&(this->globalButtonColours[INACTIVECOLOUR])},
//menu button
	{"menuitem_normal",TYPESTRING,&(this->globalMenuItemColours[NORMALCOLOUR])},
	{"menuitem_prelight",TYPESTRING,&(this->globalMenuItemColours[PRELIGHTCOLOUR])},
	{"menuitem_active",TYPESTRING,&(this->globalMenuItemColours[ACTIVECOLOUR])},
	{"menuitem_inactive",TYPESTRING,&(this->globalMenuItemColours[INACTIVECOLOUR])},
	{"menuitem_font",TYPESTRING,&(this->gMenuItemFontString)},
	{"menuitem_font_normal",TYPESTRING,&(this->gMenuItemFontColourNames[NORMALCOLOUR])},
	{"menuitem_font_prelight",TYPESTRING,&(this->gMenuItemFontColourNames[PRELIGHTCOLOUR])},
	{"menuitem_font_active",TYPESTRING,&(this->gMenuItemFontColourNames[ACTIVECOLOUR])},

//font
	{"font",TYPESTRING,&(this->gFontString)},
	{"font_normal",TYPESTRING,&(this->gFontColourNames[NORMALCOLOUR])},
	{"font_prelight",TYPESTRING,&(this->gFontColourNames[PRELIGHTCOLOUR])},
	{"font_active",TYPESTRING,&(this->gFontColourNames[ACTIVECOLOUR])},

//window manager
	{"theme",TYPESTRING,&(this->gThemePath)},

	{NULL,0,NULL}
};

}

const char *LFSTK_lib::LFSTK_getThemeName(void)
{
	return(this->gThemePath);
}

bool LFSTK_lib::LFSTK_loadVarsFromFile(const char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	char	buffer[2048];
	int		cnt;
	char*	argname=NULL;
	char*	strarg=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					sscanf(buffer,"%ms %ms",&argname,&strarg);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((strarg!=NULL) && (argname!=NULL) && (strcmp(argname,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
										case TYPEINT:
											*(int*)dataptr[cnt].data=atoi(strarg);
											break;
										case TYPESTRING:
											if(*(char**)(dataptr[cnt].data)!=NULL)
												free(*(char**)(dataptr[cnt].data));
											sscanf(buffer,"%*s %m[^\n]s",(char**)dataptr[cnt].data);
										break;
										case TYPEBOOL:
											*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
											break;
										}
								}
							cnt++;
						}
					if(argname!=NULL)
						free(argname);
					if(strarg!=NULL)
						free(strarg);
					argname=NULL;
					strarg=NULL;
				}
			fclose(fd);
			return(true);
		}
	return(false);
}