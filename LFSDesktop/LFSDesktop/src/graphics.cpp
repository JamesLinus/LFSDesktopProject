/******************************************************
*
*     ©keithhedger Tue  5 May 14:40:11 BST 2015
*     kdhedger68713@gmail.com
*
*     graphics.cpp
* 
******************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <search.h>

#include <cairo.h>

#include "prefs.h"
#include "files.h"
#include "graphics.h"

cairo_t	*cr;

void drawImage(char *type,char *label,int x,int y,bool mounted)
{
	ENTRY	testentry={NULL,NULL};
	ENTRY	*retentry=NULL;
	char	*tstr;
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

	scale=((diskIconStruct*)retentry->data)->scale;

	if(mounted==true)
		{
			cairo_save(cr);
				cairo_translate(cr,x,y);
				cairo_scale (cr,scale,scale);
				cairo_set_source_surface(cr,((diskIconStruct*)retentry->data)->cairoImage,0,0);
				cairo_paint(cr);
			cairo_restore(cr);
		}
	else
		{
			cairo_save(cr);
				cairo_translate(cr,x,y);
				cairo_scale (cr,scale,scale);
				cairo_set_source_surface(cr,((diskIconStruct*)retentry->data)->cairoImage,0,0);
				cairo_paint_with_alpha(cr,0.5);
			cairo_restore(cr);
		}
}

