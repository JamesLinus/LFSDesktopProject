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
#include "LFSTKlib.h"

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

	XSetForeground(this->display,this->gc,this->normalColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	XSetForeground(this->display,this->gc,this->whiteColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
	XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
	XSetForeground(this->display,this->gc,this->blackColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
	XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
}

void LFSTK_buttonClass::mouseDown()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->normalColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	XSetForeground(this->display,this->gc,this->blackColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
	XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
	XSetForeground(this->display,this->gc,this->whiteColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
	XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
}

void LFSTK_buttonClass::mouseUp()
{
	if(this->inWindow==false)
		this->LFSTK_clearWindow();
	else
		this->mouseEnter();
}

void LFSTK_buttonClass::mouseExit()
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
}

void LFSTK_buttonClass::mouseEnter()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->highlightColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	XSetForeground(this->display,this->gc,this->whiteColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
	XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
	XSetForeground(this->display,this->gc,this->blackColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
	XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
	this->inWindow=true;
}

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

LFSTK_buttonClass::LFSTK_buttonClass(Display *dsp,Window parent,int x,int y,int w,int h,int gravity,char* colnorm,char* colhi)
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
	XSelectInput(this->display,this->window,SubstructureRedirectMask|Button1MotionMask|ButtonReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);
	this->normalColour=this->LFSTK_setColour(colnorm);
	this->highlightColour=this->LFSTK_setColour(colhi);

	this->gc=XCreateGC(this->display,this->parent,0,NULL);

	this->listen.function=gadgetEvent;
	this->listen.pointer=this;
}

LFSTK_buttonClass::LFSTK_buttonClass(LFSTK_windowClass* wc,char* label,int x,int y,int w,int h,int gravity,char* colnorm,char* colhi)
{
	XSetWindowAttributes	wa;
	XGCValues	gcv;

	this->display=wc->display;
	this->parent=wc->window;

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;

	this->screen=wc->screen;
	this->visual=wc->visual;
	this->rootWindow=wc->rootWindow;
	this->cm=wc->cm;

	wa.bit_gravity=NorthWestGravity;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
	XSelectInput(this->display,this->window,SubstructureRedirectMask|Button1MotionMask|ButtonReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);
	this->normalColour=this->LFSTK_setColour(colnorm);
	this->highlightColour=this->LFSTK_setColour(colhi);

	this->gc=XCreateGC(this->display,this->parent,0,NULL);

	this->listen.function=gadgetEvent;
	this->listen.pointer=this;
}