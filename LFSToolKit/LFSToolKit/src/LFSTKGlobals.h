/*
 *
 * ©K. D. Hedger. Fri 24 Jul 19:33:15 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKGlobals.h) is part of LFSToolKit.

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

#ifndef _LFSTKGLOBALS_
#define _LFSTKGLOBALS_

#include <X11/Xft/Xft.h>

#define DEFAULTFONT "sans-serif:size=10"

enum {FLATBUTTON=0,EMBOSSEDBUTTON};

struct buttonCB
{
	void	(*pressCallback)(void *,int);
	void	(*releaseCallback)(void *,int);
	int		userData;
};

struct fontColour
{
	XftColor color;
	XftDraw *draw;
	Visual *visual;
	Colormap colormap;
};

struct fontStruct
{
	int ascent;
	int descent;
	int size;
	void *data;
};

struct listener
{
	void	(*function)(void *,XEvent *);
	void	*pointer;
};

#endif