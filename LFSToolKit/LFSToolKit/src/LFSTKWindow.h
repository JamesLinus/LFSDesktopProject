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
#include "LFSTKLib.h"

/**
 *
 * \brief Window class for LFSToolKit.
 *
 * Basic window class for LFSToolKit.
 *
 * \note The main window class MUST be passed to child widgets.
 *
 */

class LFSTK_windowClass
{
	public:
		LFSTK_windowClass();
		~LFSTK_windowClass();
		LFSTK_windowClass(int x,int y,int w,int h,const char* name,bool override);
		void LFSTK_clearWindow(void);
		unsigned long LFSTK_setColour(const char *name);
		void LFSTK_resizeWindow(int w,int h);

		void LFSTK_setListener(Window w,listener *l);
		listener *LFSTK_getListener(Window w);

		void LFSTK_setFontString(const char *s);
		void LFSTK_setFontColourName(int p,const char *colour);
		void LFSTK_setWindowColourName(int p,const char* colour);
		void LFSTK_setDecorated(bool isDecorated);
		void LFSTK_setSticky(bool set);
		void LFSTK_setKeepAbove(bool set);
		void LFSTK_setActive(bool set);
		bool LFSTK_getActive(void);

		geometryStruct	*LFSTK_getGeom(void);

		Display			*display;
		Window			window;
		GC				gc;
		int				screen;
		Visual			*visual;
		Window			rootWindow;		
		Colormap		cm;
		XContext		listeners;
		XftDraw 		*draw;

		char			*fontString;
		char			*fontColourNames[MAXCOLOURS];
		colourStruct	windowColourNames[MAXCOLOURS];

		LFSTK_lib		*globalLib;

		fontStruct		*font;

	private:
		void initWindow(void);
		void loadGlobalColours(void);
		int			x;
		int			y;
		int			w;
		int			h;
		char		*windowName;
		bool		isActive;
};

#endif
