/*
 *
 * ©K. D. Hedger. Fri 24 Jul 20:38:19 BST 2015 kdhedger68713@gmail.com

 * This file (lib.cpp) is part of LFSToolKit.

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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xft/Xft.h>

#include "LFSTKButton.h"
#include "LFSTKWindow.h"
#include "LFSTKMenuButton.h"
#include "lib.h"

colourStruct	globalWindowColours[MAXCOLOURS];
colourStruct	globalButtonColours[MAXCOLOURS];
colourStruct	globalMenuItemColours[MAXCOLOURS];
bool			globalColoursSet=false;

char			*gFontString=NULL;
char			*gFontColourNames[MAXFONTCOLS];
char			*gMenuItemFontString;
char			*gMenuItemFontColourNames[MAXCOLOURS];

args			lfsToolKitGlobals[]=
{
//window
	{"window_normal",TYPESTRING,&globalWindowColours[NORMALCOLOUR]},
	{"window_prelight",TYPESTRING,&globalWindowColours[PRELIGHTCOLOUR]},
	{"window_active",TYPESTRING,&globalWindowColours[ACTIVECOLOUR]},
//button
	{"button_normal",TYPESTRING,&globalButtonColours[NORMALCOLOUR]},
	{"button_prelight",TYPESTRING,&globalButtonColours[PRELIGHTCOLOUR]},
	{"button_active",TYPESTRING,&globalButtonColours[ACTIVECOLOUR]},
//menu button
	{"menuitem_normal",TYPESTRING,&globalMenuItemColours[NORMALCOLOUR]},
	{"menuitem_prelight",TYPESTRING,&globalMenuItemColours[PRELIGHTCOLOUR]},
	{"menuitem_active",TYPESTRING,&globalMenuItemColours[ACTIVECOLOUR]},
	{"menuitem_font",TYPESTRING,&gMenuItemFontString},
	{"menuitem_font_normal",TYPESTRING,&gMenuItemFontColourNames[NORMALCOLOUR]},
	{"menuitem_font_prelight",TYPESTRING,&gMenuItemFontColourNames[PRELIGHTCOLOUR]},
	{"menuitem_font_active",TYPESTRING,&gMenuItemFontColourNames[ACTIVECOLOUR]},

//font
	{"font",TYPESTRING,&gFontString},
	{"font_normal",TYPESTRING,&gFontColourNames[NORMALCOLOUR]},
	{"font_prelight",TYPESTRING,&gFontColourNames[PRELIGHTCOLOUR]},
	{"font_active",TYPESTRING,&gFontColourNames[ACTIVECOLOUR]},


	{NULL,0,NULL}
};

void saveVarsToFile(const char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	int		cnt=0;

	if(filepath[0]=='-')
		fd=stdout;
	else
		fd=fopen(filepath,"w");

	if(fd!=NULL)
		{
			while(dataptr[cnt].name!=NULL)
				{
					switch(dataptr[cnt].type)
						{
						case TYPEINT:
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
							break;
						case TYPESTRING:
							if(*(char**)(dataptr[cnt].data)!=NULL)
								fprintf(fd,"%s	%s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
							break;
						case TYPEBOOL:
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
							break;
						}
					cnt++;
				}
			fclose(fd);
		}
}

bool loadVarsFromFile(char* filepath,args* dataptr)
{
	FILE*	fd=NULL;
	char	buffer[2048];
	int		cnt;
	char*	argname=NULL;
	char*	strarg=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					sscanf(buffer,"%as %as",&argname,&strarg);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((strarg!=NULL) && (argname!=NULL) && (strcmp(argname,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
										case TYPEINT:
											*(int*)dataptr[cnt].data=atoi(strarg);
											break;
										case TYPESTRING:
											if(*(char**)(dataptr[cnt].data)!=NULL)
												free(*(char**)(dataptr[cnt].data));
											sscanf(buffer,"%*s %a[^\n]s",(char**)dataptr[cnt].data);
											break;
										case TYPEBOOL:
											*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
											break;
										}
								}
							cnt++;
						}
					if(argname!=NULL)
						free(argname);
					if(strarg!=NULL)
						free(strarg);
					argname=NULL;
					strarg=NULL;
				}
			fclose(fd);
			return(true);
		}
	return(false);
}
void gadgetEvent(void *self,XEvent *e,int type)
{

	LFSTK_buttonClass *bc;

	void *ud;

	bc=static_cast<LFSTK_buttonClass*>(self);
	ud=bc->LFSTK_getCallbackUD();

	switch (e->type)
		{
		case EnterNotify:
			//printf("enter - ");
			bc->mouseEnter();
			break;
		case LeaveNotify:
			//printf("leave - %i - %i\n",bc->parent,e->xany.window);
			bc->mouseExit();
		break;
		case ButtonRelease:
			bc->mouseUp();
			break;
		case MotionNotify:
//			printf("move - ");
			break;
		case ButtonPress:
//			printf("press - ");
			bc->mouseDown();
			break;
		case Expose:
			bc->LFSTK_clearWindow();
			break;
		}
//	printf("%i\n",ud);
}

fontStruct* ftload(Display *disp,int scr,char *name)
{
	XftFont  *font=NULL;
	if (name != NULL)
		{
			font=XftFontOpenXlfd(disp,scr,name);
			if (font==NULL)
				font=XftFontOpenName(disp,scr,name);
			if (font==NULL)
				fprintf(stderr,"cannot not load font %s",name);
		}

	if (font==NULL)
		font=XftFontOpenName(disp,scr,DEFAULTFONT);

	if (font==NULL)
		return NULL;

	fontStruct *f=(fontStruct*)malloc(sizeof(fontStruct));
	f->size=font->ascent+font->descent;
	f->ascent=font->ascent;
	f->descent=font->descent;
	f->data=font;

	return f;
}

/*
fontColour *ftLoadColour(LFSTK_windowClass *wc,const char *name)
{
	XftDraw *draw;
	XftColor colour;

	if ((draw=XftDrawCreate(wc->display,wc->rootWindow,wc->visual,wc->cm))==NULL)
		return NULL;

	if (!XftColorAllocName(wc->display,wc->visual,wc->cm,name,&colour))
		{
			XftDrawDestroy(draw);
			return NULL;
		}

	fontColour *c=(fontColour*)malloc(sizeof(fontColour));
	c->draw=draw;
	c->color=colour;
	c->visual=wc->visual;
	c->colormap=wc->cm;

	return c;
}
*/

int ftTextWidth_Utf8(LFSTK_windowClass *wc,char *s)
{
	XftFont *font=(XftFont*)wc->font->data;
	XGlyphInfo info;
	XftTextExtentsUtf8(wc->display,font,(XftChar8 *)s,strlen(s),&info);
	return info.width-info.x;
}

int getTextwidth(Display* disp,XftFont *font,char *s)
{
	XGlyphInfo info;
	XftTextExtentsUtf8(disp,font,(XftChar8 *)s,strlen(s),&info);
	return info.width-info.x;
}

void drawUtf8String(LFSTK_windowClass *wc,Window d,XftFont* font,int x,int y,char *col,char *s)
{
	//XftDraw *draw;
	XftColor colour;

	//if ((draw=XftDrawCreate(wc->display,wc->rootWindow,wc->visual,wc->cm))==NULL)
	//	return;

	if (!XftColorAllocName(wc->display,wc->visual,wc->cm,col,&colour))
		{
			//XftDrawDestroy(draw);
			return;
		}
	
	//XftDrawChange(draw,d);
	XftDrawChange(wc->draw,d);
//	XftDrawStringUtf8(draw,&colour,(XftFont*)wc->font->data,x,y,(XftChar8 *)s,strlen(s));
	XftDrawStringUtf8(wc->draw,&colour,font,x,y,(XftChar8 *)s,strlen(s));
//	XftDrawStringUtf8(draw,&wc->colourNames[0].pixel,font,x,y,(XftChar8 *)s,strlen(s));
//	XftDrawDestroy(draw);
}
