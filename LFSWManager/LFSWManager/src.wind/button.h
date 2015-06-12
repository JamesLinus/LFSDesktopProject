
#ifndef _BUTTON_
#define _BUTTON_

#include "wind.h"

struct button
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
