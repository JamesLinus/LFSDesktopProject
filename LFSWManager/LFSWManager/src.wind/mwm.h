
#ifndef _MWM_
#define _MWM_

void mwm_startwm(void);
void mwm_manage(struct client *);
void mwm_propertynotify(struct client *,XPropertyEvent *);

#endif
