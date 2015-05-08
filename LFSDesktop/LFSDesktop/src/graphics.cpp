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

#include "prefs.h"
#include "files.h"

/*
Pixmap	diskPixmap;
Pixmap	diskPixmapMask;
Pixmap	diskPixmapOffline;
Pixmap	diskPixmapMaskOffline;

Pixmap	usbPixmap;
Pixmap	usbPixmapMask;
Pixmap	usbPixmapOffline;
Pixmap	usbPixmapMaskOffline;

Pixmap	cdromPixmap;
Pixmap	cdromPixmapMask;
Pixmap	cdromPixmapOffline;
Pixmap	cdromPixmapMaskOffline;
*/

void drawImage(char *type,char *label,int x,int y,bool mounted)
{
	if(mounted==true)
		{
			if(strcasecmp(type,"sata")==0)
				{
					XSetClipMask(display,gc,diskIconsPixmapMask[SATA][0]);
					XSetClipOrigin(display,gc,x,y);
					XCopyArea(display,diskIconsPixmap[SATA][0],drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
				}
			if(strcasecmp(type,"usb")==0)
				{
					XSetClipMask(display,gc,diskIconsPixmapMask[USB][0]);
					XSetClipOrigin(display,gc,x,y);
					XCopyArea(display,diskIconsPixmap[USB][0],drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
				}
			if(strcasecmp(type,"cdrom")==0)
				{
					XSetClipMask(display,gc,diskIconsPixmapMask[CDROM][0]);
					XSetClipOrigin(display,gc,x,y);
					XCopyArea(display,diskIconsPixmap[CDROM][0],drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
				}
			}
		else
			{
				if(strcasecmp(type,"sata")==0)
					{
						XSetClipMask(display,gc,diskIconsPixmapMask[SATA][1]);
						XSetClipOrigin(display,gc,x,y);
						XCopyArea(display,diskIconsPixmap[SATA][1],drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
					}
				if(strcasecmp(type,"usb")==0)
					{
						XSetClipMask(display,gc,diskIconsPixmapMask[USB][1]);
						XSetClipOrigin(display,gc,x,y);
						XCopyArea(display,diskIconsPixmap[USB][1],drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
					}
				if(strcasecmp(type,"cdrom")==0)
					{
						XSetClipMask(display,gc,diskIconsPixmapMask[CDROM][1]);
						XSetClipOrigin(display,gc,x,y);
						XCopyArea(display,diskIconsPixmap[CDROM][1],drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
					}
			}
}