/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:55:02 BST 2015 kdhedger68713@gmail.com

 * This file (graphics.cpp) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/shape.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <search.h>
#include <mntent.h>

#include <cairo.h>

#include "globals.h"
#include "prefs.h"
#include "files.h"
#include "disks.h"
#include "graphics.h"

cairo_t				*cr;
char				*fontName;
cairo_font_weight_t weight;
cairo_font_slant_t	slant;
int					fontSize;
cairoColor			backColour;
cairoColor			foreColour;

void debugDesk(void)
{
	for(int yyy=0;yyy<yCnt;yyy++)
		{
			for(int xxx=0;xxx<xCnt;xxx++)
				{
					cairo_save(cr);
						cairo_translate(cr,xxx*gridSize+gridBorder,yyy*gridSize+gridBorder);
						cairo_rectangle (cr, 0, 0,32, 32);
						if(xySlot[xxx][yyy]==0)
							cairo_set_source_rgba (cr, 0, 1, 0, 0.25);
						else
							cairo_set_source_rgba (cr, 1, 0, 0, 0.25);
						cairo_fill (cr);
					cairo_restore(cr);
				}
		}

//	for(int j=0;j<deskIconsCnt;j++)
//		{
//			printf("%s\n",deskIconsArray[j].label);
//		}
}

void drawImage(char *type,const char *catagory,int x,int y,bool mounted)
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
			if(strcmp(catagory,"customicon")==0)
				tstr=strdup(type);
			else
				tstr=pathToIcon(type,catagory);
			makeImage((char*)tstr,(diskIconStruct*)(retentry->data));
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

void drawIcons(void)
{
	FILE					*tp;
	XRectangle				rect;
	int						diskx,disky;
	bool					mounted=false;
	struct mntent			*entry;
	bool					loop;
	char					*tlable;
	char					*dot=NULL;
	cairo_text_extents_t	extents;
	int						boxheight;

	XDestroyRegion(rg);
	rg=XCreateRegion();

	cairo_save(cr);
		cairo_select_font_face(cr,fontName,slant,weight);
		cairo_set_font_size(cr,fontSize);
		cairo_text_extents(cr,"qI_^`",&extents);
	cairo_restore(cr);
	boxheight=extents.height+4;

	for(int j=0;j<deskIconsCnt;j++)
		{
			if((deskIconsArray[j].installed==false) || (deskIconsArray[j].ignore==true))
				continue;
			if(deskIconsArray[j].file==false)
				{
					tp=setmntent("/proc/mounts","r");
					mounted=false;
					loop=true;
					entry=getmntent(tp);
					while((entry!=NULL) && (loop=true))
						{
							if(strcasecmp(deskIconsArray[j].dev,entry->mnt_fsname)==0)
								{
									mounted=true;
									loop=false;
								}
							entry=getmntent(tp);
						}
					endmntent(tp);
				}
			else
				mounted=true;

			diskx=deskIconsArray[j].x*gridSize+gridBorder;
			disky=deskIconsArray[j].y*gridSize+gridBorder;
			if(deskIconsArray[j].file==false)
				{
					if(deskIconsArray[j].icon!=NULL)
						drawImage(deskIconsArray[j].icon,"customicon",diskx,disky,mounted);						
					else
						drawImage((char*)iconDiskType[deskIconsArray[j].iconhint],"devices",diskx,disky,mounted);
				}
			else
				{
					if(j<RESERVED)
						{
							if(strcmp(deskIconsArray[j].label,"Home")==0)
								drawImage((char*)iconDiskType[HOME],"",diskx,disky,true);
							else
								drawImage((char*)iconDiskType[deskIconsArray[j].iconhint],"",diskx,disky,true);
						}
					else
						{
							if(deskIconsArray[j].icon!=NULL)
								{
									drawImage(deskIconsArray[j].icon,"customicon",diskx,disky,mounted);
								}
							else
								{
									if(strstr(deskIconsArray[j].mime,"inode"))
										drawImage(deskIconsArray[j].mime,"places",diskx,disky,true);
									else
										drawImage(deskIconsArray[j].mime,"mimetypes",diskx,disky,true);
								}
						}
				}

			rect.x=diskx;
			rect.y=disky;
			rect.width=iconSize;
			rect.height=iconSize;
			XUnionRectWithRegion(&rect,rg, rg);

			XSetClipMask(display,gc,0);
	
			tlable=strdup(deskIconsArray[j].label);
			dot=NULL;
			if(showSuffix==false)
				{
					dot=strrchr(tlable,'.');
					if(dot!=NULL)
						*dot=0;
				}

			cairo_save(cr);
				cairo_select_font_face(cr,fontName,slant,weight);
				cairo_set_font_size(cr,fontSize);
				cairo_text_extents(cr,tlable, &extents);
				rect.x=diskx+(iconSize/2)-(extents.x_advance/2)-2;
				rect.y=disky+iconSize;
				rect.width=extents.x_advance+4;
				rect.height=boxheight;
//paint rect background
				cairo_rectangle(cr,rect.x,rect.y,rect.width,rect.height);
				cairo_set_source_rgba(cr,backColour.r,backColour.g,backColour.b,backColour.a);
				cairo_fill(cr);

//paint text
				cairo_move_to(cr,rect.x+2,rect.y+(rect.height/2)-(extents.y_bearing/2)-1);
				cairo_set_source_rgba(cr,foreColour.r,foreColour.g,foreColour.b,foreColour.a);
				cairo_show_text(cr,tlable); 
			cairo_restore(cr);

			free(tlable);
		}

	if(debugDeskFlag==true)
		debugDesk();


	XShapeCombineRegion(display,rootWin,ShapeInput,0,0,rg,ShapeSet);
}

int get_argb_visual(Visual** vis,int *depth)
{
	/* code from gtk project,gdk_screen_get_rgba_visual */
	XVisualInfo visual_template;
	XVisualInfo *visual_list=NULL;
	int nxvisuals=0,i;
	visual_template.screen=screen;
	visual_list=XGetVisualInfo (display,0,&visual_template,&nxvisuals);

	for (i=0; i < nxvisuals; i++)
		{
			if (visual_list[i].depth==32 &&
			        (visual_list[i].red_mask==0xff0000 &&
			         visual_list[i].green_mask==0x00ff00 &&
			         visual_list[i].blue_mask==0x0000ff ))
				{
					*vis=visual_list[i].visual;
					*depth=visual_list[i].depth;
					XFree(visual_list);
					return 0;
				}
		}
	// no argb visual available
	printf("no rgb\n");
	XFree(visual_list);
	return 1;
}

void createDesktopWindow(void)
{
	int	rc=0;
	Atom	xa;
	Atom	xa_prop[10];
	Hints	hints;

	rc=get_argb_visual(&visual,&depth);
	if(rc==0)
		{
			XSetWindowAttributes attr;
			attr.colormap=XCreateColormap(display,DefaultRootWindow(display),visual,AllocNone);
			attr.border_pixel=0;
			attr.background_pixel=0;

			rootWin=XCreateWindow(display,DefaultRootWindow(display),0,0,displayWidth,displayHeight,0,depth,InputOutput,visual,CWColormap | CWBorderPixel | CWBackPixel ,&attr);

			xa=XInternAtom(display,"_NET_WM_WINDOW_TYPE",False);
			xa_prop[0]=XInternAtom(display,"_NET_WM_WINDOW_TYPE_DESKTOP",False);
			if(xa!=None)
				XChangeProperty(display,rootWin,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,1);

			xa=XInternAtom(display,"_NET_WM_STATE",False);
			xa_prop[0]=XInternAtom(display,"_NET_WM_STATE_SKIP_PAGER",False);
			xa_prop[1]=XInternAtom(display,"_NET_WM_STATE_SKIP_TASKBAR",False);
			xa_prop[2]=XInternAtom(display,"_NET_WM_STATE_BELOW",False);
			xa_prop[3]=XInternAtom(display,"_NET_WM_STATE_STICKY",False);
			if(xa!=None)
				XChangeProperty(display,rootWin,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,4);

			xa=XInternAtom(display,"_NET_WM_ALLOWED_ACTIONS",False);
			xa_prop[0]=XInternAtom(display,"_NET_WM_STATE_STICKY",False);
			xa_prop[1]=XInternAtom(display,"_NET_WM_STATE_BELOW",False);
			xa_prop[2]=XInternAtom(display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);
	
			if(xa!=None)
				XChangeProperty(display,rootWin,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,3);

			hints.flags=2;
			hints.decorations=0;
			hints.functions=0;
			hints.inputMode=0;
			hints.status=0;
			xa_prop[9]=XInternAtom(display,"_MOTIF_WM_HINTS",True);
			XChangeProperty(display,rootWin,xa_prop[9],xa_prop[9],32,PropModeReplace,(unsigned char *)&hints,5);

			rg=XCreateRegion();
			XMapWindow(display,rootWin);
			XSync(display,False);

			XMoveWindow(display,rootWin,0,0);
			XLowerWindow(display,rootWin);
	
			buffer=XdbeAllocateBackBufferName(display,rootWin,XdbeBackground);
			swapInfo.swap_window=rootWin;
			swapInfo.swap_action=XdbeBackground;
			XdbeSwapBuffers(display,&swapInfo,1);
			drawOnThis=buffer;
		}
}
