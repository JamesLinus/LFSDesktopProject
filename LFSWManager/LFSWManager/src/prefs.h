/******************************************************
*
*     ©keithhedger Sun  3 May 19:46:41 BST 2015
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#ifndef _PREFS_
#define _PREFS_

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/Xdbe.h>

//#include "globals.h"
#if 0
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
extern XFontStruct		*labelFont;

extern unsigned long	labelBackground;
extern unsigned long	labelForeground;

extern int				**xySlot;
#endif

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

extern args				wmPrefs[];


void saveVarsToFile(const char* filepath,args* dataptr);
bool loadVarsFromFile(char* filepath,args* dataptr);

#endif
