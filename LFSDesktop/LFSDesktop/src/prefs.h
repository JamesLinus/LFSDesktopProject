/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:54:53 BST 2015 kdhedger68713@gmail.com

 * This file (prefs.h) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _PREFS_
#define _PREFS_

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/Xdbe.h>

#include "globals.h"

#define MAXGRIDY 8
#define MAXGRIDX 8

extern args				desktopPrefs[];

extern char				*iconTheme;
extern int				iconSize;
extern int				gridSize;
extern int				gridBorder;
extern int				refreshRate;
extern char				*terminalCommand;
extern bool				showSuffix;
extern char				*fontFace;
extern char				*foreCol;
extern char				*backCol;
extern char				*backAlpha;
extern char				*ignores;

extern Display			*display;
extern Window			rootWin;
extern int				displayWidth;
extern int				displayHeight;
extern GC				gc;
extern Region			rg;
extern XdbeBackBuffer	buffer;
extern XdbeSwapInfo		swapInfo;
extern Drawable			drawOnThis;
extern Colormap			cm;
extern Visual			*visual;
extern int				depth;
extern int				screen;
extern int				blackColor;
extern int				whiteColor;

extern GC				labelGC;
//extern XFontStruct		*labelFont;

extern unsigned long	labelBackground;
extern unsigned long	labelForeground;

extern int				**xySlot;

void saveVarsToFile(const char* filepath,args* dataptr);
bool loadVarsFromFile(char* filepath,args* dataptr);

#endif
