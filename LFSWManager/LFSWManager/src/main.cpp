/*
 * Copyright 2010 Johan Veenhuizen
 *
 * Permission is hereby granted,free of charge,to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction,including without limitation
 * the rights to use,copy,modify,merge,publish,distribute,sublicense,
 * and/or sell copies of the Software,and to permit persons to whom the
 * Software is furnished to do so,subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS",WITHOUT WARRANTY OF ANY KIND,EXPRESS OR
 * IMPLIED,INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,DAMAGES OR
 * OTHER LIABILITY,WHETHER IN AN ACTION OF CONTRACT,TORT OR OTHERWISE,
 * ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xinerama.h>

#include <Imlib2.h>

#include "wind.h"
#include "x11font.h"
#include "list.h"
#include "lib.h"
#include "root.h"
#include "ewmh.h"
#include "client.h"
#include "mwm.h"
#include "prefs.h"
#include "button.h"
#include "atoms.h"

#include "deleven.xbm"
#include "delodd.xbm"
#include "maxeven.xbm"
#include "maxodd.xbm"
#include "mineven.xbm"
#include "minodd.xbm"
#include "shadeeven.xbm"
#include "shadeodd.xbm"

DEFINE_BITMAP(deleven);
DEFINE_BITMAP(delodd);
DEFINE_BITMAP(maxeven);
DEFINE_BITMAP(maxodd);
DEFINE_BITMAP(mineven);
DEFINE_BITMAP(minodd);
DEFINE_BITMAP(shadeeven);
DEFINE_BITMAP(shadeodd);

enum			runlevel runlevel=RL_STARTUP;
int				exitstatus;

/*
 * If true,enable debug mode. This will enable synchronous
 * X11 transactions,and print Xlib errors to standard error.
 */
Bool			debug=False;

// The display name used in call to XOpenDisplay
const char		*displayname=NULL;

// The last X error reported
const char		*xerror=NULL;

Window 			root;
unsigned long	foregroundpixel;
unsigned long	backgroundpixel;
unsigned long	hlforegroundpixel;
unsigned long	hlbackgroundpixel;
GC				foreground;
GC				background;
GC				hlforeground;
GC				hlbackground;

//int				lineheight;
//int				halfleading;
sigset_t		sigmask;

int errhandler(Display *dpy,XErrorEvent *e)
{
	char buf[128];
	buf[0]='\0';
	XGetErrorText(dpy,e->error_code,buf,sizeof buf);
	if (debug)
		errorf("Xlib: %s",buf);
	xerror=buf;
	return 0;
}

void onsignal(int signo)
{
}

int waitevent(void)
{
	if (XPending(dpy)>0)
		return 0;
	fd_set rfds;
	int nfds=0;
	FD_ZERO(&rfds);

	int conn=ConnectionNumber(dpy);
	FD_SET(conn,&rfds);
	nfds=MAX(conn + 1,nfds);

	if (pselect(nfds,&rfds,NULL,NULL,NULL,&sigmask)==-1)
		{
			errorf("pselect: %s",strerror(errno));
			exitstatus=1;
			return -1;
		}
	else if (FD_ISSET(conn,&rfds))
		{
			// Normal X event
			return 0;
		}
	else
		{
			// Shouldn't happen
			errorf("BUG: unhandled pselect condition");
			exitstatus=1;
			return -1;
		}
}

void usage(FILE *f)
{
	fprintf(f,"%s\nUsage: %s [[ OPT ] ... [ OPT ]] [ DISPLAY ]\n"
	        "-n	number		Number of desktops\n"
	        "-t	font		Title font\n"
	        "-f	colour		Forecolour\n"
	        "-b	colour		back colour\n"
	        "-F	colour		Focused forecolour\n"
	        "-B	colour		Focusud backcolour\n"
	        "-p	placement	New window placement (0=Smart( Screen ), 1=Under mouse ,2=Centre on monitor with mouse( default ), 3=Screen centre, 4=Smart( Monitor with mouse ) )\n"
			"-l	updates		Live update of window when resizing ( >0=Live ( default=5 - slowest=1, faster/less updates >1 ), 0=Update window on relase of mouse button - fastest\n"
			"-w	outfile		After setting prefs from default, prefsfile and command line write out a prefsfile and quit ( MUST be last option ).\n"
	        ,PACKAGE_STRING,progname);
}

const char	*themePartNames[]=
{
	"top-left-active",
	"top-left-inactive",
	"top-right-active",
	"top-right-inactive",
	"title-1-active",
	"title-1-inactive",
	"title-2-active",
	"title-2-inactive",
	"title-3-active",
	"title-3-inactive",
	"title-4-active",
	"title-4-inactive",
	"title-5-active",
	"title-5-inactive",
	"left-active",
	"left-inactive",
	"right-active",
	"right-inactive",
	"bottom-left-active",
	"bottom-left-inactive",
	"bottom-active",
	"bottom-inactive",
	"bottom-right-active",
	"bottom-right-inactive",
	NULL
};

void loadTheme(void)
{
	char		buffer[2048];
	int			partcnt=0;
	int			hite=0;
	Imlib_Image	image;
	GC			inversegc;

	imlib_context_set_dither(0);
	imlib_context_set_display(dpy);
	imlib_context_set_visual(visual);

	while(themePartNames[partcnt]!=NULL)
		{
			theme.gotPart[partcnt]=false;
			sprintf(buffer,"%s/xfwm4/%s.png",theme.pathToTheme,themePartNames[partcnt]);
			image=imlib_load_image(buffer);
			if(image==NULL)
				{
					sprintf(buffer,"%s/xfwm4/%s.xpm",theme.pathToTheme,themePartNames[partcnt]);
					image=imlib_load_image(buffer);
				}

			if(image!=NULL)
				{
					theme.gotPart[partcnt]=true;
					imlib_context_set_image(image);

					hite=imlib_image_get_height();
					theme.partsWidth[partcnt]=imlib_image_get_width();
					if(hite>theme.titleBarHeight)
						theme.titleBarHeight=hite;
					theme.partsHeight[partcnt]=hite;

					imlib_context_set_drawable(root);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&theme.pixmaps[partcnt],&theme.masks[partcnt]);
					imlib_free_image();

					theme.inverseMasks[partcnt]=XCreatePixmap(dpy,root,theme.partsWidth[partcnt],theme.partsHeight[partcnt],1);
					inversegc=XCreateGC(dpy,theme.inverseMasks[partcnt],0,NULL);
					XSetForeground(dpy,inversegc,whiteColor);
					XFillRectangle(dpy,theme.inverseMasks[partcnt],inversegc,0,0,theme.partsWidth[partcnt],theme.partsHeight[partcnt]);
					XSetForeground(dpy,inversegc,blackColor);
					XSetClipOrigin(dpy,inversegc,0,0);
					XSetClipMask(dpy,inversegc,theme.masks[partcnt]);
					XFillRectangle(dpy,theme.inverseMasks[partcnt],inversegc,0,0,theme.partsWidth[partcnt],theme.partsHeight[partcnt]);
					XFreeGC(dpy,inversegc);
				}
			partcnt++;
		}

	theme.leftWidth=theme.partsWidth[LEFTACTIVE];
	theme.rightWidth=theme.partsWidth[RIGHTACTIVE];
	theme.titleBarHeight=theme.partsHeight[TITLE3ACTIVE];
	theme.bottomHeight=theme.partsHeight[BOTTOMACTIVE];
}

int main(int argc,char *argv[])
{
	int					cnt=-1;
	XineramaScreenInfo	*p=NULL;
	char				*prefsfile;

	progname=argv[0];
	// The Xmb* functions use LC_CTYPE
	setlocale(LC_CTYPE,"");

	srand((unsigned)time(NULL));

	runlevel=RL_STARTUP;

	fontColours[FOCUSEDFORE]=strdup("rgb:00/00/00");
	fontColours[FOCUSEDBACK]=strdup("rgb:00/ff/ff");
	fontColours[FORE]=strdup("rgb:00/00/00");
	fontColours[BACK]=strdup("rgb:ff/ff/ff");
	Desk ndesk=0;
	numberOfDesktops=4;
	placement=CENTREMMONITOR;
	titleFont=strdup(DEFAULTFONT);
	liveUpdate=5;

	asprintf(&prefsfile,"%s/.config/LFS/lfswmanager.rc",getenv("HOME"));
	loadVarsFromFile(prefsfile,wmPrefs);
	free(prefsfile);

	ndesk=numberOfDesktops;

	int opt;
	while ((opt=getopt(argc,argv,"?hp:B:b:F:f:n:t:l:w:v")) != -1)
		switch (opt)
			{
			case 'B':
				free(fontColours[FOCUSEDBACK]);
				fontColours[FOCUSEDBACK]=strdup(optarg);
				break;
			case 'b':
				free(fontColours[BACK]);
				fontColours[BACK]=strdup(optarg);
				break;
			case 'F':
				free(fontColours[FOCUSEDFORE]);
				fontColours[FOCUSEDFORE]=strdup(optarg);
				break;
			case 'f':
				free(fontColours[FORE]);
				fontColours[FORE]=strdup(optarg);
				break;
			case 'n':
				{
					errno=0;
					char *p;
					long n=strtol(optarg,&p,10);
					if ((n<0) || (errno != 0) || (*optarg=='\0') || (*p != '\0'))
						{
							errorf("%s: invalid desktop count",optarg);
							exit(1);
						}
					ndesk=n;
				}
				break;
			case 't':
				free(titleFont);
				titleFont=strdup(optarg);
				break;
			case 'p':
				placement=atoi(optarg);
				break;

			case 'l':
				liveUpdate=atoi(optarg);
				break;

			case 'w':
				saveVarsToFile(optarg,wmPrefs);
				exit(0);
				break;

			case 'v':
				debug=True;
				break;
			default:
				usage(stderr);
				exit(1);
			}

	if (optind<argc)
		displayname=argv[optind++];

	if (optind<argc)
		{
			errorf("unexpected argument -- %s",argv[optind]);
			usage(stderr);
			exit(1);
		}

	if (debug)
		{
			fprintf(stderr,"%s\n",PACKAGE_STRING);
			fprintf(stderr,"Synchronous DEBUG mode enabled. "
			        "Printing Xlib errors on standard error.\n");
			fprintf(stderr,"Report bugs to <kdhedger68713@gmail.com>.\n");
		}

	XSetErrorHandler(errhandler);

	if ((dpy=XOpenDisplay(displayname))==NULL)
		{
			errorf("cannot open display \"%s\"",XDisplayName(displayname));
			exit(1);
		}

	XSynchronize(dpy,debug);

	screen=DefaultScreen(dpy);
	displayWidth=DisplayWidth(dpy,screen);
	displayHeight=DisplayHeight(dpy,screen);

	root=DefaultRootWindow(dpy);
	visual=DefaultVisual(dpy,screen);
	blackColor=BlackPixel(dpy,screen);
	whiteColor=WhitePixel(dpy,screen);

	cnt=ScreenCount(dpy);
	p=XineramaQueryScreens(dpy,&cnt);
	if(p!=NULL)
		{
			if(cnt>0)
				{
					monitorData=(monitors*)malloc(sizeof(monitors)*cnt);
					numberOfMonitors=cnt;

					for (int x=0; x<cnt; x++)
						{
							monitorData[x].monW=p[x].width;
							monitorData[x].monH=p[x].height;
							monitorData[x].monX=p[x].x_org;
							monitorData[x].monY=p[x].y_org;
						}
				}
			free(p);
		}

	font=ftload(titleFont);
	if (font==NULL)
		{
			errorf("cannot load font");
			exit(1);
		}
	fnormal=ftloadcolor(fontColours[FORE]);
	fhighlight=ftloadcolor(fontColours[FOCUSEDFORE]);
	if (fnormal==NULL || fhighlight==NULL)
		{
			errorf("cannot load font colors");
			exit(1);
		}

//printf("hl=%i lh=%i fs=%i\n",leading,frameTop,font->size);
//	asprintf(&theme.pathToTheme,"%s","/home/keithhedger/.themes/OldWoodAndBrass");
//	asprintf(&theme.pathToTheme,"%s","/home/keithhedger/.themes/CaptainAmerica");
//	asprintf(&theme.pathToTheme,"%s","/home/keithhedger/.themes/OldBrownWood");
//	asprintf(&theme.pathToTheme,"%s","/home/keithhedger/.themes/HiberNation");
//	asprintf(&theme.pathToTheme,"%s","/usr/share/themes/Crux");
//	asprintf(&theme.pathToTheme,"%s","/usr/share/themes/G2");
//	asprintf(&theme.pathToTheme,"%s","/usr/share/themes/B6");
//	asprintf(&theme.pathToTheme,"%s","/home/keithhedger/.themes/OldyXmasTheme");
	asprintf(&theme.pathToTheme,"%s","/home/keithhedger/.themes/WoodThemeRustic");

	loadTheme();
	theme.useTheme=true;

	if(theme.useTheme==false)
		{
			frameTop=20;
			frameBottom=8;
			frameLeft=2+1;
			frameRight=2+1;
		}
	else
		{
			frameTop=theme.titleBarHeight;
			frameBottom=theme.bottomHeight;
			frameLeft=theme.leftWidth;
			frameRight=theme.rightWidth;
		}


	if (frameTop % 2==0)
		deletebitmap=&deleven;
	else
		deletebitmap=&delodd;

	if (frameTop % 2==0)
		maximizeBitmap=&maxeven;
	else
		maximizeBitmap=&maxodd;

	if (frameTop % 2==0)
		minimizeBitmap=&mineven;
	else
		minimizeBitmap=&minodd;

	if (frameTop % 2==0)
		shadeBitmap=&shadeeven;
	else
		shadeBitmap=&shadeodd;

	foregroundpixel=getpixel(fontColours[FORE]);
	backgroundpixel=getpixel(fontColours[BACK]);
	hlforegroundpixel=getpixel(fontColours[FOCUSEDFORE]);
	hlbackgroundpixel=getpixel(fontColours[FOCUSEDBACK]);

	XGCValues	gcv;

	gcv.foreground=foregroundpixel;
	gcv.background=backgroundpixel;
	foreground=XCreateGC(dpy,root,GCForeground | GCBackground,&gcv);

	gcv.foreground=backgroundpixel;
	gcv.background=foregroundpixel;

	background=XCreateGC(dpy,root,GCForeground | GCBackground,&gcv);

	gcv.foreground=hlforegroundpixel;
	gcv.background=hlbackgroundpixel;
	hlforeground=XCreateGC(dpy,root,GCForeground | GCBackground,&gcv);

	gcv.foreground=hlbackgroundpixel;
	gcv.background=hlforegroundpixel;
	hlbackground=XCreateGC(dpy,root,GCForeground | GCBackground,&gcv);

	listeners=XUniqueContext();

	sigset_t sigsafemask;
	sigprocmask(SIG_SETMASK,NULL,&sigmask);
	sigprocmask(SIG_SETMASK,NULL,&sigsafemask);

	struct sigaction sa;
	sigfillset(&sa.sa_mask);
	sa.sa_flags=SA_RESTART;
	sa.sa_handler=onsignal;
	struct sigaction osa;

	sigaction(SIGHUP,NULL,&osa);
	if (osa.sa_handler != SIG_IGN)
		{
			sigaction(SIGHUP,&sa,NULL);
			sigaddset(&sigsafemask,SIGHUP);
		}

	sigaction(SIGINT,NULL,&osa);
	if (osa.sa_handler != SIG_IGN)
		{
			sigaction(SIGINT,&sa,NULL);
			sigaddset(&sigsafemask,SIGINT);
		}

	sigaction(SIGTERM,NULL,&osa);
	if (osa.sa_handler != SIG_IGN)
		{
			sigaction(SIGTERM,&sa,NULL);
			sigaddset(&sigsafemask,SIGTERM);
		}

	sigprocmask(SIG_SETMASK,&sigsafemask,NULL);

	WM_CHANGE_STATE=XInternAtom(dpy,"WM_CHANGE_STATE",False);
	WM_DELETE_WINDOW=XInternAtom(dpy,"WM_DELETE_WINDOW",False);
	WM_PROTOCOLS=XInternAtom(dpy,"WM_PROTOCOLS",False);
	WM_STATE=XInternAtom(dpy,"WM_STATE",False);

	initroot();
	ewmh_startwm();
	mwm_startwm();

	if (ndesk != 0)
		setndesk(ndesk);

	Cursor curs=XCreateFontCursor(dpy,XC_left_ptr);
	XDefineCursor(dpy,root,curs);

	XSetInputFocus(dpy,PointerRoot,RevertToPointerRoot,CurrentTime);
	manageall();

	refocus(CurrentTime);

	runlevel=RL_NORMAL;

	while (waitevent() != -1)
		{
			XEvent e;
			XNextEvent(dpy,&e);
			if (redirect(&e,e.xany.window)==-1)
				{
					/*
					 * EWMH specifies some root window client
					 * messages with a non-root event window,
					 * so we need to redirect those manually.
					 */
					if (e.type==ClientMessage)
						redirect(&e,root);
				}

			shuffle();
			restack();
		}

	runlevel=RL_SHUTDOWN;

	// We make sure the focused window stays on top
	// when we map windows from other desktops,and
	// to warp the pointer so that focus is not lost.
	Window w=None;
	struct geometry g;
	struct client *c=getfocus();
	if (c != NULL)
		{
			cpopapp(c);
			restack();
			w=c->window;
			g=cgetgeom(c);
		}

	unmanageall();

	if (w != None)
		XWarpPointer(dpy,None,w,0,0,0,0,g.width / 2,g.height / 2);
	XSetInputFocus(dpy,PointerRoot,RevertToPointerRoot,CurrentTime);

	ewmh_stopwm();

	ftfreecolor(fnormal);
	ftfreecolor(fhighlight);
	ftfree(font);

	XFreeGC(dpy,foreground);
	XFreeGC(dpy,background);
	XFreeGC(dpy,hlforeground);
	XFreeGC(dpy,hlbackground);
	XCloseDisplay(dpy);

	return exitstatus;
}
