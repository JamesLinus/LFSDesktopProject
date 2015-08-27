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

#include "LFSTKLib.h"

enum {DEFNORMAL,DEFPRELIE,DEFACTIVE,DEFINACTIVE,DEFFONTNORMAL,DEFFONTPRELIE,DEFFONTACTIVE,DEFFONTINACTIVE};

const char *defaultColourStrings[]={"grey50","grey80","grey40","grey90"};
const char *defaultFontColourStrings[]={"white","black","white","grey80"};
const char *defaultFontString="sans-serif:size=10";
const char *defaultThemePath="/usr/share/themes/Crux/xfwm4";
const char *defaultFrameStrings[]={"black","#00ffff","black","white","white"};

LFSTK_lib::~LFSTK_lib()
{
	free(lfsToolKitGlobals);
}

/**
* Sets a global string or the default.
*
* \param state Gadget state.
* \param type Type of string to set.
* \param str New string.
*/
void LFSTK_lib::LFSTK_setGlobalString(int state,int type,const char *str)
{
	const char	*ptr=NULL;
	
	switch(type)
		{
			case TYPEWINDOW:
				ptr=this->globalWindowColours[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalWindowColours[state]=strdup(str);
				break;
			case TYPEBUTTON:
				ptr=this->globalButtonColours[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalButtonColours[state]=strdup(str);
				break;
			case TYPEMENUITEM:
				ptr=this->globalMenuItemColours[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalMenuItemColours[state]=strdup(str);
				break;
			case TYPEFONTCOLOUR:
				ptr=this->globalFontColourNames[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalFontColourNames[state]=strdup(str);
				break;
			case TYPEMENUITEMFONTCOLOUR:
				ptr=this->globalMenuItemFontColourNames[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalMenuItemFontColourNames[state]=strdup(str);
				break;
			case TYPEMENUITEMFONT:
				ptr=this->globalMenuItemFontString;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalMenuItemFontString=strdup(str);
				break;
			case TYPEFONT:
				ptr=this->globalFontString;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalFontString=strdup(str);
				break;
			case TYPETHEME:
				ptr=this->globalThemePath;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalThemePath=strdup(str);
				break;
			case TYPEWMFRAME:
				ptr=this->globalFrameColours[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalFrameColours[state]=strdup(str);
				break;
		}
}

/**
* Gets a global string or the default.
*
* \param state Gadget state.
* \param type Type of string to get.
* \return const char* String.
* \note Don't free return value.
* \note If global string not loaded return default.
*/
const char *LFSTK_lib::LFSTK_getGlobalString(int state,int type)
{
	const char	*ptr=NULL;
	
	switch(type)
		{
			case TYPEWINDOW:
				ptr=this->globalWindowColours[state];
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPEBUTTON:
				ptr=this->globalButtonColours[state];
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPEMENUITEM:
				ptr=this->globalMenuItemColours[state];
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPEFONTCOLOUR:
				ptr=this->globalFontColourNames[state];
				if(ptr==NULL)
					ptr=defaultFontColourStrings[state];
				break;
			case TYPEMENUITEMFONTCOLOUR:
				ptr=this->globalMenuItemFontColourNames[state];
				if(ptr==NULL)
					ptr=defaultFontColourStrings[state];
				break;
			case TYPEMENUITEMFONT:
				ptr=this->globalMenuItemFontString;
				if(ptr==NULL)
					ptr=defaultFontString;			
				break;
			case TYPEFONT:
				ptr=this->globalFontString;
				if(ptr==NULL)
					ptr=defaultFontString;
				break;
			case TYPETHEME:
				ptr=this->globalThemePath;
				if(ptr==NULL)
					ptr=defaultThemePath;
				break;
			case TYPEWMFRAME:
				ptr=this->globalFrameColours[state];
				if(ptr==NULL)
					ptr=defaultFrameStrings[state];
				break;
		}

	if(ptr!=NULL)
		return(ptr);
}

LFSTK_lib::LFSTK_lib(bool loadvars)
{
	char *env=NULL;

	args myargs[]=
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
		{"menuitem_font",TYPESTRING,&(this->globalMenuItemFontString)},
		{"menuitem_font_normal",TYPESTRING,&(this->globalMenuItemFontColourNames[NORMALCOLOUR])},
		{"menuitem_font_prelight",TYPESTRING,&(this->globalMenuItemFontColourNames[PRELIGHTCOLOUR])},
		{"menuitem_font_active",TYPESTRING,&(this->globalMenuItemFontColourNames[ACTIVECOLOUR])},
		{"menuitem_font_inactive",TYPESTRING,&(this->globalMenuItemFontColourNames[INACTIVECOLOUR])},

//font
		{"font",TYPESTRING,&(this->globalFontString)},
		{"font_normal",TYPESTRING,&(this->globalFontColourNames[NORMALCOLOUR])},
		{"font_prelight",TYPESTRING,&(this->globalFontColourNames[PRELIGHTCOLOUR])},
		{"font_active",TYPESTRING,&(this->globalFontColourNames[ACTIVECOLOUR])},
		{"font_inactive",TYPESTRING,&(this->globalFontColourNames[INACTIVECOLOUR])},

//window manager
		{"wmactive_frame",TYPESTRING,&(this->globalFrameColours[ACTIVEFRAME])},
		{"wmactive_fill",TYPESTRING,&(this->globalFrameColours[ACTIVEFRAMEFILL])},
		{"wminactive_frame",TYPESTRING,&(this->globalFrameColours[INACTIVEFRAME])},
		{"wminactive_fill",TYPESTRING,&(this->globalFrameColours[INACTIVEFRAMEFILL])},
		{"widgetcolour",TYPESTRING,&(this->globalFrameColours[WIDGETCOLOUR])},
		{"theme",TYPESTRING,&(this->globalThemePath)},

		{NULL,0,NULL},
	};

	this->lfsToolKitGlobals=(args*)calloc(1,sizeof(myargs));
	memcpy(this->lfsToolKitGlobals,myargs,sizeof(myargs));

	for(int j=0;j<MAXCOLOURS;j++)
		{
			this->globalWindowColours[j]=NULL;
			this->globalButtonColours[j]=NULL;
			this->globalMenuItemColours[j]=NULL;
			this->globalMenuItemFontColourNames[j]=NULL;
		}

	for(int j=0;j<MAXFRAMECOLOURS;j++)
		this->globalFrameColours[j]=NULL;

	for(int j=0;j<MAXCOLOURS;j++)
		this->globalFontColourNames[j]=NULL;

	this->globalFontString=NULL;
	this->globalMenuItemFontString=NULL;
	this->globalThemePath=NULL;

	if(loadvars==true)
		{
			asprintf(&env,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
			this->LFSTK_loadVarsFromFile(env,this->lfsToolKitGlobals);
			free(env);
		}
}

/**
* Load variables from prefs file.
*
* \param filepath Path to prefs file.
* \param args Argument list..
* \return bool Success.
* \note args is an array of the form:
* \note PREFNAME,PREFTYPE,VARIABLEADDRESS.
* \note PREFTYPE=TYPEINT|TYPESTRING|TYPEBOOL.
* \note eg:
* \note {...,"font_inactive",TYPESTRING,&(this->globalFontColourNames[INACTIVECOLOUR],...,NULL,0,NULL};
*/
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

/**
* Save variables to prefs file.
*
* \param filepath Path to prefs file.
* \param args Argument list..
* \note args is an array of the form:
* \note PREFNAME,PREFTYPE,VARIABLEADDRESS.
* \note PREFTYPE=TYPEINT|TYPESTRING|TYPEBOOL.
* \note eg:
* \note {...,"font_inactive",TYPESTRING,&(this->globalFontColourNames[INACTIVECOLOUR],...,NULL,0,NULL};
*/
void LFSTK_lib::LFSTK_saveVarsToFile(const char* filepath,const args* dataptr)
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
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
							break;
						case TYPESTRING:
							if(*(char**)(dataptr[cnt].data)!=NULL)
								fprintf(fd,"%s	%s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
							break;
						case TYPEBOOL:
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
							break;
						}
					cnt++;
				}
			if(fd!=stdout)
				fclose(fd);
		}

}

/**
* Get tool kit args array.
* \return args.
* \note args is an array of the form:
* \note PREFNAME,PREFTYPE,VARIABLEADDRESS.
* \note PREFTYPE=TYPEINT|TYPESTRING|TYPEBOOL.
* \note eg:
* \note {...,"font_inactive",TYPESTRING,&(this->globalFontColourNames[INACTIVECOLOUR],...,NULL,0,NULL};
* \note Don't free returned array.
*/
const args *LFSTK_lib::LFSTK_getTKArgs(void)
{
	return(this->lfsToolKitGlobals);
}
