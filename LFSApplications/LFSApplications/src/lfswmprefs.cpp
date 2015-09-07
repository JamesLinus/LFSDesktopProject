/*
 *
 * ©K. D. Hedger. Mon  7 Sep 13:20:24 BST 2015 kdhedger68713@gmail.com

 * This file (lfswmprefs.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include <LFSTKMenuButton.h>
#include <LFSTKLineEdit.h>
#include <LFSTKLabel.h>
#include <LFSTKLib.h>

enum {EXIT=0,APPLY,SAVE,NOMORE};
enum {ACTIVEFRAME=0,ACTIVEFRAMEFILL,INACTIVEFRAME,INACTIVEFRAMEFILL,TEXTCOLOUR};

const char			*buttonnames[]={"Button Normal","Button Prelight","Button Active","Button Inactive"};
const char			*menunames[]={"Menu Normal","Menu Prelight","Menu Active","Menu Inactive"};
const char			*fontnames[]={"Font Normal","Font Prelight","Font Active","Font Inactive"};
const char			*itemfontnames[]={"Menu Font Norm","Menu Font Prel","Menu Font Activ","Menu Font Inact"};

//prefs
int					placement=MOUSEMONITOR;
unsigned int		numberOfDesktops;
char				*titleFont;
char				*fontColours[5];
int					doswapdesk=-1;
int					liveUpdate=1;
char				*terminalCommand=NULL;

//theme
themeStruct			theme;
char				*fontName;
int					fontSize;
char				*pathToTheme;

args				wmPrefs[]=
{
	{"wmactive_frame",TYPESTRING,&fontColours[ACTIVEFRAME]},
	{"wmactive_fill",TYPESTRING,&fontColours[ACTIVEFRAMEFILL]},
	{"wminactive_frame",TYPESTRING,&fontColours[INACTIVEFRAME]},
	{"wminactive_fill",TYPESTRING,&fontColours[INACTIVEFRAMEFILL]},
	{"widgetcolour",TYPESTRING,&fontColours[TEXTCOLOUR]},
	{"placement",TYPEINT,&placement},
	{"titlefont",TYPESTRING,&titleFont},
	{"desktops",TYPEINT,&numberOfDesktops},
	{"liveupdate",TYPEINT,&liveUpdate},
	{"theme",TYPESTRING,&theme.pathToTheme},
	{"termcommand",TYPESTRING,&terminalCommand},
	{NULL,0,NULL}
};

bool				mainloop=false;
LFSTK_windowClass	*wc;
LFSTK_buttonClass	*bc[NOMORE]={NULL,};
LFSTK_labelClass	*lc;

bool callback(void *p,void* ud)
{
	char *env;

	if((long)ud==EXIT)
		{
			mainloop=false;
			return(false);
		}

	switch((long)ud)
		{
			case SAVE:
				asprintf(&env,"%s/.config/LFS/lfswmanager.rc",getenv("HOME"));
				wc->globalLib->LFSTK_saveVarsToFile(env,wc->globalLib->LFSTK_getTKArgs());
				free(env);
				break;

			case APPLY:
				wc->LFSTK_setWindowColourName(NORMALCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));
				wc->globalLib->LFSTK_setGlobalString(NORMALCOLOUR,TYPEWINDOW,wc->windowColourNames[NORMALCOLOUR].name);
				wc->LFSTK_clearWindow();
				break;
		}
}