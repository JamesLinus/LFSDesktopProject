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
#include "disks.h"
#include "cpu.h"
#include "windowlist.h"

LFSTK_labelClass	*clockButton=NULL;
int					refreshRate=1;
int					updateWindowCnt=0;

void  alarmCallBack(int sig)
{
	char		clockbuffer[256];
	time_t		rawtime;
	struct tm	*timeinfo;

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	strftime(clockbuffer,255,"%I:%M:%S",timeinfo);
	clockButton->LFSTK_setLabel(clockbuffer);

	if(diskButton!=NULL)
		updateDiskStats();
	if(cpuButton!=NULL)
		updateCpuStats();

	if(windowMenu!=NULL)
		{
			updateWindowCnt++;
			if(updateWindowCnt==WINDOWREFRESH)
				{
					updateWindowCnt=0;
					updateWindowMenu();
				}
		}

	signal(SIGALRM,SIG_IGN);
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
	XFlush(mainwind->display);
}

int addClock(int x,int y,int grav)
{
	clockButton=new LFSTK_labelClass(mainwind,"--:--:--",x,0,BWIDTH,panelHeight,grav);
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
	return(BWIDTH+SPACING);
}
