/*
 *
 * ©K. D. Hedger. Fri 24 Jul 20:38:28 BST 2015 kdhedger68713@gmail.com

 * This file (lib.h) is part of LFSToolKit.

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

#ifndef _LFSTKLIB_
#define _LFSTKLIB_

#include <X11/Xlib.h>
#include "LFSTKWindow.h"

enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

struct args
{
	const char*		name;
	int				type;
	void*			data;
};

extern args			lfsToolKitGlobals[];
extern colourStruct	globalWindowColours[MAXCOLOURS];
extern colourStruct	globalButtonColours[MAXCOLOURS];
extern colourStruct	globalMenuItemColours[MAXCOLOURS];
extern bool			globalColoursSet;

extern char			*gFontString;
extern char			*gFontColourNames[MAXFONTCOLS];
extern char			*gMenuItemFontString;
extern char			*gMenuItemFontColourNames[MAXCOLOURS];

void				gadgetEvent(void *self,XEvent *e,int type);
fontStruct			*ftload(Display *disp,int scr,char *name);
//fontColour			*ftLoadColour(LFSTK_windowClass *wc,const char *name);
int					ftTextWidth_Utf8(LFSTK_windowClass *wc,char *s);
void				drawUtf8String(LFSTK_windowClass *wc,Window d,XftFont* font,int x,int y,char *col,char *s);
bool 				loadVarsFromFile(char* filepath,args* dataptr);
int					getTextwidth(Display* disp,XftFont *font,char *s);

#endif
