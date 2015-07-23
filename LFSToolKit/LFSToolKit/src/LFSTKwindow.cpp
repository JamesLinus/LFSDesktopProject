/*
 *
 * ©K. D. Hedger. Thu 23 Jul 16:14:48 BST 2015 kdhedger68713@gmail.com

 * This file (window.cpp) is part of LFSToolKit.

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

#include "window.h"

LFSTK_windowClass::~LFSTK_windowClass()
{
}

LFSTK_windowClass::LFSTK_windowClass()
{
}

LFSTK_windowClass::LFSTK_windowClass(int x,int y,int w,int h)
{
	XSetWindowAttributes	wa;

	this->display=XOpenDisplay(NULL);
	if(this->display==NULL)
		exit(1);

	this->screen=DefaultScreen(this->display);
	this->visual=DefaultVisual(this->display,this->screen);
	this->rootWindow=DefaultRootWindow(this->display);

	wa.bit_gravity=NorthWestGravity;
	this->window=XCreateWindow(this->display,this->rootWindow,this->x,this->y,this->w,this->h,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,0);

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
}


