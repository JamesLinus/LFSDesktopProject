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

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "wind.h"
#include "button.h"
#include "lib.h"
#include "frame.h"
#include "client.h"

bitmap			*deletebitmap;
bitmap			*maximizeBitmap;
bitmap			*minimizeBitmap;
bitmap			*shadeBitmap;

void update(struct button *b)
{
	bool	invert;
	int		usepixnum=0;
	int		partoffset=0;


	if(theme.useTheme==true)
		{
			//if(chasfocus(b->f->client)==true)
			if((b->f!=NULL) && (b->f->client!=NULL) && (chasfocus(b->f->client)==true))
				{
					//if(chasfocus(b->f->client)==true)
						partoffset=0;
				}
			else
				partoffset=1;

	if(partoffset==1)
		usepixnum=1;
	else
		{
			usepixnum=0;

			if(b->entered)
				usepixnum=2;

			if(b->pressed && b->entered)
				usepixnum=3;
		}
	GC gc=XCreateGC(dpy,b->window,0,NULL);
//XCopyArea(dpy,b->pixmap,b->window,fg,0,0,b->width,b->height,0,0);

	XSetForeground(dpy,gc,whiteColor);
	XSetClipMask(dpy,gc,None);
	XSetFillStyle(dpy,gc,FillTiled);
	XSetTile(dpy,gc,theme.pixmaps[TITLE5ACTIVE+partoffset]);
	XFillRectangle(dpy,b->window,gc,0,0,b->width,b->height);

	usepixnum=b->buttonNumber+usepixnum;
	XSetClipMask(dpy,gc,theme.masks[usepixnum]);
	XSetClipOrigin(dpy,gc,0,0);
	XCopyArea(dpy,theme.pixmaps[usepixnum],b->window,gc,0,0,theme.partsWidth[usepixnum],theme.partsHeight[usepixnum],0,0);
	//XCopyArea(dpy,theme.pixmaps[CLOSEACTIVE],b->window,gc,0,0,16,16,0,0);
	//printf("CLOSEACTIVE=%i b->buttonNumber+usepixnum=%i<<\n",CLOSEACTIVE,b->buttonNumber+usepixnum);
	

//	invert=b->pressed && b->entered;
//	if(invert)
//		{
//		}
//	else
//		{
//			XCopyArea(dpy,theme.parts[,b->window,fg,0,0,b->width,b->height,0,0);
//		}
		}
	else
		{
#if 1
	invert=b->pressed && b->entered;
	GC fg=invert ? background : foreground;
	GC bg=invert ? foreground : background;

	XFillRectangle(dpy,b->pixmap,bg,0,0,b->width,b->height);

	drawbitmap(b->pixmap,fg,b->bitmap,
	           (b->width-b->bitmap->width) / 2,
	           (b->height-b->bitmap->height) / 2);

	if (!invert)
		{
			//XSetLineAttributes(dpy,fg,b->entered ? 1 + 2 * halfleading : 0,LineSolid,CapButt,JoinMiter);
			XSetLineAttributes(dpy,fg,b->entered ? 1 + 2 * 2 : 0,LineSolid,CapButt,JoinMiter);
			XDrawRectangle(dpy,b->pixmap,fg,0,0,b->width-1,b->height-1);
			XSetLineAttributes(dpy,fg,0,LineSolid,CapButt,JoinMiter);
		}

	XCopyArea(dpy,b->pixmap,b->window,fg,0,0,b->width,b->height,0,0);
#endif
	}
}
#if 0
void update(struct button *b)
{
	Bool invert=b->pressed && b->entered;
	GC fg=invert ? background : foreground;
	GC bg=invert ? foreground : background;

	XFillRectangle(dpy,b->pixmap,bg,0,0,b->width,b->height);

	drawbitmap(b->pixmap,fg,b->bitmap,
	           (b->width-b->bitmap->width) / 2,
	           (b->height-b->bitmap->height) / 2);

	if (!invert)
		{
			//XSetLineAttributes(dpy,fg,b->entered ? 1 + 2 * halfleading : 0,LineSolid,CapButt,JoinMiter);
			XSetLineAttributes(dpy,fg,b->entered ? 1 + 2 * 2 : 0,LineSolid,CapButt,JoinMiter);
			XDrawRectangle(dpy,b->pixmap,fg,0,0,b->width-1,b->height-1);
			XSetLineAttributes(dpy,fg,0,LineSolid,CapButt,JoinMiter);
		}

	XCopyArea(dpy,b->pixmap,b->window,fg,0,0,b->width,b->height,0,0);
}
#endif

void buttonpress(struct button *b,XButtonEvent *e)
{
	b->pressed=True;
	update(b);
}

void buttonrelease(struct button *b,XButtonEvent *e)
{
	if (e->button==Button1)
		{
			if (b->pressed && b->entered)
				b->function(b->arg,e->time);
			b->pressed=False;
			update(b);
		}
}

void enternotify(struct button *b,XCrossingEvent *e)
{
	b->entered=True;
	update(b);
}

void leavenotify(struct button *b,XCrossingEvent *e)
{
	if (b->entered)
		{
			b->entered=False;
			update(b);
		}
}

void unmapnotify(struct button *b,XUnmapEvent *e)
{
	if (b->pressed)
		{
			b->pressed=False;
			update(b);
		}
}

void expose(struct button *b,XExposeEvent *e)
{
	if(theme.useTheme==true)
		update(b);
	else
		XCopyArea(dpy,b->pixmap,b->window,foreground,e->x,e->y,e->width,e->height,e->x,e->y);
}

void buttonevent(void *self,XEvent *e)
{
	switch (e->type)
		{
		case Expose:
			expose((button*)self,&e->xexpose);
			break;
		case EnterNotify:
			enternotify((button*)self,&e->xcrossing);
			break;
		case LeaveNotify:
			leavenotify((button*)self,&e->xcrossing);
			break;
		case ButtonPress:
			buttonpress((button*)self,&e->xbutton);
			break;
		case ButtonRelease:
			buttonrelease((button*)self,&e->xbutton);
			break;
		case UnmapNotify:
			unmapnotify((button*)self,&e->xunmap);
			break;
		}
}

struct button *bcreate(void (*function)(void *,Time),void *arg,struct bitmap *bitmap,Window parent,int x,int y,int width,int height,int gravity,int buttonnum,struct frame *f)
{
	XSetWindowAttributes	sa;
	sa.win_gravity=gravity;

	struct button *b=(button*)xmalloc(sizeof *b);
	b->function=function;
	b->arg=arg;
	b->bitmap=bitmap;
	b->width=width;
	b->height=height;
	b->pixmap=XCreatePixmap(dpy,root,width,height,DefaultDepth(dpy,screen));
	b->pressed=False;
	b->entered=False;
	b->window=XCreateWindow(dpy,parent,x,y,width,height,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&sa);
	b->listen.function=buttonevent;
	b->listen.pointer=b;
	b->buttonNumber=buttonnum;
	b->f=f;

	setlistener(b->window,&b->listen);
	XGrabButton(dpy,Button1,AnyModifier,b->window,False,EnterWindowMask | LeaveWindowMask | ButtonReleaseMask,GrabModeAsync,GrabModeAsync,None,None);
	XSelectInput(dpy,b->window,EnterWindowMask | LeaveWindowMask | StructureNotifyMask | ExposureMask);
	update(b);
	XMapWindow(dpy,b->window);
	
	return b;
}

void bdestroy(struct button *b)
{
	setlistener(b->window,NULL);
	XFreePixmap(dpy,b->pixmap);
	XDestroyWindow(dpy,b->window);
	free(b);
}
