/*
 *
 * ©K. D. Hedger. Sun 13 Sep 14:10:19 BST 2015 kdhedger68713@gmail.com

 * This file (lfsdesktopprefs.cpp) is part of LFSApplications.

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

enum {ICONTHEME,ICONSIZE,GRIDSIZE,GRIDBORDER,REFRESHRATE,SHOWSUFFIX,FORECOLOUR,BACKCOLOUR,TERMCOMMAND,FONTFACE,IGNORES,NUMPREFS};

char	*prefs[NUMPREFS]={NULL,};

args	desktopPrefs[]=
{
	{"icontheme",TYPESTRING,&prefs[ICONTHEME]},
	{"iconsize",TYPESTRING,&prefs[ICONSIZE]},
	{"gridsize",TYPESTRING,&prefs[GRIDSIZE]},
	{"gridborder",TYPESTRING,&prefs[GRIDBORDER]},
	{"refreshrate",TYPESTRING,&prefs[REFRESHRATE]},
	{"termcommand",TYPESTRING,&prefs[TERMCOMMAND]},
	{"showextension",TYPESTRING,&prefs[SHOWSUFFIX]},
	{"fontface",TYPESTRING,&prefs[FONTFACE]},
	{"labelforeground",TYPESTRING,&prefs[FORECOLOUR]},
	{"labelbackground",TYPESTRING,&prefs[BACKCOLOUR]},
	{"noshow",TYPESTRING,&prefs[IGNORES]},
	{NULL,0,NULL}
};

int main(int argc, char **argv)
{
	XEvent			event;
	int				sx=0;
	int				sy=0;
	geometryStruct	*geom;
	int				bwidth=96;
	int				bhite=24;
	int				spacing=bwidth+10;
	int				vspacing=bhite+10;
	int				col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;

	return(0);
};