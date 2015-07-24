/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:54:59 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKButton.h) is part of LFSToolKit.

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


#ifndef _LFSTKBUTTON_
#define _LFSTKBUTTON_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

class LFSTK_buttonClass
{
	public:
		LFSTK_buttonClass();
		~LFSTK_buttonClass();
		LFSTK_buttonClass(Display* dsp,Window parent,int x,int y,int w,int h,int gravity,char* foreground,char* background);
		void LFSTK_ClearWindow(void);
		//void LFSTK_setlistener(Window w,const struct listener *l);

		Display		*display;
		Window		window;
		GC			gc;
		int			screen;
		Visual		*visual;
		Window		rootWindow;		
		Colormap	cm;
	private:
		int			x;
		int			y;
		int			w;
		int			h;
		int			blackColour;
		int			whiteColour;
		XContext	listeners;
};

#endif
