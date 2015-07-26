/*
 *
 * ©K. D. Hedger. Fri 24 Jul 20:38:28 BST 2015 kdhedger68713@gmail.com

 * This file (lib.h) is part of LFSToolKit.

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

#ifndef _LFSTKLIB_
#define _LFSTKLIB_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "LFSTKWindow.h"
#include "LFSTKGlobals.h"

#define DEFAULTFONT "sans-serif:size=10"

struct fontStruct
{
	int ascent;
	int descent;
	int size;
	void *data;
};

void gadgetEvent(void *self,XEvent *e);
fontStruct *ftload(LFSTK_windowClass* wc,const char *name);

#endif
