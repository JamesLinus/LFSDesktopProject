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

#include "clock.h"
#include <unistd.h>
#include <signal.h>
//#include <iostream>
#include <ctime>
LFSTK_labelClass	*clockButton=NULL;
int refreshRate=1;
char clockbuffer[256];

void  alarmCallBack(int sig)
{
//printf("----------------\n\n\n");
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo=localtime(&rawtime);

//	strftime(clockbuffer,255,"%d-%m-%Y %I:%M:%S",timeinfo);
	strftime(clockbuffer,255,"%I:%M:%S",timeinfo);
  //std::string str(buffer);
//
 // std::cout << str;
//printf(">>%s<<\n",clockbuffer);
	clockButton->LFSTK_setLabel(clockbuffer);
	//clockButton->LFSTK_clearWindow();
	//clockButton->LFSTK_setLabel("XXXXXXXXXXXXX");
//printf("===============\n");
//	clockButton->LFSTK_drawLabel(NORMALCOLOUR);
	//
//printf("===============\n");
	
	signal(SIGALRM,SIG_IGN);
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
	XFlush(mainwind->display);
	//XSync(mainwind->display,true);

}

void addClock(int x,int y)
{
	clockButton=new LFSTK_labelClass(mainwind,"XX:XX:XX",x,y,BWIDTH,panelHeight,NorthWestGravity);
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
}
