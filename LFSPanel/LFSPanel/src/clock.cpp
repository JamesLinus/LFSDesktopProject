/*
 *
 * ©K. D. Hedger. Mon 21 Sep 13:41:36 BST 2015 kdhedger68713@gmail.com

 * This file (clock.cpp) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <signal.h>
#include <ctime>

#include "clock.h"

LFSTK_labelClass	*clockButton=NULL;
int					refreshRate=1;

void  alarmCallBack(int sig)
{
	char		clockbuffer[256];
	time_t		rawtime;
	struct tm	*timeinfo;

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	strftime(clockbuffer,255,"%I:%M:%S",timeinfo);
	clockButton->LFSTK_setLabel(clockbuffer);
	
	signal(SIGALRM,SIG_IGN);
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
	XFlush(mainwind->display);
}

void addClock(int x,int y)
{
	clockButton=new LFSTK_labelClass(mainwind,"--:--:--",x,y,BWIDTH,panelHeight,NorthWestGravity);
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
}