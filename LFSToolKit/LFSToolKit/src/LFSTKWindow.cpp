/*
 *
 * ©K. D. Hedger. Thu 23 Jul 16:14:48 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKwindow.cpp) is part of LFSToolKit.

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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#include "LFSTKWindow.h"

LFSTK_windowClass::~LFSTK_windowClass()
{
	XFreeGC(this->display,this->gc);
	XDeleteContext(this->display,this->window,this->listeners);
	XDestroyWindow(this->display,this->window);
	XCloseDisplay(this->display);
}

LFSTK_windowClass::LFSTK_windowClass()
{
}

unsigned long LFSTK_windowClass::LFSTK_setColour(const char *name)
{
	XColor tc,sc;
	XAllocNamedColor(this->display,this->cm,name,&sc,&tc);
	return sc.pixel;
}

void LFSTK_windowClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);
	XSetForeground(this->display,this->gc,this->whiteColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
}

void LFSTK_windowClass::LFSTK_setListener(Window w,const struct listener *l)
{
	if (l==NULL)
		XDeleteContext(this->display,w,this->listeners);
	else
		XSaveContext(this->display,w,this->listeners,(XPointer)l);
}

void LFSTK_windowClass::LFSTK_resizeWindow(int w,int h)
{
	this->w=w;
	this->h=h;
	this->LFSTK_clearWindow();
}

struct listener* LFSTK_windowClass::LFSTK_getListener(Window w)
{
	struct listener *l;
	if (XFindContext(this->display,w,this->listeners,(XPointer *)&l)==0)
		return l;
	else
		return NULL;
}

LFSTK_windowClass::LFSTK_windowClass(int x,int y,int w,int h,char* foreground,char* background)
{
	XSetWindowAttributes	wa;
	Atom					wm_delete_window;
	XGCValues				gcv;

	this->display=XOpenDisplay(NULL);
	if(this->display==NULL)
		exit(1);

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;

	this->screen=DefaultScreen(this->display);
	this->visual=DefaultVisual(this->display,this->screen);
	this->rootWindow=DefaultRootWindow(this->display);
	this->cm=DefaultColormap(this->display,this->screen);


	wa.bit_gravity=NorthWestGravity;
	wm_delete_window=XInternAtom(this->display,"WM_DELETE_WINDOW",0);

	this->window=XCreateWindow(this->display,this->rootWindow,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
	XSelectInput(this->display,this->window, ButtonPressMask | ButtonReleaseMask | ExposureMask|StructureNotifyMask);

	XSetWMProtocols(this->display,this->window,&wm_delete_window,1);
 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);
	this->foreColour=this->LFSTK_setColour(foreground);
	this->backColour=this->LFSTK_setColour(background);

	gcv.foreground=this->foreColour;
	gcv.background=this->backColour;
	this->gc=XCreateGC(this->display,this->rootWindow,GCForeground | GCBackground,&gcv);

	this->listeners=XUniqueContext();
//	this->listen.function=gadgetEvent;
//	this->listen.pointer=this;
//	this->listen.userData=-1;

//	XSaveContext(this->display,this->window,this->listeners,(XPointer)&(this->listen));	
}


