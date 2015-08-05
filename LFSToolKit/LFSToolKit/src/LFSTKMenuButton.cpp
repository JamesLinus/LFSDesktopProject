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
	for(int j=0;j<MAXCOLOURS;j++)
		this->menuItemColours[j].name=NULL;

	this->menuItemFontString=NULL;

	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setMenuItemColours(j,this->colourNames[j].name);

	this->LFSTK_setFontString(this->wc->fontString);
	if(globalColoursSet==false)
		{
			this->LFSTK_setMenuItemFontString(this->wc->fontString);
			for(int j=0;j<MAXFONTCOLS;j++)
				this->LFSTK_setMenuItemsFontColourName(j,fontColourNames[j]);
		}
	else
		{
			this->LFSTK_setMenuItemFontString(gMenuItemFontString);
			for(int j=0;j<MAXFONTCOLS;j++)
				this->LFSTK_setMenuItemsFontColourName(j,gMenuItemFontColourNames[j]);
		}
	
}

void LFSTK_menuButtonClass::LFSTK_setMenuItemsFontColourName(int p,char* colour)
{
	this->menuItemFontColourNames[p]=strdup(colour);
}

LFSTK_menuButtonClass::~LFSTK_menuButtonClass()
{
	delete this->menus;
}

LFSTK_menuButtonClass::LFSTK_menuButtonClass()
{
}

void LFSTK_menuButtonClass::LFSTK_setMenuItemFontString(char *s)
{
	if(this->menuItemFontString!=NULL)
		free(this->menuItemFontString);
	this->menuItemFontString=strdup(s);
	this->menuItemfont=ftload(this->display,this->screen,s);
}

void LFSTK_menuButtonClass::LFSTK_setMenuItemColours(int p,char* colour)
{
	XColor tc,sc;
	if(this->menuItemColours[p].name!=NULL)
		free(this->menuItemColours[p].name);
	this->menuItemColours[p].name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->menuItemColours[p].pixel=sc.pixel;	
}

void LFSTK_menuButtonClass::drawLabel(int p)
{
	switch(this->labelOrientation)
		{
			case LEFT:
				drawUtf8String(this->wc,this->window,(XftFont*)(this->font->data),2,(this->h/2)+((this->wc->font->ascent-2)/2),this->fontColourNames[p],this->label);
				break;
			case CENTRE:
				drawUtf8String(this->wc,this->window,(XftFont*)(this->font->data),(this->w/2)-(ftTextWidth_Utf8(this->wc,this->label)/2),(this->h/2)+((this->wc->font->ascent-2)/2),this->fontColourNames[p],this->label);
				break;
		}
}

void LFSTK_menuButtonClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->colourNames[NORMALCOLOUR].pixel);
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
	this->drawLabel(FONTNORMALCOL);
}

void LFSTK_menuButtonClass::LFSTK_setColoursFromGlobals(void)
{
	if(globalColoursSet==true)
		{
			for(int j=0;j<MAXCOLOURS;j++)
				{
					this->LFSTK_setColourName(j,globalButtonColours[j].name);
					this->LFSTK_setMenuItemColours(j,globalMenuItemColours[j].name);
				}
			for(int j=0;j<MAXFONTCOLS;j++)
				this->LFSTK_setMenuItemsFontColourName(j,menuItemFontColourNames[j]);
		}
}

void LFSTK_menuButtonClass::mouseDown()
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

	this->drawLabel(FONTACTIVECOL);

	for(int j=0;j<this->menuCount;j++)
		{
			testwid=getTextwidth(this->display,(XftFont*)(this->menuItemfont->data),this->menus[j].label);
			if(testwid>maxwid)
				maxwid=testwid;
		}

	addto=this->menuItemfont->ascent+this->menuItemfont->descent+8;
	maxwid+=4;
	g=this->wc->LFSTK_getGeom();
	subwc=new LFSTK_windowClass(this->x+g->x,this->y+g->y+this->h,maxwid,this->menuCount*addto,true);
	sy=0;

	for(int j=0;j<MAXCOLOURS;j++)
		subwc->LFSTK_setColourName(j,menuItemColours[j].name);
	for(int j=0;j<MAXFONTCOLS;j++)
		subwc->LFSTK_setFontColourName(j,menuItemFontColourNames[j]);
	subwc->LFSTK_setFontString(this->menuItemFontString);

	for(int j=0;j<this->menuCount;j++)
		{
			bc=new LFSTK_buttonClass(subwc,this->menus[j].label,0,sy,maxwid,addto,0);
			this->menus[j].bc=bc;
			bc->LFSTK_setLabelOriention(LEFT);
			bc->LFSTK_setCallBack(NULL,this->callback.releaseCallback,this->menus[j].userData);
			bc->LFSTK_setStyle(FLATBUTTON);
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
}
	
void LFSTK_menuButtonClass::mouseUp()
{
	if(this->inWindow==false)
		this->LFSTK_clearWindow();
	else
		{
			this->mouseEnter();
			if((this->callback.releaseCallback!=NULL) && ((this->callback.ignoreCallback==false)))
				this->callback.releaseCallback(this,this->callback.userData);
		}
}

void LFSTK_menuButtonClass::mouseExit()
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
}

void LFSTK_menuButtonClass::mouseEnter()
{
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
	this->drawLabel(FONTHILITECOL);
}

void LFSTK_menuButtonClass::LFSTK_addMenus(menuItemStruct* menus,int cnt)
{
	this->menus=menus;
	this->menuCount=cnt;
}

LFSTK_menuButtonClass::LFSTK_menuButtonClass(LFSTK_windowClass* parentwc,char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;
	XGCValues	gcv;

	this->wc=parentwc;
	this->display=this->wc->display;
	this->parent=this->wc->window;

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;

	this->screen=this->wc->screen;
	this->visual=this->wc->visual;
	this->rootWindow=this->wc->rootWindow;
	this->cm=this->wc->cm;

	this->label=strdup(label);

	wa.bit_gravity=NorthWestGravity;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);
	XSelectInput(this->display,this->window,SubstructureRedirectMask|Button1MotionMask|ButtonReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	//this->initButton();
	this->initMenuButton();
 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);

	this->gc=this->wc->gc;

	this->listen.function=gadgetEvent;
	this->listen.pointer=this;
	this->listen.type=MENUBUTTONGADGET;

	this->style=EMBOSSEDBUTTON;
	this->LFSTK_setLabelOriention(1000);
	this->LFSTK_setCallBack(NULL,NULL,(void*)-1);
	wc->LFSTK_setListener(this->window,this->LFSTK_getListen());
}