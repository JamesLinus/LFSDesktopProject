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

#define BUFFERSIZE 256

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

struct	cairoColor
{
	double	r,g,b,a;
};

extern cairo_t				*cr;
extern char					*fontName;
extern cairo_font_weight_t	weight;
extern cairo_font_slant_t	slant;
extern int					fontSize;
extern cairoColor			backColour;
extern cairoColor			foreColour;

void drawImage(char *type,const char *catagory,int x,int y,bool mounted);

void createColours(void);
int get_argb_visual(Visual** vis,int *depth);
void createDesktopWindow(void);
void drawIcons(void);

#endif
