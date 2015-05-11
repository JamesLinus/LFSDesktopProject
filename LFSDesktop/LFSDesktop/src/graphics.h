/******************************************************
*
*     ©keithhedger Tue  5 May 14:40:11 BST 2015
*     kdhedger68713@gmail.com
*
*     graphics.h
* 
******************************************************/

#ifndef _GRAPHICS_
#define _GRAPHICS_

#include <Imlib2.h>
#include <cairo.h>

struct	diskIconStruct
	{
		Pixmap	pixmap;
		Pixmap	mask;
		Pixmap	pixmapOffline;
		Pixmap	maskOffline;
		Imlib_Image	image;
		GC		gc;
		cairo_surface_t	*cairoImage;
	};

extern cairo_t	*cr;

void drawImage(char *type,char *label,int x,int y,bool mounted);

#endif
