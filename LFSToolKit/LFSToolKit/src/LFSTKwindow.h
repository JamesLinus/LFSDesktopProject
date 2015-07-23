/*
 *
 * ©K. D. Hedger. Thu 23 Jul 16:15:33 BST 2015 kdhedger68713@gmail.com

 * This file (window.h) is part of LFSToolKit.

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

#ifndef _WINDOW_
#define _WINDOW_

class LFSTK_windowClass
{
	public:
		LFSTK_windowClass(int x,int y,int w,int h);
		LFSTK_windowClass();
		~LFSTK_windowClass();

	private:
		Display	*display;
		int		screen;
		Visual	*visual;
		Window	rootWindow;
		Window	window;
		GC		gc;
		
		int		x;
		int		y;
		int		w;
		int		h;
};

//LFSTK_windowClass::~LFSTK_windowClass();
//LFSTK_windowClass::LFSTK_windowClass();

#endif
