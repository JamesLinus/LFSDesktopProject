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
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>

#include <X11/extensions/Xinerama.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xdbe.h>

#define REFRESHRATE 2
#define MAXGRIDY 4
#define GRIDBORDER 32
#define ICONSIZE 40
#define GRIDSIZE (ICONSIZE+32)

enum {SATA=0,USB,CDROM};
enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

extern args				diskData[];
extern char				*diskName;
extern char				*diskUUID;
extern int				diskXPos;
extern int				diskYPos;
extern char				*diskType;

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
extern unsigned long	labelBackground;
extern unsigned long	labelForeground;

void saveVarsToFile(const char* filepath,args* dataptr);
void loadVarsFromFile(char* filepath,args* dataptr);

#endif
