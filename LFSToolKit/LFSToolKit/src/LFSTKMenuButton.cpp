/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:55:09 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKMenuButton.cpp) is part of LFSToolKit.

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

#include "LFSTKMenuButton.h"
#include "lib.h"

void LFSTK_menuButtonClass::initMenuButton(void)
{
}

LFSTK_menuButtonClass::~LFSTK_menuButtonClass()
{
	delete this->menus;
}

LFSTK_menuButtonClass::LFSTK_menuButtonClass()
{
}

/**
* Draw label.
* \param p Button state.
* \note State NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2.
*/
void LFSTK_menuButtonClass::drawLabel(int p)
{
	switch(this->labelOrientation)
		{
			case LEFT:
				drawUtf8String(this->wc,this->window,(XftFont*)(this->font->data),2,(this->h/2)+((this->wc->font->ascent-2)/2),this->fontColourNames[p],this->label);
				break;
			default://centre
				drawUtf8String(this->wc,this->window,(XftFont*)(this->font->data),(this->w/2)-(ftTextWidth_Utf8(this->wc,this->label)/2),(this->h/2)+((this->wc->font->ascent-2)/2),this->fontColourNames[p],this->label);
				break;
		}
}

void LFSTK_menuButtonClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	if(this->isActive==true)
		XSetForeground(this->display,this->gc,this->colourNames[NORMALCOLOUR].pixel);
	else
		XSetForeground(this->display,this->gc,this->colourNames[INACTIVECOLOUR].pixel);

	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	if(this->style==EMBOSSEDBUTTON)
		{
			XSetForeground(this->display,this->gc,this->whiteColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
			XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
			XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
		}
	this->drawLabel(NORMALCOLOUR);
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
* \note This shows the menu items previously loaded.
*/
bool LFSTK_menuButtonClass::mouseDown(XButtonEvent *e)
{
	LFSTK_buttonClass	*bc;
	geometryStruct		*g;
	LFSTK_windowClass	*subwc;
	int					maxwid=0;
	XEvent				event;
	bool				run=true;
	int					testwid=0;
	int					addto;
	int					sy;
	fontStruct			*tfont;
	const char			*itemfont;

	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[ACTIVECOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	if(this->style==EMBOSSEDBUTTON)
		{
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
			XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
			XSetForeground(this->display,this->gc,this->whiteColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
			XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
		}

	this->drawLabel(ACTIVECOLOUR);

	itemfont=this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT);
	tfont=ftload(this->display,this->screen,itemfont);
	for(int j=0;j<this->menuCount;j++)
		{
			testwid=getTextwidth(this->display,(XftFont*)(tfont->data),this->menus[j].label);
			if(testwid>maxwid)
				maxwid=testwid;
		}

	addto=tfont->ascent+tfont->descent+8;
	maxwid+=4;
	g=this->wc->LFSTK_getGeom();
	subwc=new LFSTK_windowClass(this->x+g->x,this->y+g->y+this->h,maxwid,this->menuCount*addto,"",true,false);
	sy=0;

	for(int j=0;j<this->menuCount;j++)
		{
			bc=new LFSTK_buttonClass(subwc,this->menus[j].label,0,sy,maxwid,addto,0);
			this->menus[j].bc=bc;
			bc->LFSTK_setLabelOriention(LEFT);
			bc->LFSTK_setCallBack(NULL,this->callback.releaseCallback,this->menus[j].userData);
			bc->LFSTK_setStyle(FLATBUTTON);
			bc->LFSTK_setFontString(itemfont);
			for(int j=0;j<MAXCOLOURS;j++)
				{
					bc->LFSTK_setColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEM));
					bc->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEMFONTCOLOUR));
				}
			sy+=addto;
		}
	XMapWindow(subwc->display,subwc->window);
	XMapSubwindows(subwc->display,subwc->window);

	while (run==true)
		{
			XNextEvent(subwc->display,&event);
			listener *l=subwc->LFSTK_getListener(event.xany.window);
			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			switch(event.type)
				{
				case LeaveNotify:
					if(event.xany.window==subwc->window)
						run=false;
					break;
				case Expose:
				break;
				default:
					break;
				}
		}
	for(int j=0;j<this->menuCount;j++)
		delete this->menus[j].bc;

	delete subwc;
	delete g;
	return(true);
}
	
/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuButtonClass::mouseUp(XButtonEvent *e)
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
			this->mouseEnter(e);
			if((this->callback.releaseCallback!=NULL) && ((this->callback.ignoreCallback==false)))
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuButtonClass::mouseExit(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->LFSTK_clearWindow();
	this->inWindow=false;
	return(true);
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuButtonClass::mouseEnter(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[PRELIGHTCOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	if(this->style==EMBOSSEDBUTTON)
		{
			XSetForeground(this->display,this->gc,this->whiteColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
			XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
			XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);
		}

	this->inWindow=true;
	this->drawLabel(PRELIGHTCOLOUR);
	return(true);
}

/**
* Add array of menu items to gadget.
* \param menus Pointer to array of menuItemStruct's.
* \param cnt Length of array.
* \note It is up to the caller to set the menuItemStruct and free it when finished.
*/
void LFSTK_menuButtonClass::LFSTK_addMenus(menuItemStruct* menus,int cnt)
{
	this->menus=menus;
	this->menuCount=cnt;
}

/**
* Main menu button constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_menuButtonClass::LFSTK_menuButtonClass(LFSTK_windowClass* parentwc,char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=NorthWestGravity;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,Button1MotionMask|ButtonReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->initMenuButton();

	this->style=EMBOSSEDBUTTON;
	this->LFSTK_setLabelOriention(CENTRE);

	this->listen.function=gadgetEvent;
	this->listen.pointer=this;
	this->listen.type=MENUBUTTONGADGET;
	wc->LFSTK_setListener(this->window,this->LFSTK_getListen());
}