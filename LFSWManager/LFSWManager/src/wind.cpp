
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#include <stdarg.h>
#include <stdio.h>

#include "wind.h"

Display				*dpy;
struct font 		*font;
struct fontcolor	*fhighlight;
struct fontcolor	*fnormal;

unsigned int		screen;
Visual				*visual;
int					displayWidth;
int					displayHeight;
monitors			*monitorData;
unsigned int		numberOfMonitors;
XContext			listeners;
const char			*progname;
int					blackColor;
int					whiteColor;
GC					mainGC;

//wm prefs
int					placement=MOUSEMONITOR;
unsigned int		numberOfDesktops;
char				*titleFont;
char				*fontColours[5];
int					doswapdesk=-1;
int					liveUpdate=1;
char				*terminalCommand=NULL;
char				*lfstkFile=NULL;

//theme
themeStruct			theme;
char				*fontName;
int					fontSize;
char				*pathToTheme;

int					frameTop;
int					frameBottom;
int					frameLeft;
int					frameRight;

//new colours
//active frame
unsigned long		activeFrameFill;
unsigned long		activeFrame;
unsigned long		inactiveFrame;
unsigned long		inactiveFrameFill;
unsigned long		widgetColour;
//buttons
GC					activeGC;
//wmactive_frame #948069
//wmactive_fill #5E3300
//wminactive_frame #9B9184
//wminactive_fill #836C51
//wmtext	red
//unsigned long		textColour;

#ifdef _DEBUGLEVEL_
//debug
int					errLine;
const char			*errFile;
const char			*errFunc;
#endif
bool				xLibWarnings=false;

/*
 * Print formatted error message
 */
void errorf(const char *fmt,...)
{
	va_list ap;
	fprintf(stderr,"%s: ",progname);
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	va_end(ap);
	fprintf(stderr,"\n");
}

void setlistener(Window w,const struct listener *l)
{
	CHECKPOINT
	if (l==NULL)
		XDeleteContext(dpy,w,listeners);
	else
		XSaveContext(dpy,w,listeners,(XPointer)l);
}

struct listener *getlistener(Window w)
{
	CHECKPOINT
	struct listener *l;
	if (XFindContext(dpy,w,listeners,(XPointer *)&l)==0)
		return l;
	else
		return NULL;
}

int redirect(XEvent *e,Window w)
{
	CHECKPOINT
	struct listener *l=getlistener(w);
	if (l==NULL)
		return -1;

	l->function(l->pointer,e);
	return 0;
}
