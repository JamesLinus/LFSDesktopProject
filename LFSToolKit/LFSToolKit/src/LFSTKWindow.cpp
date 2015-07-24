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

#include <X11/Xlib.h>

#include "LFSTKWindow.h"

LFSTK_windowClass::~LFSTK_windowClass()
{
	XFreeGC(this->display,this->gc);
	XDestroyWindow(this->display,this->window);
	XCloseDisplay(this->display);
}

LFSTK_windowClass::LFSTK_windowClass()
{
}

void LFSTK_windowClass::LFSTK_ClearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);
	XSetForeground(this->display,this->gc,this->whiteColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,1000,1000);
}

void LFSTK_windowClass::LFSTK_setlistener(Window w,const struct listener *l)
{
	if (l==NULL)
		XDeleteContext(this->display,w,this->listeners);
	else
		XSaveContext(this->display,w,this->listeners,(XPointer)l);
}

LFSTK_windowClass::LFSTK_windowClass(int x,int y,int w,int h)
{
	XSetWindowAttributes	wa;
	Atom					wm_delete_window;

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
	XSelectInput(this->display,this->window,SubstructureRedirectMask | ButtonPressMask | ButtonReleaseMask | ExposureMask);
	this->gc=XCreateGC(this->display,this->window,0,0);

	XSetWMProtocols(this->display,this->window,&wm_delete_window,1);
 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);
	this->LFSTK_ClearWindow();
}


