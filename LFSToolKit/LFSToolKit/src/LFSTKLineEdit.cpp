/*
 *
 * ©K. D. Hedger. Wed  5 Aug 12:36:15 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKLineEdit.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not,see <http://www.gnu.org/licenses/>.
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
	this->cursorPos=0;
	this->buffer="";

	XA_CLIPBOARD=XInternAtom(this->display,"CLIPBOARD",True);
	XA_COMPOUND_TEXT=XInternAtom(this->display,"COMPOUND_TEXT",true);
	XA_UTF8_STRING=XInternAtom(this->display,"UTF8_STRING",true);
	XA_TARGETS=XInternAtom(this->display,"TARGETS",True);
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

	this->drawLabel();
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
	this->gotFocus=true;
}

void LFSTK_lineEditClass::mouseExit(void)
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
}

void LFSTK_lineEditClass::lostFocus(XEvent *e)
{
	XUngrabKeyboard(this->display,CurrentTime);
	this->gotFocus=false;
	this->LFSTK_clearWindow();
	this->inWindow=false;
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
	this->drawLabel();
}

void LFSTK_lineEditClass::drawLabel(void)
{
	int x;

	drawUtf8String(this->wc,this->window,(XftFont*)(this->font->data),2,(this->h/2)+((this->wc->font->ascent-2)/2),"black",this->buffer.c_str());
	if(this->gotFocus==true)
		{
			x=getTextwidth(this->display,(XftFont*)(this->font->data),this->buffer.substr(0,this->cursorPos).c_str());
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,2+x,3,2+x,this->h-3);
		}
}

void LFSTK_lineEditClass::getClip(void)
{
	Window			selectionOwner;
	unsigned char	*data=NULL;
	Atom			type;
	int				format,result;
	unsigned long	len,bytesLeft,dummy;
	bool			run=true;
	XEvent			event;

	selectionOwner=XGetSelectionOwner(this->display,XA_CLIPBOARD);
	if (selectionOwner!=None)
		{
			XConvertSelection(this->display,XA_CLIPBOARD,XA_UTF8_STRING,XA_CLIPBOARD,this->window,CurrentTime);
			XFlush(this->display);

			while (run==true)
				{
					XNextEvent(this->display,&event);
					switch(event.type)
						{
							case SelectionNotify:
								if(event.xselection.requestor==this->window)
									run=false;
								break;
						}
				}

			XGetWindowProperty(this->display,this->window,XA_CLIPBOARD,0,0,False,AnyPropertyType,&type,&format,&len,&bytesLeft,&data);
			if (data)
				{
					XFree(data);
					data=NULL;
				}

			// If there is any data
			if (bytesLeft)
				{
					// Fetch the data
					result=XGetWindowProperty(this->display,this->window,XA_CLIPBOARD,0,bytesLeft,False,AnyPropertyType,&type,&format,&len,&dummy,&data);

					// If we got some data,duplicate it
					if (result==Success)
						{
							this->buffer.insert(this->cursorPos,(char*)data);
							this->cursorPos+=strlen((char*)data);
							XFree(data);
						}
				}

			// Delete the property now that we are finished with it
			XDeleteProperty(this->display,this->window,XA_CLIPBOARD);
		}
}

void LFSTK_lineEditClass::keyRelease(XKeyEvent *e)
{
	int		x;
	char	c[255];
	KeySym	keysym_return;

	XLookupString(e,(char*)&c,255,&keysym_return,NULL);

	if(e->state==ControlMask)
		{
			if(keysym_return==XK_v)
				this->getClip();
		}
	else
		{
			switch(keysym_return)
				{
				case XK_BackSpace:
					if(this->cursorPos>0)
						{
							this->buffer.erase(this->cursorPos-1,1);
							this->cursorPos--;
						}
					break;
				case XK_Left:
					if(this->cursorPos>0)
						this->cursorPos--;
					break;
				case XK_Right:
					if(this->cursorPos<this->buffer.length())
						this->cursorPos++;
					break;
				case XK_End:
					this->cursorPos=this->buffer.length();
					break;
				case XK_Home:
					this->cursorPos=0;
					break;

				default:
					this->buffer.insert(this->cursorPos,1,c[0]);
					this->cursorPos++;
					break;
				}
		}
	this->LFSTK_clearWindow();
	this->drawLabel();
	//printf("--%i--\n",this->cursorPos);
	//printf("--%i--\n",e->state);
}

