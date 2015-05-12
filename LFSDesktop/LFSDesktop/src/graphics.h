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

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};

struct	diskIconStruct
	{
		double			scale;
		cairo_surface_t	*cairoImage;
	};

extern cairo_t	*cr;

void drawImage(char *type,char *label,int x,int y,bool mounted);

void createColours(void);
int get_argb_visual(Visual** vis,int *depth);
void createDesktopWindow(void);

#endif
