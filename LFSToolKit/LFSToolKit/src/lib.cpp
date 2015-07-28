/*
 *
 * ©K. D. Hedger. Fri 24 Jul 20:38:19 BST 2015 kdhedger68713@gmail.com

 * This file (lib.cpp) is part of LFSToolKit.

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
#include <stdio.h>
#include <X11/Xft/Xft.h>

#include "LFSTKButton.h"
#include "LFSTKWindow.h"
#include "lib.h"

void gadgetEvent(void *self,XEvent *e)
{
	int ud=reinterpret_cast<LFSTK_buttonClass*>(self)->LFSTK_getCallbackUD();

	switch (e->type)
		{
		case EnterNotify:
			printf("enter - ");
			reinterpret_cast<LFSTK_buttonClass*>(self)->mouseEnter();
			break;
		case LeaveNotify:
			printf("leave - ");
			reinterpret_cast<LFSTK_buttonClass*>(self)->mouseExit();
			break;
		case ButtonRelease:
			printf("release - ");
			reinterpret_cast<LFSTK_buttonClass*>(self)->mouseUp();
			break;
		case MotionNotify:
			printf("move - ");
			break;
		case ButtonPress:
			printf("press - ");
			reinterpret_cast<LFSTK_buttonClass*>(self)->mouseDown();
			break;
		case Expose:
			reinterpret_cast<LFSTK_buttonClass*>(self)->LFSTK_clearWindow();
			break;
		}
	printf("%i\n",ud);
}

fontStruct* ftload(LFSTK_windowClass* wc,const char *name)
{
	XftFont  *font=NULL;

	if (name != NULL)
		{
			font=XftFontOpenXlfd(wc->display,wc->screen,name);
			if (font==NULL)
				font=XftFontOpenName(wc->display,wc->screen,name);
			if (font==NULL)
				fprintf(stderr,"cannot not load font %s",name);
		}

	if (font==NULL)
		font=XftFontOpenName(wc->display,wc->screen,DEFAULTFONT);

	if (font==NULL)
		return NULL;

	fontStruct *f=(fontStruct*)malloc(sizeof(fontStruct));
	f->size=font->ascent+font->descent;
	f->ascent=font->ascent;
	f->descent=font->descent;
	f->data=font;

	return f;
}

fontColour *ftLoadColour(LFSTK_windowClass *wc,const char *name)
{
	XftDraw *draw;
	XftColor colour;

	if ((draw=XftDrawCreate(wc->display,wc->rootWindow,wc->visual,wc->cm))==NULL)
		return NULL;

	if (!XftColorAllocName(wc->display,wc->visual,wc->cm,name,&colour))
		{
			XftDrawDestroy(draw);
			return NULL;
		}

	fontColour *c=(fontColour*)malloc(sizeof(fontColour));
	c->draw=draw;
	c->color=colour;
	c->visual=wc->visual;
	c->colormap=wc->cm;

	return c;
}

void ftDrawString_Utf8(LFSTK_windowClass *wc,Window d,int x,int y,char *s)
{
	XftFont *font=(XftFont*)wc->font->data;
	XftDrawChange(wc->fnormal->draw,d);
	XftDrawStringUtf8(wc->fnormal->draw,&wc->fnormal->color,font,x,y,(XftChar8 *)s,strlen(s));
}

int ftTextWidth_Utf8(LFSTK_windowClass *wc,char *s)
{
	XftFont *font=(XftFont*)wc->font->data;
	XGlyphInfo info;
	XftTextExtentsUtf8(wc->display,font,(XftChar8 *)s,strlen(s),&info);
	return info.width-info.x;
}
