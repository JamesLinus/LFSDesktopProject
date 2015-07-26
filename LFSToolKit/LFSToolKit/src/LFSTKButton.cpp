/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:55:09 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKButton.cpp) is part of LFSToolKit.

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

#include <X11/Xlib.h>

#include "LFSTKButton.h"
#include "lib.h"

LFSTK_buttonClass::~LFSTK_buttonClass()
{
	XFreeGC(this->display,this->gc);
	XDestroyWindow(this->display,this->window);
}

LFSTK_buttonClass::LFSTK_buttonClass()
{
}

void LFSTK_buttonClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->backColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	XSetForeground(this->display,this->gc,this->whiteColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
	XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
	XSetForeground(this->display,this->gc,this->blackColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
	XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
}

//void LFSTK_buttonClass::LFSTK_setlistener(Window w,const struct listener *l)
//{
//	if (l==NULL)
//		XDeleteContext(this->display,w,this->listeners);
//	else
//		XSaveContext(this->display,w,this->listeners,(XPointer)l);
//}

unsigned long LFSTK_buttonClass::LFSTK_setColour(const char *name)
{
	XColor tc,sc;
	XAllocNamedColor(this->display,this->cm,name,&sc,&tc);
	return sc.pixel;
}

struct listener* LFSTK_buttonClass::LFSTK_getListen(void)
{
	return(&(this->listen));
}

void LFSTK_buttonClass::LFSTK_setListenData(listener *l)
{
		this->listen.function=l->function;
		//this->listen.pointer=l->pointer;
}

LFSTK_buttonClass::LFSTK_buttonClass(Display *dsp,Window parent,int x,int y,int w,int h,int gravity,char* foreground,char* background)
{
	XSetWindowAttributes	wa;
	XGCValues	gcv;

	this->display=dsp;
	this->parent=parent;

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;

	this->screen=DefaultScreen(this->display);
	this->visual=DefaultVisual(this->display,this->screen);
	this->rootWindow=DefaultRootWindow(this->display);
	this->cm=DefaultColormap(this->display,this->screen);

	wa.bit_gravity=NorthWestGravity;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
	XSelectInput(this->display,this->window,SubstructureRedirectMask|Button1MotionMask|ButtonReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask);

 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);
	this->foreColour=this->LFSTK_setColour(foreground);
	this->backColour=this->LFSTK_setColour(background);

	gcv.foreground=this->foreColour;
	gcv.background=this->backColour;
	this->gc=XCreateGC(this->display,this->parent,GCForeground | GCBackground,&gcv);

	this->listen.function=gadgetEvent;
	this->listen.pointer=this;
/*

oid setlistener(Window w,const struct listener *l)
{
	CHECKPOINT
	if (l==NULL)
		XDeleteContext(dpy,w,listeners);
	else
		XSaveContext(dpy,w,listeners,(XPointer)l);
}

*/
}

