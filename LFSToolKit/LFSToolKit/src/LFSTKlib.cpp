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

#include "LFSTKWindow.h"
#include "LFSTKlib.h"
#include "LFSTKButton.h"

void gadgetEvent(void *self,XEvent *e)
{
	int ud=-1;
	ud=reinterpret_cast<LFSTK_buttonClass*>(self)->listen.userData;

	switch (e->type)
		{
		case EnterNotify:
			printf("enter - ");
			if(ud!=-1)
				reinterpret_cast<LFSTK_buttonClass*>(self)->mouseEnter();
			break;
		case LeaveNotify:
			printf("leave - ");
			if(ud!=-1)
				reinterpret_cast<LFSTK_buttonClass*>(self)->mouseExit();
			break;
		case ButtonRelease:
			printf("release - ");
			if(ud!=-1)
				reinterpret_cast<LFSTK_buttonClass*>(self)->mouseUp();
			break;
		case MotionNotify:
			printf("move - ");
			break;
		case ButtonPress:
			printf("press - ");
			if(ud!=-1)
				reinterpret_cast<LFSTK_buttonClass*>(self)->mouseDown();
			break;
		}
	printf("%i\n",ud);
}

fontStruct *ftload(LFSTK_windowClass* wc,const char *name)
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

