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

#include <X11/Xlib.h>

/*
extern Pixmap	diskPixmap;
extern Pixmap	diskPixmapMask;
extern Pixmap	diskPixmapOffline;
extern Pixmap	diskPixmapMaskOffline;

extern Pixmap	usbPixmap;
extern Pixmap	usbPixmapMask;
extern Pixmap	usbPixmapOffline;
extern Pixmap	usbPixmapMaskOffline;

extern Pixmap	cdromPixmap;
extern Pixmap	cdromPixmapMask;
extern Pixmap	cdromPixmapOffline;
extern Pixmap	cdromPixmapMaskOffline;
*/
void drawImage(char *type,char *label,int x,int y,bool mounted);

#endif
