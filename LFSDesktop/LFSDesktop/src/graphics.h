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

#include <cairo.h>

struct	diskIconStruct
	{
		double			scale;
		cairo_surface_t	*cairoImage;
	};

extern cairo_t	*cr;

void drawImage(char *type,char *label,int x,int y,bool mounted);

#endif
