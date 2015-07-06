
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#ifndef _WIND_H_
#define _WIND_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "config.h"

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

#define NELEM(v) (sizeof v / sizeof v[0])

// The desktop number of sticky windows
#define DESK_ALL 0xffffffffUL

typedef unsigned long Desk;

enum {NOPLACE=0,UNDERMOUSE,CENTREMMONITOR,CENTRESCREEN,MOUSEMONITOR};
enum {FOCUSEDFORE=0,FOCUSEDBACK,FORE,BACK};

struct geometry
{
	int x;
	int y;
	int width;
	int height;
	int borderwidth;
};

struct extents
{
	int top;
	int bottom;
	int left;
	int right;
};

struct listener
{
	void (*function)(void *,XEvent *);
	void *pointer;
};

struct bitmap
{
	unsigned char *bits;
	int width;
	int height;
	Pixmap pixmap;
};

#define DEFINE_BITMAP(name) \
		struct bitmap name={ \
			.bits=name##_bits,\
			.width=name##_width,\
			.height=name##_height,\
			.pixmap=None \
		}

struct font
{
	int ascent;
	int descent;
	int size;
	void *data;
};

typedef struct
{
	int type;       /* ColormapNotify */
	unsigned long serial;   /* # of last request processed by server */
	Bool send_event;        /* true if this came from a SendEvent request */
	Display *display;       /* Display the event was read from */
	Window window;
	Colormap colormap;      /* colormap or None */
	Bool mynew;
	int state;      /* ColormapInstalled,ColormapUninstalled */
} myXColormapEvent;

struct	monitors
{
	int		monW;
	int		monH;
	int		monX;
	int		monY;
};

extern enum runlevel
{
	RL_STARTUP,RL_NORMAL,RL_SHUTDOWN,
} runlevel;

// The display name argument used in call to XOpenDisplay
extern const char		*displayname;

// The last X error reported
extern const char		*xerror;

extern Display			*dpy;
extern Window			root;

// Normal colors
extern unsigned long	foregroundpixel;
extern unsigned long	backgroundpixel;

// Highlight colors
extern unsigned long	hlforegroundpixel;
extern unsigned long	hlbackgroundpixel;

// Normal colors
extern GC				foreground;
extern GC				background;

// Highlight colors
extern GC				hlforeground;
extern GC				hlbackground;

extern int				lineheight;
extern int				halfleading;

extern struct font		*font;
extern struct fontcolor	*fhighlight;
extern struct fontcolor	*fnormal;

extern unsigned int		screen;
extern int				displayWidth;
extern int				displayHeight;
extern monitors			*monitorData;
extern unsigned int		numberOfMonitors;
extern XContext			listeners;
extern const char		*progname;

//wm prefs
extern int				placement;
extern	unsigned int	numberOfDesktops;
extern char				*titleFont;
extern char				*fontColours[4];
extern int				liveUpdate;

void errorf(const char *,...);
void setlistener(Window,const struct listener *);
int redirect(XEvent *,Window);

extern int doswapdesk;

#endif
