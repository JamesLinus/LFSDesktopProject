
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
int					displayWidth;
int					displayHeight;
monitors			*monitorData;
unsigned int		numberOfMonitors;
XContext			listeners;
const char			*progname;

//wm prefs
int					placement=MOUSEMONITOR;
unsigned int		numberOfDesktops;
char				*titleFont;
char				*fontColours[4];

//bool				flipDesk=false;
//int					flipToDesk=0;

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
	if (l==NULL)
		XDeleteContext(dpy,w,listeners);
	else
		XSaveContext(dpy,w,listeners,(XPointer)l);
}

struct listener *getlistener(Window w)
{
	struct listener *l;
	if (XFindContext(dpy,w,listeners,(XPointer *)&l)==0)
		return l;
	else
		return NULL;
}

int redirect(XEvent *e,Window w)
{
	struct listener *l=getlistener(w);
	if (l==NULL)
		return -1;
	l->function(l->pointer,e);
	return 0;
}
