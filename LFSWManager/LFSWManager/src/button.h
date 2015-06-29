
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/


#ifndef _BUTTON_
#define _BUTTON_

#include "wind.h"

struct			button
{
					listener listen;
					void (*function)(void *,Time);
					void *arg;
					struct bitmap *bitmap;
					Pixmap pixmap;
					int width;
					int height;
					Window window;
					Bool pressed;
					Bool entered;
};

extern bitmap	*deletebitmap;
extern bitmap	*maximizeBitmap;

void update(struct button *b);
void buttonpress(struct button *,XButtonEvent *);
void buttonrelease(struct button *,XButtonEvent *);
void enternotify(struct button *,XCrossingEvent *);
void leavenotify(struct button *,XCrossingEvent *);
void expose(struct button *,XExposeEvent *);
void unmapnotify(struct button *,XUnmapEvent *);
void buttonevent(void *,XEvent *);

struct button *bcreate(void (*)(void *,Time),void *,struct bitmap *,Window,int,int,int,int,int);
void bdestroy(struct button *);

#endif
