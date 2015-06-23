
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#include "wind.h"

Display				*dpy;
struct font 		*font;
struct fontcolor	*fhighlight;
struct fontcolor	*fnormal;

Atom				WM_CHANGE_STATE;
Atom				WM_DELETE_WINDOW;
Atom				WM_PROTOCOLS;
Atom				WM_STATE;

