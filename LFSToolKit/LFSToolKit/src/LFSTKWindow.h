/*
 *
 * ©K. D. Hedger. Thu 23 Jul 16:15:33 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKwindow.h) is part of LFSToolKit.

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

#ifndef _LFSTKWINDOW_
#define _LFSTKWINDOW_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "LFSTKGlobals.h"
#include "LFSTKlib.h"

class LFSTK_windowClass
{
	public:
		LFSTK_windowClass();
		~LFSTK_windowClass();
		LFSTK_windowClass(int x,int y,int w,int h,char* foreground,char* background);
		void LFSTK_clearWindow(void);
		void LFSTK_setlistener(Window w,const struct listener *l);
		unsigned long LFSTK_setColour(const char *name);
		void LFSTK_resizeWindow(int w,int h);
//		void LFSTK_gadgetEvent(void *self,XEvent *e);
//		void* LFSTK_returnGE(void);

		Display		*display;
		Window		window;
		GC			gc;
		int			screen;
		Visual		*visual;
		Window		rootWindow;		
		Colormap	cm;
		XContext	listeners;
		struct listener listen;

	private:
		int			x;
		int			y;
		int			w;
		int			h;
		int			foreColour;
		int			backColour;
		int			blackColour;
		int			whiteColour;
};

#endif
