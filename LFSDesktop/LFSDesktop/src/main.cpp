
//Mon 31 Jul 2006 12:30:55 BST
//
//LFSDesktop
//

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/shape.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include <libudev.h>

#include "config.h"

#include "globals.h"
#include "prefs.h"
#include "files.h"
#include "disks.h"
#include "graphics.h"

#define UNKNOWNARG -100

struct	option long_options[] =
{
	{"clean",0,0,'c'},
	{"theme",1,0,'t'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0,0,0,0}
};

bool	needsRefresh=true;

void printhelp(void)
{
	printf("Usage: lfsdesktop [OPTION]\n"
	       "A CLI application\n"
	       " -c,--clean		Clean disk info data\n"
	       " -t,--theme		Set theme\n"
	       " -v,--version	output version information and exit\n"
	       " -h,-?,--help	print this help\n\n"
	       "Report bugs to kdhedger@yahoo.co.uk\n"
	      );
}

int	foundDiskNumber=-1;

bool findIcon(int x, int y)
{
	FILE	*fp;
	FILE	*fd;
	char	*command;
	char	line[2048];
	int		dx,dy;
	char	label[256];
	char	uuid[256];
	char	dataline[256];

	for(int j=0;j<numberOfDisksAttached;j++)
		{
			if((x>=(attached[j].x*gridSize+gridBorder))&&(x<=(attached[j].x*gridSize+gridBorder)+iconSize)&&(y>=(attached[j].y*gridSize+gridBorder))&&(y<=(attached[j].y*gridSize+gridBorder)+iconSize))
				{
					diskXPos=attached[j].x;
					diskYPos=attached[j].y;
					foundDiskNumber=j;
					return(true);
				}
		}
return(false);

	asprintf(&command,"find %s -mindepth 1",diskInfoPath);
	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					if(strlen(line)>0)
						{
							loadVarsFromFile(line,diskData);
							if((diskUUID!=NULL) && (strlen(diskUUID)>1))
								{
									if((x>=(diskXPos*gridSize+gridBorder))&&(x<=(diskXPos*gridSize+gridBorder)+iconSize)&&(y>=(diskYPos*gridSize+gridBorder))&&(y<=(diskYPos*gridSize+gridBorder)+iconSize))
										{
											pclose(fp);
											return(true);
										}
								}
						}
				}
			pclose(fp);
		}
	return(false);
}

bool findIconXX(int x, int y)
{
	FILE	*fp;
	FILE	*fd;
	char	*command;
	char	line[2048];
	int		dx,dy;
	char	label[256];
	char	uuid[256];
	char	dataline[256];

	asprintf(&command,"find %s -mindepth 1",diskInfoPath);
	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					if(strlen(line)>0)
						{
							loadVarsFromFile(line,diskData);
							if((diskUUID!=NULL) && (strlen(diskUUID)>1))
								{
									if((x>=(diskXPos*gridSize+gridBorder))&&(x<=(diskXPos*gridSize+gridBorder)+iconSize)&&(y>=(diskYPos*gridSize+gridBorder))&&(y<=(diskYPos*gridSize+gridBorder)+iconSize))
										{
											pclose(fp);
											return(true);
										}
								}
						}
				}
			pclose(fp);
		}
	return(false);
}

void  alarmCallBack(int sig)
{
	signal(SIGALRM,SIG_IGN);
	needsRefresh=true;
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
}

int main(int argc,char **argv)
{
	int				c;
	XEvent			ev;
	char			*command;
	unsigned long	timer=0;
	FILE			*fw;
	char			*path;
	char			buffer[100];
	pid_t			pid=getpid();
	int				xCnt;
	int				yCnt;
	bool			done=true;
	Time			time=0;
	bool			firstClick=false;
	bool			foundIcon=false;

	asprintf(&path,"%s/.config/LFS/pidfile",getenv("HOME"));
	fw=fopen(path,"r");
	if(fw!=NULL)
		{
			fgets(buffer,100,fw);
			kill(atoi(buffer),SIGKILL);
			fclose(fw);
		}

	fw=fopen(path,"w");
	if(fw!=NULL)
		{
			fprintf(fw,"%i",pid);
			fclose(fw);
		}
	free(path);
	nice(15);

	cairo_surface_t *sfc;

	signal(SIGALRM,alarmCallBack);

	done=true;
	asprintf(&diskInfoPath,"%s/.config/LFS/disks",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",diskInfoPath);
	system(command);
	free(command);

	asprintf(&cachePath,"%s/.config/LFS/cache",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",cachePath);
	system(command);
	free(command);

	asprintf(&prefsPath,"%s/.config/LFS/lfsdesktop.rc",getenv("HOME"));
	loadVarsFromFile(prefsPath,desktopPrefs);

	while (1)
		{
			int option_index=0;
			c=getopt_long (argc,argv,"v?hct:",long_options,&option_index);
			if (c==-1)
				break;

			switch (c)
				{
				case 'c':
					asprintf(&command,"rm %s/*",diskInfoPath);
					system(command);
					free(command);
					break;

				case 't':
					if(iconTheme!=NULL)
						free(iconTheme);
					iconTheme=strdup(optarg);
					break;

				case 'v':
					printf("lfsdesktop %s\n",VERSION);
					return 0;
					break;

				case '?':
				case 'h':
					printhelp();
					return 0;
					break;

				default:
					fprintf(stderr,"?? Unknown argument ??\n");
					return UNKNOWNARG;
					break;
				}
		}

	if (optind < argc)
		{
			printf("non-option ARGV-elements: ");
			while (optind < argc)
				printf("%s ",argv[optind++]);
			printf("\n");
		}

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	XSynchronize(display,true);
	screen=DefaultScreen(display);
	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);
	screen=DefaultScreen(display);
	rootWin=DefaultRootWindow(display);
	visual=DefaultVisual(display,screen);
	cm=DefaultColormap(display,screen);

	createDesktopWindow();

	sfc=cairo_xlib_surface_create(display,drawOnThis,visual,displayWidth,displayHeight);
	cairo_xlib_surface_set_size(sfc,displayWidth,displayHeight);
	cr=cairo_create(sfc);

	gc=XCreateGC(display,drawOnThis,0,NULL);
	XSetFillStyle(display,gc,FillSolid);
	XSelectInput(display,rootWin,StructureNotifyMask |ExposureMask | ButtonPress| ButtonReleaseMask|PointerMotionMask | EnterWindowMask | LeaveWindowMask);

	blackColor=BlackPixel(display,screen);
	whiteColor=WhitePixel(display,screen);

	hcreate(100);

	createColours();

	xCnt=displayWidth/gridSize;
	yCnt=displayHeight/gridSize;

	xySlot=(int**)malloc(xCnt*sizeof(int*));
	for(int j=0; j<xCnt; j++)
		xySlot[j]=(int*)malloc(yCnt*sizeof(int));

	for(int yy=0; yy<yCnt; yy++)
		for(int xx=0; xx<xCnt; xx++)
			xySlot[xx][yy]=0;

	fileInfoPtr=(fileInfo*)calloc(20,sizeof(fileInfo));
	asprintf(&command,"%s/home.rc",cachePath);
	desktopFilesCnt=0;
	if(fileExists(command)!=0)
		{
			fw=fopen(command,"w");
			if(fw!=NULL)
				{
					desktopFilesCnt=1;
					fileInfoPtr[0].x=0;
					fileInfoPtr[0].y=0;
					fileInfoPtr[0].label=strdup("Home");
					fileInfoPtr[0].mime=strdup("inode/directory");
					fileInfoPtr[0].path=strdup(getenv("HOME"));

					fprintf(fw,"label	%s\n",fileInfoPtr[0].label);
					fprintf(fw,"mime	%s\n",fileInfoPtr[0].mime);
					fprintf(fw,"path	%s\n",fileInfoPtr[0].path);
					fprintf(fw,"xpos	%i\n",fileInfoPtr[0].x);
					fprintf(fw,"ypos	%i\n",fileInfoPtr[0].y);
					fclose(fw);
				}
		}
	else
		{
			readDesktopFile("home");
		}
	
	free(command);

	getSavedDiskData();
	scanForMountableDisks();
	alarm(refreshRate);

	XdbeSwapBuffers(display,&swapInfo,1);

	char	*fdiskname=NULL;
	char	*fdiskuuid=NULL;
	char	*fdisktype=NULL;
	int		oldx=-1,oldy=-1;
	bool	buttonDown=false;
	int		oldboxx=-1,oldboxy=-1;
	bool	dragging=false;

	fileInfoPtr[0].icon=pathToIcon((char*)"home","places");

	while(done)
		{
			if(needsRefresh==true)
				{
					scanForMountableDisks();
					drawIcons();
					XdbeSwapBuffers(display,&swapInfo,1);
					needsRefresh=false;
				}

			if(dragging==false)
				usleep(10000);

			XCheckWindowEvent(display,rootWin,ButtonPress|ButtonReleaseMask|PointerMotionMask,&ev);

			switch(ev.type)
				{
				case ButtonPress:
					if(ev.xbutton.button!=Button1)
						break;
					dragging=true;
					buttonDown=true;
					if(firstClick==false)
						{
							firstClick=true;
							time=ev.xbutton.time;
						}
					else
						{
							firstClick=false;
							if(ev.xbutton.time-time<800)
								{
									mountDisk(ev.xbutton.x,ev.xbutton.y);
									needsRefresh=true;
								}
							else
								{
									time=ev.xbutton.time;
									firstClick=true;
								}
						}

					if(foundIcon==false)
						{
							oldx=-1;
							oldy=-1;
							foundIcon=findIcon(ev.xbutton.x,ev.xbutton.y);
						}

					if(foundIcon==true)
						{
							//if(fdiskname!=NULL)
							//	free(fdiskname);
							//fdiskname=strdup(diskName);
							//if(fdiskuuid!=NULL)
							//	free(fdiskuuid);
							//fdiskuuid=strdup(diskUUID);
							//if(fdisktype!=NULL)
							//	free(fdisktype);
							//fdisktype=strdup(diskType);
							oldx=diskXPos;
							oldy=diskYPos;
							oldboxx=ev.xbutton.x;
							oldboxy=ev.xbutton.y;
						}
					else
						{
							//if(fdiskname!=NULL)
							//	free(fdiskname);
							//fdiskname=NULL;
							//if(fdiskuuid!=NULL)
							//	free(fdiskuuid);
							//if(fdisktype!=NULL)
							//	free(fdisktype);
							fdisktype=NULL;
							fdiskuuid=NULL;
							diskName=NULL;
							diskUUID=NULL;
							diskType=NULL;
							oldx=-1;
							oldy=-1;
							foundDiskNumber=-1;
						}

					break;
				case ButtonRelease:
				//printf("ButtonRelease\n");
					dragging=false;
					buttonDown=false;
					if(foundIcon==true)
						{
							int newx,newy;
							newx=(ev.xbutton.x-gridBorder)/gridSize;
							newy=(ev.xbutton.y-gridBorder)/gridSize;
							foundIcon=false;
							if(xySlot[newx][newy]==0)
								{
									xySlot[oldx][oldy]=0;
									//DEBUGVAL(foundDiskNumber);
									//makeDiskInfofile(NULL,fdiskname,fdiskuuid,newx,newy,fdisktype);
									makeDiskInfofile(NULL,attached[foundDiskNumber].label,attached[foundDiskNumber].uuid,newx,newy,(char*)iconDiskType[attached[foundDiskNumber].type]);
									needsRefresh=true;
									getSavedDiskData();
									scanForMountableDisks();
								}

							XSetForeground(display,gc,0);
							XSetFillStyle(display,gc,FillSolid);
							newx=((ev.xbutton.x-gridBorder)/gridSize)*gridSize+(gridBorder/2);
							newy=((ev.xbutton.y-gridBorder)/gridSize)*gridSize+(gridBorder/2);
							XDrawRectangle(display,rootWin,gc,newx,newy,gridSize,gridSize);

							//if(fdiskname!=NULL)
							//	free(fdiskname);
							//fdiskname=NULL;
							//if(fdiskuuid!=NULL)
							//	free(fdiskuuid);
							//if(fdisktype!=NULL)
							//	free(fdisktype);
							fdisktype=NULL;
							fdiskuuid=NULL;
							diskName=NULL;
							diskUUID=NULL;
							diskType=NULL;
							oldx=-1;
							oldy=-1;
							alarm(refreshRate);
						}

					break;
				case MotionNotify:
				{
					int newboxx,newboxy;

					if(foundIcon==true && buttonDown==true)
						{
							alarm(0);
							if((ev.xmotion.x>oldboxx+iconSize) || (ev.xmotion.x<oldboxx-iconSize) || (ev.xmotion.y>oldboxy+iconSize) || (ev.xmotion.y<oldboxy-iconSize))
								{
									int obx,oby;

									XSetForeground(display,gc,0);
									XSetFillStyle(display,gc,FillSolid);
									obx=((oldboxx-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									oby=((oldboxy-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									XDrawRectangle(display,rootWin,gc,obx,oby,gridSize,gridSize);

									oldboxx=ev.xmotion.x;
									oldboxy=ev.xmotion.y;

									XSetForeground(display,gc,labelBackground);
									XSetFillStyle(display,gc,FillSolid);
									newboxx=((ev.xmotion.x-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									newboxy=((ev.xmotion.y-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									XDrawRectangle(display,rootWin,gc,newboxx,newboxy,gridSize,gridSize);
								}
						}
				}
				break;

				default:
					break;
				}
			ev.type=-1;
		}

	XClearWindow(display,rootWin);
	XCloseDisplay(display);
	for(int j=0; j<xCnt; j++)
		free(xySlot[j]);

	return 0;
}

