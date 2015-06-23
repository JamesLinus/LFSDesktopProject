
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#ifndef _MWM_
#define _MWM_

void mwm_startwm(void);
void mwm_manage(struct client *);
void mwm_propertynotify(struct client *,XPropertyEvent *);

#endif
