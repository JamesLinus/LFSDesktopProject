
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#ifndef _FRAME_
#define _FRAME_

struct frame
{
	struct listener listener;
	struct client *client;
	struct button *deletebutton;
	struct button *maximize;
	struct button *minimize;
	struct button *shade;
	struct dragger *topleftresizer;
	struct dragger *toprightresizer;
	Pixmap pixmap;
	GC *background;
	int namewidth;
	int x;
	int y;
	int width;
	int height;
	int oldX;
	int oldY;
	int oldWidth;
	int oldHeight;
	Window window;
	int downx;	// window relative pointer x at button press
	int downy;	// window relative pointer y at button press
	Bool grabbed;
	bool isMaximized;
	bool isShaded;
};

extern Window	windowToUpdate;
extern int		newwid,newhite;
extern bool		fromDragger;


struct frame *fcreate(struct client *);
void fdestroy(struct frame *);
void fupdate(struct frame *);
Window fgetwin(struct frame *);
struct geometry fgetgeom(struct frame *);
struct extents estimateframeextents(Window);
void maximizeWindow(void *myclient,Time t);
void repaint(struct frame *f);

#endif