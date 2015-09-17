/*
 *
 * ©K. D. Hedger. Thu 17 Sep 11:54:58 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKToggleButton.cpp) is part of LFSToolKit.

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
 
#include "LFSTKToggleButton.h"

LFSTK_toggleButtonClass::~LFSTK_toggleButtonClass()
{
}

LFSTK_toggleButtonClass::LFSTK_toggleButtonClass()
{
}

/**
* Main button constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_toggleButtonClass::LFSTK_toggleButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->listen.function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->listen.pointer=this;
	this->listen.type=TOGGLEGADGET;

	this->wc->LFSTK_setListener(this->window,this->LFSTK_getListen());
	this->boxStyle=TOGGLECHECK;

	this->toggleState=false;
}

/**
* Draw a togglebox.
*/
void LFSTK_toggleButtonClass::drawBox(int state)
{
	int boxsize;
	int	topcol=this->whiteColour;
	int bottomcol=this->blackColour;
	int	holdcol;

	if(state==ACTIVECOLOUR)
		{
			topcol=this->blackColour;
			bottomcol=this->whiteColour;
		}
	if(this->toggleState==true)
		{
			holdcol=topcol;
			topcol=bottomcol;
			bottomcol=holdcol;
		}

	boxsize=this->h/2;

	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[state].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,boxsize,boxsize);

	switch(state)
		{
			case NORMALCOLOUR:
			case PRELIGHTCOLOUR:
			case ACTIVECOLOUR:
				XSetForeground(this->display,this->gc,topcol);
				XDrawLine(this->display,this->window,this->gc,0,boxsize,0,0);
				XDrawLine(this->display,this->window,this->gc,0,0,boxsize-1,0);
				XSetForeground(this->display,this->gc,bottomcol);
				XDrawLine(this->display,this->window,this->gc,0,boxsize,boxsize,boxsize);
				XDrawLine(this->display,this->window,this->gc,boxsize,boxsize,boxsize,0);
				
				break;
		}		
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_toggleButtonClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->wc->windowColourNames[NORMALCOLOUR].pixel);

	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
	if(this->inWindow==false)
		this->drawBox(NORMALCOLOUR);
	else
		this->drawBox(PRELIGHTCOLOUR);
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseEnter(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->LFSTK_clearWindow();
	this->drawBox(PRELIGHTCOLOUR);
	this->inWindow=true;
	return(true);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseExit(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->LFSTK_clearWindow();
	this->drawBox(NORMALCOLOUR);
	this->inWindow=false;
	return(true);
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseDown(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->LFSTK_clearWindow();
	this->drawBox(ACTIVECOLOUR);
	
	if(this->callback.pressCallback!=NULL)
		return(this->callback.pressCallback(this,this->callback.userData));
	return(true);
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseUp(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	if(this->inWindow==false)
		this->LFSTK_clearWindow();
	else
		{
			this->toggleState=!this->toggleState;
			this->mouseEnter(e);
			if(this->callback.releaseCallback!=NULL)
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
}
