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

#include "LFSTKMenuButton.h"

void LFSTK_menuButtonClass::initMenuButton(void)
{
}

LFSTK_menuButtonClass::~LFSTK_menuButtonClass()
{
}

LFSTK_menuButtonClass::LFSTK_menuButtonClass()
{
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_menuButtonClass::LFSTK_clearWindow()
{
	geometryStruct g={0,0,this->w,this->h};

	if(this->isActive==true)
		{
			this->drawBox(&g,NORMALCOLOUR,this->style);
			this->LFSTK_drawLabel(NORMALCOLOUR);
		}
	else
		{
			this->drawBox(&g,INACTIVECOLOUR,this->style);
			this->LFSTK_drawLabel(INACTIVECOLOUR);
		}
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
* \note This shows the menu items previously loaded.
*/
bool LFSTK_menuButtonClass::mouseDown(XButtonEvent *e)
{
	LFSTK_buttonClass		*bc;
	geometryStruct			*g;
	LFSTK_windowClass		*subwc;
	int						maxwid=0;
	XEvent					event;
	bool					run=true;
	int						testwid=0;
	int						addto;
	int						sy;
	fontStruct				*tfont;
	const char				*itemfont;
	LFSTK_menuButtonClass	*mb=NULL;

	geometryStruct	geom={0,0,this->w,this->h};
	bevelType		bv=BEVELIN;


	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->drawBox(&geom,ACTIVECOLOUR,this->getActiveBevel());
	this->LFSTK_drawLabel(ACTIVECOLOUR);

	itemfont=this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT);
	tfont=this->wc->globalLib->LFSTK_loadFont(this->display,this->screen,itemfont);
	for(int j=0;j<this->menuCount;j++)
		{
			testwid=this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(tfont->data),this->menus[j].label);
			if(testwid>maxwid)
				maxwid=testwid;
		}

	addto=tfont->ascent+tfont->descent+8;
	maxwid+=4;
	g=this->LFSTK_getGeom();
	subwc=new LFSTK_windowClass(g->x,g->y+this->h,maxwid,this->menuCount*addto,"",true,true);
	sy=0;

	for(int j=0;j<this->menuCount;j++)
		{
			if(this->menus[j].subMenus==NULL)
				{
					bc=new LFSTK_buttonClass(subwc,this->menus[j].label,0,sy,maxwid,addto,0);
					this->menus[j].bc=bc;
					bc->LFSTK_setLabelOriention(LEFT);
					bc->LFSTK_setCallBack(NULL,this->callback.releaseCallback,(void*)&(this->menus[j]));
					bc->LFSTK_setStyle(BEVELNONE);
					bc->LFSTK_setFontString(itemfont);
					bc->LFSTK_setLabelAutoColour(this->autoLabelColour);
					for(int j=0;j<MAXCOLOURS;j++)
						{
							bc->LFSTK_setColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEM));
							bc->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEMFONTCOLOUR));
						}
				}
			else
				{
					mb=new LFSTK_menuButtonClass(subwc,this->menus[j].label,0,sy,maxwid,addto,0);
					mb->LFSTK_setStyle(BEVELNONE);
					mb->LFSTK_setFontString(itemfont);
					mb->LFSTK_setLabelAutoColour(this->autoLabelColour);
					for(int k=0;k<MAXCOLOURS;k++)
						{
							mb->LFSTK_setColourName(k,this->wc->globalLib->LFSTK_getGlobalString(k,TYPEMENUITEM));
							mb->LFSTK_setFontColourName(k,this->wc->globalLib->LFSTK_getGlobalString(k,TYPEMENUITEMFONTCOLOUR));
						}
					this->menus[j].bc=static_cast<LFSTK_buttonClass*>(mb);
					mb->LFSTK_addMenus(this->menus[j].subMenus,this->menus[j].subMenuCnt);
					mb->LFSTK_setCallBack(NULL,this->callback.releaseCallback,(void*)&(this->menus[j]));
					mb->LFSTK_setIgnoreCB(true);
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
				case ButtonRelease:
					run=false;
					break;
				default:
					break;
				}
		}

	for(int j=0;j<this->menuCount;j++)
		{
			if(this->menus[j].bc!=NULL)
				delete this->menus[j].bc;
			this->menus[j].bc=NULL;
		}

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
	geometryStruct	g={0,0,this->w,this->h};
	bevelType		bv=BEVELIN;


	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->drawBox(&g,PRELIGHTCOLOUR,this->style);
	this->LFSTK_drawLabel(PRELIGHTCOLOUR);
}

/**
* Add array of menu items to gadget.
* \param menus Pointer to array of menuItemStruct's.
* \param cnt Length of array.
* \note It is up to the caller to set the menuItemStruct and free it and any data it contains when finished.
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
LFSTK_menuButtonClass::LFSTK_menuButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=NorthWestGravity;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,Button1MotionMask|ButtonReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->initMenuButton();

	this->style=BEVELOUT;
	this->LFSTK_setLabelOriention(CENTRE);

	this->listen.function=&(this->wc->globalLib->LFSTK_gadgetEvent);
	this->listen.pointer=this;
	this->listen.type=MENUBUTTONGADGET;
	wc->LFSTK_setListener(this->window,this->getListen());
}