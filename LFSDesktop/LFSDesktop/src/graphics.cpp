/******************************************************
*
*     ©keithhedger Tue  5 May 14:40:11 BST 2015
*     kdhedger68713@gmail.com
*
*     graphics.cpp
* 
******************************************************/

#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <search.h>

#include "prefs.h"
#include "files.h"
#include "graphics.h"
#include <cairo.h>

cairo_t	*cr;

void drawImage(char *type,char *label,int x,int y,bool mounted)
{
	ENTRY	testentry={NULL,NULL};
	ENTRY	*retentry=NULL;
	char	*tstr;
	int		w,h;
	double	scale;

	testentry.key=(char*)type;
	testentry.data=NULL;
	retentry=hsearch(testentry,ENTER);
	if(retentry->data==NULL)
		{
			retentry->data=malloc(sizeof(diskIconStruct));
			tstr=pathToIcon(type);
			makeImage((char*)tstr,(char*)type,(diskIconStruct*)(retentry->data));
			free(tstr);
		}

	if(mounted==true)
		{
			w=cairo_image_surface_get_width(((diskIconStruct*)retentry->data)->cairoImage);
			h=cairo_image_surface_get_height(((diskIconStruct*)retentry->data)->cairoImage);
			scale=(double)iconSize/w;
			cairo_save(cr);
				cairo_translate(cr,x,y);
				cairo_scale (cr,scale,scale);
				cairo_set_source_surface(cr,((diskIconStruct*)retentry->data)->cairoImage,0,0);
				cairo_paint(cr);
			cairo_restore(cr);
		}
	else
		{
			w=cairo_image_surface_get_width(((diskIconStruct*)retentry->data)->cairoImage);
			h=cairo_image_surface_get_height(((diskIconStruct*)retentry->data)->cairoImage);
			scale=(double)iconSize/w;
			cairo_save(cr);
				cairo_translate(cr,x,y);
				cairo_scale (cr,scale,scale);
				cairo_set_source_surface(cr,((diskIconStruct*)retentry->data)->cairoImage,0,0);
				cairo_paint_with_alpha(cr,0.5);
			cairo_restore(cr);
		}
}

void drawImageXXXX(char *type,char *label,int x,int y,bool mounted)
{
	ENTRY	testentry={NULL,NULL};
	ENTRY	*retentry=NULL;
	char	*tstr;

	testentry.key=(char*)type;
	testentry.data=NULL;
	retentry=hsearch(testentry,ENTER);
	if(retentry->data==NULL)
		{
			retentry->data=malloc(sizeof(diskIconStruct));
			tstr=pathToIcon(type);
			makeImage((char*)tstr,(char*)type,(diskIconStruct*)(retentry->data));
			free(tstr);
		}

	if(mounted==true)
		{
imlib_context_set_mask_alpha_threshold(15);	
					XSetClipMask(display,((diskIconStruct*)retentry->data)->gc,((diskIconStruct*)retentry->data)->mask);
					XSetClipOrigin(display,((diskIconStruct*)retentry->data)->gc,x,y);
					XCopyArea(display,((diskIconStruct*)retentry->data)->pixmap,drawOnThis,((diskIconStruct*)retentry->data)->gc,0,0,iconSize,iconSize,x,y);
/*
					imlib_context_set_image(((diskIconStruct*)retentry->data)->image);
					imlib_context_set_colormap(cm);
					imlib_context_set_anti_alias(1);
					imlib_image_set_has_alpha(1);
					imlib_context_set_blend(1);
					imlib_context_set_dither(1);
					imlib_context_set_mask(((diskIconStruct*)retentry->data)->mask);
					imlib_context_set_cliprect(x, y,iconSize,iconSize);
					imlib_context_set_display(display);
			//w=imlib_image_get_width();
		//	h=imlib_image_get_height();
					imlib_context_set_visual(visual);
					imlib_context_set_drawable(drawOnThis);

//XSetBackground(display, gc,whiteColor);
imlib_context_set_mask_alpha_threshold(0);
//XImage *xim=XGetImage(display,((diskIconStruct*)retentry->data)->pixmap,0, 0,iconSize, iconSize, AllPlanes,ZPixmap);
//XPutImage(display,drawOnThis,  gc, xim, 0, 0,x, y, iconSize, iconSize);
					imlib_render_image_on_drawable_at_size(x,y,iconSize,iconSize);
*/
			}
		else
			{

					XSetClipMask(display,gc,((diskIconStruct*)retentry->data)->maskOffline);
					XSetClipOrigin(display,gc,x,y);
					XCopyArea(display,((diskIconStruct*)retentry->data)->pixmapOffline,drawOnThis,gc,0,0,iconSize,iconSize,x,y);
			}
}