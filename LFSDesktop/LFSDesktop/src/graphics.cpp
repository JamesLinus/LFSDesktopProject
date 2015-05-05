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

void drawImage(char *type,char *label,int x,int y,bool mounted)
{
	if(mounted==true)
		{
					//					printf(">>%s >>%s<<<<\n",dtype,dname);
			if(strcasecmp(type,"sata")==0)
				{
					XSetClipMask(display,gc,diskPixmapMask);
					XSetClipOrigin(display,gc,x,y);
					XCopyArea(display,diskPixmap,drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
				}
			if(strcasecmp(type,"usb")==0)
				{
					XSetClipMask(display,gc,usbPixmapMask);
					XSetClipOrigin(display,gc,x,y);
					XCopyArea(display,usbPixmap,drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
				}
			if(strcasecmp(type,"cdrom")==0)
				{
					XSetClipMask(display,gc,cdromPixmapMask);
					XSetClipOrigin(display,gc,x,y);
					XCopyArea(display,cdromPixmap,drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
				}
			}
		else
			{
				if(strcasecmp(type,"sata")==0)
					{
						XSetClipMask(display,gc,diskPixmapMask);
						XSetClipOrigin(display,gc,x,y);
						XCopyArea(display,diskPixmapOffline,drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
					}
				if(strcasecmp(type,"usb")==0)
					{
						XSetClipMask(display,gc,usbPixmapMask);
						XSetClipOrigin(display,gc,x,y);
						XCopyArea(display,usbPixmapOffline,drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
					}
				if(strcasecmp(type,"cdrom")==0)
					{
						XSetClipMask(display,gc,cdromPixmapMask);
						XSetClipOrigin(display,gc,x,y);
						XCopyArea(display,cdromPixmapOffline,drawOnThis,gc,0,0,ICONSIZE,ICONSIZE,x,y);
					}
			}
}