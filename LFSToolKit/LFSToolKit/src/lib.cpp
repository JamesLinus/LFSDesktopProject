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
#include "LFSTKLineEdit.h"

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

bool loadVarsFromFile(const char* filepath,args* dataptr)
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
					sscanf(buffer,"%ms %ms",&argname,&strarg);
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
											sscanf(buffer,"%*s %m[^\n]s",(char**)dataptr[cnt].data);
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

bool gadgetEvent(void *self,XEvent *e,int type)
{
	bool				retval=true;
	LFSTK_gadgetClass	*gadget=NULL;

//printf("---%i---\n",type);
//printf("===type=%i====\n",e->type);
	gadget=static_cast<LFSTK_gadgetClass*>(self);

	switch (e->type)
		{
			case EnterNotify:
				retval=gadget->mouseEnter(&e->xbutton);
				break;
			case LeaveNotify:
				retval=gadget->mouseExit(&e->xbutton);
				break;
			case ButtonRelease:
				retval=gadget->mouseUp(&e->xbutton);
				break;
			case ButtonPress:
				XSetInputFocus(gadget->wc->display,e->xbutton.window,RevertToNone,CurrentTime);
				retval=gadget->mouseDown(&e->xbutton);
				break;

			case MotionNotify:
				break;
			case Expose:
//			 		printf("xpose from lib\n");
				gadget->LFSTK_clearWindow();
				break;

			case FocusIn:
				//printf("focus in libev\n");
				retval=gadget->gotFocus(e);
				break;
			case FocusOut:
				//printf("focus out libev\n");
				retval=gadget->lostFocus(e);
				break;

			case KeyRelease:
				retval=gadget->keyRelease(&e->xkey);
				//printf("KeyRelease\n");
				break;
	
			case ConfigureNotify:
//				printf("conf>>>>>>>>>\n");
				break;
			case GravityNotify:
//				printf("grav>>>>>>>>>>>\n");
//				retval=true;
				break;
			case ResizeRequest:
				//XCheckTypedWindowEvent
//				printf("resize\n");
				break;
			case ClientMessage:
//				printf("ClientMessage\n");
				break;
				
		}
//	printf("%i\n",ud);
	if(retval==false)
		XSendEvent(gadget->wc->display,gadget->wc->window,False,0L,e);
	return(retval);
}

fontStruct* ftload(Display *disp,int scr,const char *name)
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

int ftTextWidth_Utf8(LFSTK_windowClass *wc,char *s)
{
	XftFont *font=(XftFont*)wc->font->data;
	XGlyphInfo info;
	XftTextExtentsUtf8(wc->display,font,(XftChar8 *)s,strlen(s),&info);
	return info.width-info.x;
}

int getTextwidth(Display* disp,XftFont *font,const char *s)
{
	XGlyphInfo info;
	XftTextExtentsUtf8(disp,font,(XftChar8 *)s,strlen(s),&info);
	return info.width-info.x;
}

void drawUtf8String(LFSTK_windowClass *wc,Window d,XftFont* font,int x,int y,const char *col,const char *s)
{
	XftColor colour;

	if (!XftColorAllocName(wc->display,wc->visual,wc->cm,col,&colour))
		return;
	
	XftDrawChange(wc->draw,d);
	XftDrawStringUtf8(wc->draw,&colour,font,x,y,(XftChar8 *)s,strlen(s));
}
