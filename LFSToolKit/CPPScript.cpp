#if 0

#©keithhedger Mon 20 Jul 14:09:10 BST 2015 kdhedger68713@gmail.com

g++ "$0" -I/tmp/AAA/usr/local/include -L/tmp/AAA/usr/local/lib -llfstoolkit -lXm $(pkg-config --cflags --libs xt xext ice sm x11 xft) -lXm -lXaw3d -lXt -lXext -lICE -lSM -lX11||exit 1
./a.out "$@"
retval=$?
rm ./a.out
exit $retval
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
//#include <X11/Xft/Xft.h>

#include <LFSTKWindow.h>
#include <LFSTKButton.h>
#include "LFSTKMenuButton.h"
//#include <LFSTKGlobals.h>

void bcb(void *p,int ud)
{
	printf(">>>from cb %i<<<<\n",ud);
	if(ud==-1)
		exit(0);
}

#define streq(a,b)	( strcmp((a),(b))==0 )
#define BUFFERSIZE 2048
#define MAXCATS 500
#define MAXENTRYS 500

struct			menuEntryStruct
{
	char			*name;
	char			*exec;
};

struct			appMenuStruct
{
	const char		*name;
	menuEntryStruct	entry[MAXENTRYS];
	int				maxentrys;
};

const char		*myCats[]= {"Settings","Utility","Development","Education","Graphics","Network","AudioVideo","Audio","Video","Office","Shell","Game","System",NULL};

appMenuStruct	mainMenus[MAXCATS];
bool			mainloop=false;
bool			makestatic=false;

void setCatagories(void)
{
	FILE	*fp;
	char	buffer[BUFFERSIZE];
	char	*splitstr=NULL;
	int		mycatcnt;
	bool	foundmatch;
	FILE	*filedata;
	int		foundcatmatch;
	char	foundnamebuffer[BUFFERSIZE];
	char	foundexecbuffer[BUFFERSIZE];
	bool	overridefound;

	for(int j=0; j<MAXCATS; j++)
		{
			mainMenus[j].name=NULL;
			mainMenus[j].maxentrys=0;
			for(int k=0; k<MAXENTRYS; k++)
				{
					mainMenus[j].entry[k].name=NULL;
					mainMenus[j].entry[k].exec=NULL;

				}
		}

	fp=popen("find /usr/share/applications -name \"*.desktop\"|sort","r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			while(fgets(buffer,BUFFERSIZE,fp))
				{
					buffer[strlen(buffer)-1]=0;
					filedata=fopen(buffer,"r");
					if(filedata!=NULL)
						{
							foundmatch=false;
							overridefound=false;
							while(fgets(buffer,BUFFERSIZE,filedata))
								{
									if(buffer[strlen(buffer)-1]=='\n')
										buffer[strlen(buffer)-1]=0;
									splitstr=NULL;
									splitstr=strtok(buffer,"=");

									if(splitstr!=NULL)
										{
											if(strncmp(splitstr,"Categories",10)==0)
												{
													mycatcnt=0;
													foundcatmatch=-1;
													splitstr=strtok(NULL,"=");
													while(myCats[mycatcnt]!=NULL)
														{
															if(strstr(splitstr,myCats[mycatcnt])!=NULL)
																{
																	foundmatch=true;
																	foundcatmatch=mycatcnt;
																	break;
																}
															mycatcnt++;
														}
												}
											if(strcmp(splitstr,"Name")==0)
												{
													splitstr=strtok(NULL,"=");
													sprintf(foundnamebuffer,"%s",splitstr);
												}
											if(strcmp(splitstr,"Exec")==0)
												{
													splitstr=strtok(NULL,"=");
													sprintf(foundexecbuffer,"%s",splitstr);
												}
											if(strcmp(splitstr,"NoDisplay")==0)
												{
													splitstr=strtok(NULL,"=");
													if(strcasecmp(splitstr,"true")==0)
														overridefound=true;
												}
										}
								}

							if((foundmatch==true) && (overridefound==false))
								{
									mainMenus[foundcatmatch].name=myCats[foundcatmatch];
									char *ptr=strstr(foundexecbuffer,"%");
									while(ptr!=NULL)
										{
											sprintf(ptr,"  ");
											ptr=strstr(foundexecbuffer,"%");
										}
									mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].name=strdup(foundnamebuffer);
									mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].exec=strdup(foundexecbuffer);
									mainMenus[foundcatmatch].maxentrys++;
								}
							fclose(filedata);
						}
				}
			pclose(fp);
		}
}
#include <typeinfo>
#include <strings.h>
#include <string>
int main(int argc, char **argv)
{
	int keep_running=1;
	XEvent event;
	setCatagories();
	LFSTK_buttonClass *bc;
	LFSTK_menuButtonClass *bc1;

	printf("Hello World!\n");

	LFSTK_windowClass *wc=new LFSTK_windowClass(500,1100,800,400,true,"rgb:00/00/00","rgb:80/80/80");
	wc->LFSTK_setDecorated(true);
//using std::string;
//string s=typeid(LFSTK_windowClass).name();
//
//printf(">>>%s<<<\n",s.c_str());
menuItemStruct *ms,*pms;

	int sx=0;
	int sy=0;
					//bc1=new LFSTK_menuButtonClass(wc,(char*)"david",sx,sy,100,28,0,"rgb:a0/a0/a0","rgb:d0/d0/d0","rgb:80/80/80");
					//XMapWindow(wc->display,bc1->window);

	for(int j=0; j<MAXCATS; j++)
		{
			if(mainMenus[j].name!=NULL)
				{
					//bc=new LFSTK_buttonClass(wc,(char*)mainMenus[j].name,sx,sy,100,28,0,"rgb:a0/a0/a0","rgb:d0/d0/d0","rgb:80/80/80");
					//bc->LFSTK_setCallBack(NULL,bcb,j+100);
					//bc->LFSTK_setStyle(EMBOSSEDBUTTON);
					//XMapWindow(wc->display,bc->window);
					
					//sy+=28;
					//bc1=NULL;
					bc1=new LFSTK_menuButtonClass(wc,(char*)mainMenus[j].name,sx,sy,100,28,0,"rgb:a0/a0/a0","rgb:d0/d0/d0","rgb:80/80/80");
					bc1->LFSTK_setCallBack(NULL,bcb,j);
					bc1->LFSTK_setStyle(FLATBUTTON);
					bc1->LFSTK_setLabelOriention(CENTRE);

					XMapWindow(wc->display,bc1->window);
					sy+=28;
				//wc->LFSTK_setListener(bc1->window,bc1->LFSTK_getListen());
					//bc->LFSTK_clearWindow();
					//bc1->LFSTK_clearWindow();
					//printf("+++%s+++\n",mainMenus[j].name);
					ms=new menuItemStruct[mainMenus[j].maxentrys];
					pms=ms;
					for(int k=0; k<mainMenus[j].maxentrys; k++)
						{
							pms->label=strdup(mainMenus[j].entry[k].name);
							pms->userData=k;
							pms++;
						//printf("---%s---\n",mainMenus[j].entry[k].name);
						
							//entry=XtVaCreateManagedWidget(mainMenus[j].entry[k].name,smeBSBObjectClass,menu,NULL);
							//XtVaSetValues(menu,XmNwidth,400,NULL);
							//XtAddCallback(entry,XtNcallback,entrySelectCB,(XtPointer)(long)(j*256+k));
						}
					bc1->LFSTK_addMenus(ms,mainMenus[j].maxentrys);
				}
		}
	XResizeWindow(wc->display,wc->window,100,sy);
	wc->LFSTK_resizeWindow(100,sy);
	wc->LFSTK_clearWindow();
	XMapWindow(wc->display,wc->window);

//string sbc=typeid(bc).name();
//string sbc1=typeid(bc1).name();

//printf(">>>%s<<<\n",sbc.c_str());
//printf(">>>%s<<<\n",sbc1.c_str());


/*
	LFSTK_buttonClass *bc=new LFSTK_buttonClass(wc,"button 1",50,50,100,100,0,"rgb:a0/a0/a0","rgb:d0/d0/d0");
	LFSTK_buttonClass *bc1=new LFSTK_buttonClass(wc,"button 2",250,250,100,50,0,"rgb:f0/a0/a0","rgb:e0/d0/d0");

	bc->LFSTK_setCallBack(bcb,0xdeadbeef);
	bc1->LFSTK_setCallBack(bcb,-666);
	bc->listen.userData=87654321;
	bc->LFSTK_setStyle(FLATBUTTON);

	XMapWindow(wc->display,bc->window);
	wc->LFSTK_setListener(bc->window,bc->LFSTK_getListen());

	bc1->listen.userData=666;
	XMapWindow(wc->display,bc1->window);
	wc->LFSTK_setListener(bc1->window,bc1->LFSTK_getListen());
*/
	while (keep_running)
		{
			XNextEvent(wc->display,&event);
			listener *l=wc->LFSTK_getListener(event.xany.window);
//		if(l==NULL)
//			{
//			continue;
//			}
//		else
	//	{
//			fprintf(stderr,"listner=%p type=%i\n",l,l->type);
//		} && (l->type<2 && l->type>0)
			
//if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) && (l->type<2 && l->type>0))
//printf("0000000000000 %p %p\n",l->function,l);
			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);
//				l=NULL;
//printf("1111111111111111\n");

			switch(event.type)
				{
				case ClientMessage:
					if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
						keep_running = 0;
					break;

				case Expose:
					wc->LFSTK_clearWindow();
				//	bc->LFSTK_clearWindow();
					//bc1->LFSTK_clearWindow();
					break;
				case ConfigureNotify:
					wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
					wc->LFSTK_clearWindow();

				default:
					break;
				}
		}

	//delete bc;
	//delete wc;
	return 0;
}