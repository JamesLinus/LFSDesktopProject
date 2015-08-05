/*
 *
 * ©K. D. Hedger. Wed  5 Aug 12:36:15 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKLineEdit.cpp) is part of LFSToolKit.

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

#include "LFSTKLineEdit.h"
#include "lib.h"


LFSTK_lineEditClass::~LFSTK_lineEditClass()
{
}

LFSTK_lineEditClass::LFSTK_lineEditClass()
{
	if(this->label!=NULL)
		free(this->label);
	XDestroyWindow(this->display,this->window);
}

LFSTK_lineEditClass::LFSTK_lineEditClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);
	wa.bit_gravity=NorthWestGravity;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask|FocusChangeMask|KeyReleaseMask);

	this->listen.function=gadgetEvent;
	this->listen.pointer=this;
	this->listen.type=LINEEDITGADGET;
	this->wc->LFSTK_setListener(this->window,this->LFSTK_getListen());
}

void LFSTK_lineEditClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,whiteColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	XSetForeground(this->display,this->gc,this->blackColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
	XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
	XSetForeground(this->display,this->gc,this->whiteColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
	XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
}

void LFSTK_lineEditClass::mouseUp()
{
	if(this->inWindow==false)
		this->LFSTK_clearWindow();
	else
		{
			this->mouseEnter();
			if(this->callback.releaseCallback!=NULL)
				this->callback.releaseCallback(this,this->callback.userData);
		}
}
void LFSTK_lineEditClass::mouseDown()
{
	XGrabKeyboard(this->display,this->window,true,GrabModeAsync,GrabModeAsync,CurrentTime);
	
//	if(this->callback.pressCallback!=NULL)
//		this->callback.pressCallback(this,this->callback.userData);

}

void LFSTK_lineEditClass::mouseExit()
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
	XUngrabKeyboard(this->display,CurrentTime);
}

void LFSTK_lineEditClass::mouseEnter()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,whiteColour);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	XSetForeground(this->display,this->gc,this->blackColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
	XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
	XSetForeground(this->display,this->gc,this->whiteColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
	XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);

	this->inWindow=true;
}

void LFSTK_lineEditClass::keyRelease(XEvent *e)
{
	char c[255];
	KeySym keysym_return;
	XLookupString((XKeyEvent*)&(e->xkey),(char*)&c,255, &keysym_return, NULL);

printf("key=%c\n",e->xkey.keycode);
printf("key==%c\n",c);
}

