
//Mon 31 Jul 2006 12:30:55 BST
//
//LFSDesktop
//

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>

#include <X11/extensions/Xinerama.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xdbe.h>

#include <Imlib2.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

#include "config.h"

#include "prefs.h"
#include "files.h"
#include "graphics.h"

#define UNKNOWNARG -100

bool			done=true;

//const char		*diskImagePath=DATADIR "harddrive.png";
const char		*diskImagePath="/media/LinuxData/Development/Projects/LFSDesktopProject/LFSDesktop/LFSDesktop/resources/pixmaps/" "harddrive.png";
//const char		*diskImagePathOffline="/media/LinuxData/Development/Projects/LFSDesktopProject/LFSDesktop/drive-removable-media-offline.png";
const char		*diskImagePathOffline="/media/LinuxData/Development/Projects/LFSDesktopProject/LFSDesktop/LFSDesktop/resources/pixmaps/" "harddrive-offline.png";
//const char		*diskImagePathOffline=DATADIR "harddrive-offline.png";

const char		*usbImagePath="/media/LinuxData/Development/Projects/LFSDesktopProject/LFSDesktop/LFSDesktop/resources/pixmaps/usb.png";
const char		*usbImagePathOffline="/media/LinuxData/Development/Projects/LFSDesktopProject/LFSDesktop/LFSDesktop/resources/pixmaps/usb-offline.png";
const char		*cdromImagePath="/media/LinuxData/Development/Projects/LFSDesktopProject/LFSDesktop/LFSDesktop/resources/pixmaps/cdrom.png";
const char		*cdromImagePathOffline="/media/LinuxData/Development/Projects/LFSDesktopProject/LFSDesktop/LFSDesktop/resources/pixmaps/cdrom-ofline.png";

//unsigned long	labelBackground;
//unsigned long	labelForeground;
GC				labelGC;
XFontStruct		*labelFont;

int				xCnt;
int				yCnt;
int				**xySlot;

Time			time=0;
bool			firstClick=false;
bool			foundIcon=false;
bool			needsRefresh=true;

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};


struct option long_options[] =
{
	{"clean",0,0,'c'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0,0,0,0}
};

void printhelp(void)
{
	printf("Usage: lfsdesktop [OPTION]\n"
	       "A CLI application\n"
	       " -c,--clean		Clean disk info data\n"
	       " -v,--version	output version information and exit\n"
	       " -h,-?,--help	print this help\n\n"
	       "Report bugs to kdhedger@yahoo.co.uk\n"
	      );
}

void createColours(void)
{
	XColor	colour;
	int		cc=(int)GXcopyInverted;

	XParseColor(display,DefaultColormap(display,screen),"grey40",&colour);
	XAllocColor(display,DefaultColormap(display,screen),&colour);
	labelBackground=colour.pixel;
	XParseColor(display,DefaultColormap(display,screen),"white",&colour);
	XAllocColor(display,DefaultColormap(display,screen),&colour);
	labelForeground=colour.pixel;

	labelGC=XCreateGC(display,drawOnThis,GCFunction,(XGCValues*) &cc);
	labelFont=XQueryFont(display,XGContextFromGC(gc));
	if(!labelFont)
		fprintf(stderr,"XLoadQueryFont: failed loading default font");
}

int get_argb_visual(Visual** vis,int *depth)
{
	/* code from gtk project,gdk_screen_get_rgba_visual */
	XVisualInfo visual_template;
	XVisualInfo *visual_list=NULL;
	int nxvisuals=0,i;
	visual_template.screen=screen;
	visual_list=XGetVisualInfo (display,0,&visual_template,&nxvisuals);

	for (i=0; i < nxvisuals; i++)
		{
			if (visual_list[i].depth==32 &&
			        (visual_list[i].red_mask==0xff0000 &&
			         visual_list[i].green_mask==0x00ff00 &&
			         visual_list[i].blue_mask==0x0000ff ))
				{
					*vis=visual_list[i].visual;
					*depth=visual_list[i].depth;
					XFree(visual_list);
					return 0;
				}
		}
	// no argb visual available
	printf("no rgb\n");
	XFree(visual_list);
	return 1;
}

void createDesktopWindow(void)
{
	int	rc=0;
	Atom	xa;
	Atom	xa_prop[10];
	Hints	hints;

	rc=get_argb_visual(&visual,&depth);
	if(rc==0)
		{
			XSetWindowAttributes attr;
			attr.colormap=XCreateColormap(display,DefaultRootWindow(display),visual,AllocNone);
			attr.border_pixel=0;
			attr.background_pixel=0;

			rootWin=XCreateWindow(display,DefaultRootWindow(display),0,0,displayWidth,displayHeight,0,depth,InputOutput,visual,CWEventMask |CWColormap | CWBorderPixel | CWBackPixel ,&attr);
			//XSelectInput(display,rootWin,StructureNotifyMask| ButtonPress| ButtonReleaseMask|PointerMotionMask);

			xa=XInternAtom(display,"_NET_WM_STATE",False);
			xa_prop[0]=XInternAtom(display,"_NET_WM_STATE_STICKY",False);
			xa_prop[1]=XInternAtom(display,"_NET_WM_STATE_BELOW",False);
			xa_prop[2]=XInternAtom(display,"_NET_WM_STATE_SKIP_PAGER",False);
			xa_prop[3]=XInternAtom(display,"_NET_WM_STATE_SKIP_TASKBAR",False);
			xa_prop[4]=XInternAtom(display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);
			xa_prop[9]=XInternAtom(display,"_MOTIF_WM_HINTS",True);

			xa=XInternAtom(display,"_NET_WM_STATE",False);
			if(xa!=None)
				XChangeProperty(display,rootWin,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,5);

			hints.flags=2;
			hints.decorations=0;
			XChangeProperty(display,rootWin,xa_prop[9],xa_prop[9],32,PropModeReplace,(unsigned char *)&hints,5);

			rg=XCreateRegion();
			XMapWindow(display,rootWin);
			XSync(display,False);

			XMoveWindow(display,rootWin,0,0);

			buffer=XdbeAllocateBackBufferName(display,rootWin,XdbeBackground);
			swapInfo.swap_window=rootWin;
			swapInfo.swap_action=XdbeBackground;
			XdbeSwapBuffers(display,&swapInfo,1);
			drawOnThis=buffer;
		}
}

void makeDiskInfofile(char* diskfilepath,char* label,char* uuid,int x,int y,char* type)
{
	char	*filepath;

	diskName=label;
	diskUUID=uuid;
	diskXPos=x;
	diskYPos=y;
	diskType=type;

	if(diskfilepath==NULL)
		{
			asprintf(&filepath,"%s/%s",diskInfoPath,uuid);
			saveVarsToFile(filepath,diskData);
			free(filepath);
		}
	else
		saveVarsToFile(diskfilepath,diskData);
}

void createDiskInfo(void)
{
	FILE	*fp;
	FILE	*fd;

	char	*command;
	char	line[1024];
	char	*devname;
	char	*label;
	char	*uuid;
	char	*type;
	char	*disktype;
	char	*diskfilepath;
	int		posx;
	int		posy;

	posx=0;
	posy=0;

	asprintf(&command,"lsblk -n --output=TYPE,NAME,UUID,LABEL,TRAN -lps");
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					devname=NULL;
					label=NULL;
					uuid=NULL;
					type=NULL;
					disktype=NULL;
					sscanf(line,"%as %as %as %a[^\n^\t^ ]s",&disktype,&devname,&uuid,&label);
					//printf("line=>>%s<<\n",line);
					fgets(line,1024,fp);
					sscanf(line,"%*s %*s %a[^\n]s",&type);
					//printf("line=>>%s<< type=>>%s<<\n",line,type);
					//if((devname!=NULL) && (strcmp(devname,"/dev/sr0")==0))
					if((disktype!=NULL) && (strcmp(disktype,"rom")==0))
						{
							if(type!=NULL)
								{
									free(type);
									asprintf(&type,"cdrom");
								}
							if(label==NULL)
								asprintf(&label,"CDROM");
						}
					if((type!=NULL) && (strcmp(type,"sata")==0))
						{
							free(type);
							asprintf(&type,"harddisk");
						}

					if((type!=NULL) && (strcmp(type,"usb")==0))
						{
							free(type);
							asprintf(&type,"disk-usb");
						}

					if(label==NULL)
						asprintf(&label,"IGNOREDISK");

					asprintf(&diskfilepath,"%s/%s",diskInfoPath,uuid);

					if((uuid!=NULL) && (strlen(uuid)>1))
						{
							fd=fopen(diskfilepath,"r");
							if(fd==NULL)
								{
									fd=fopen(diskfilepath,"w");
									if(fd==NULL)
										{
											printf("Can't open disk folder ...\n");
											return;
										}

									while(xySlot[posx][posy]!=0)
										{
											posy++;
											if(posy>MAXGRIDY)
												{
													posy=0;
													posx++;
												}
										}
									makeDiskInfofile(diskfilepath,label,uuid,posx,posy,type);
									xySlot[posx][posy]=1;
									fclose(fd);
								}
							free(diskfilepath);
							free(disktype);
						}
					line[0]=0;
				}
			pclose(fp);
		}
	free(command);
}

void getDiskList(args *diskdata)
{
	FILE		*fp;
	FILE		*fd;
	char		*diskfilepath;
	char		*command;
	char		line[2048];
	int			diskx;
	int			disky;
	char		dataline[256];
	char		uuid[256];
	char		label[256];

	XColor		colour;
	int			fontheight;
	int			stringwidth;

	int			boxx,boxy,boxw,boxh;
	XRectangle	rect;

	XDestroyRegion(rg);
	rg=XCreateRegion();

	asprintf(&command,"lsblk -n --output=UUID -lpds");
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					if(strlen(line)>0)
						{
							asprintf(&diskfilepath,"%s/%s",diskInfoPath,line);
							if(fileExists(diskfilepath)!=0)
								createDiskInfo();

							loadVarsFromFile(diskfilepath,diskdata);
							int xxx,yyy;
							char *dname,*duid,*dtype;

							args ag;
							ag=diskdata[2];
							diskx=(int)(*(int*)(ag.data));

							ag=diskdata[3];
							disky=(int)(*(int*)(ag.data));

							xySlot[diskx][disky]=1;
							diskx=diskx*gridSize+gridBorder;
							disky=disky*gridSize+gridBorder;

							ag=diskdata[0];
							dname=strdup((char*)(*((char**)ag.data)));
							ag=diskdata[1];
							duid=strdup((char*)(*((char**)ag.data)));
							ag=diskdata[4];
							dtype=strdup((char*)(*((char**)ag.data)));

							if(strcmp(dname,"IGNOREDISK")!=0)
								{
									FILE	*tp;
									char	*com;

									asprintf(&com,"findmnt -fn $(findfs UUID=%s)",duid);
									line[0]=0;
									tp=popen(com,"r");
									free(com);
									fgets(line,2048,tp);
									pclose(tp);


									if(strlen(line)>0)
										drawImage(dtype,dname,diskx,disky,true);
									else
										drawImage(dtype,dname,diskx,disky,false);

									rect.x=diskx;
									rect.y=disky;
									rect.width=iconSize;
									rect.height=iconSize;
									XUnionRectWithRegion(&rect,rg, rg);

									XSetClipMask(display,gc,0);

									fontheight=labelFont->ascent+labelFont->descent;
									stringwidth=XTextWidth(labelFont,label,strlen(dname));

									boxx=diskx+(iconSize/2)-(stringwidth/2)-1;
									boxy=disky+iconSize+1;
									boxw=stringwidth+2;
									boxh=fontheight-2;

									XSetForeground(display,gc,labelBackground);
									XSetFillStyle(display,gc,FillSolid);
									XFillRectangle(display,drawOnThis,gc,boxx,disky+iconSize,boxw,boxh);

									XSetForeground(display,labelGC,labelForeground);
									XSetBackground(display,labelGC,labelBackground);

									XDrawString(display,drawOnThis,labelGC,boxx+1,disky+iconSize+boxh-1,dname,strlen(dname));
								}
						}
				}
			fclose(fp);
		}
	free(command);
	XShapeCombineRegion(display,rootWin,ShapeInput,0,0,rg,ShapeSet);
}

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

	asprintf(&command,"find %s -mindepth 1",diskInfoPath);
	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				{
					line[strlen(line)-1]=0;
					loadVarsFromFile(line,diskData);
					if(strlen(line)>0)
						{
							if(strlen(diskUUID)>1)
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

void mountDisk(int x, int y)
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
					loadVarsFromFile(line,diskData);
					if(strlen(line)>0)
						{
							if(strlen(diskUUID)>1)
								{
									if((x>=(diskXPos*gridSize+gridBorder))&&(x<=(diskXPos*gridSize+gridBorder)+iconSize)&&(y>=(diskYPos*gridSize+gridBorder))&&(y<=(diskYPos*gridSize+gridBorder)+iconSize))
										{
											asprintf(&command,"udevil mount `findfs UUID=%s`",diskUUID);
											system(command);
											free(command);
											asprintf(&command,"findmnt -lno TARGET -S UUID=\"%s\"|xargs xdg-open",diskUUID);
											system(command);
											free(command);
											
											pclose(fp);
											return;
										}
								}
						}
				}
			pclose(fp);
		}
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
	Imlib_Image		diskimage;

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
			c=getopt_long (argc,argv,"v?h:c",long_options,&option_index);
			if (c==-1)
				break;

			switch (c)
				{
				case 'c':
					asprintf(&command,"rm %s/*",diskInfoPath);
					system(command);
					free(command);
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

	createDesktopWindow();

	gc=XCreateGC(display,drawOnThis,0,NULL);
	XSetFillStyle(display,gc,FillSolid);
	XSelectInput(display,rootWin,StructureNotifyMask |ExposureMask | ButtonPress| ButtonReleaseMask|PointerMotionMask | EnterWindowMask | LeaveWindowMask);

	blackColor=BlackPixel(display,screen);
	whiteColor=WhitePixel(display,screen);

	imlib_context_set_dither(0);
	imlib_context_set_display(display);
	imlib_context_set_visual(visual);
	imlib_context_set_drawable(drawOnThis);

	hcreate(100);

	char	*tstr;
//	ENTRY	testentry={"sata",NULL};
//	ENTRY	*retentry=NULL;

//sata
//	retentry=hsearch(testentry,ENTER);
//	if(retentry==NULL)
//		printf(">>NULL<<\n");
//	else
//		{
//		printf(">>%s<<>>%s<<\n",retentry->key,"XX");
//		retentry->data=malloc(sizeof(diskIconStruct));
//		((diskIconStruct*)(retentry->data))->pixmap=-666;
//	}
	
	
	//makeImage((char*)diskImagePath,(char*)"sata",SATA);
//	tstr=pathToIcon("harddisk");
//	printf(">>%s<<\n",tstr);
//	makeImage((char*)tstr,(char*)"sata",SATA,(diskIconStruct*)(retentry->data));
//	free(tstr);
//usb
//	makeImage((char*)usbImagePath,(char*)"usb",USB);
	tstr=pathToIcon("disk-usb");
//	printf(">>%s<<\n",tstr);
	makeImage((char*)tstr,(char*)"usb",USB,NULL);
	free(tstr);

//cdrom
//	makeImage((char*)cdromImagePath,(char*)"cdrom",CDROM);
	tstr=pathToIcon("cdrom");
//	printf(">>%s<<\n",tstr);
	makeImage((char*)tstr,(char*)"cdrom",CDROM,NULL);
	free(tstr);

	createColours();

	xCnt=displayWidth/gridSize;
	yCnt=displayHeight/gridSize;

	xySlot=(int**)malloc(xCnt*sizeof(int*));
	for(int j=0; j<xCnt; j++)
		xySlot[j]=(int*)malloc(yCnt*sizeof(int));

	for(int yy=0; yy<yCnt; yy++)
		for(int xx=0; xx<xCnt; xx++)
			xySlot[xx][yy]=0;

	alarm(refreshRate);

	getDiskList(diskData);
	createDiskInfo();
	XdbeSwapBuffers(display,&swapInfo,1);

	char	*fdiskname=NULL;
	char	*fdiskuuid=NULL;
	char	*fdisktype=NULL;
	int		oldx=-1,oldy=-1;
	bool	buttonDown=false;
	int	oldboxx=-1,oldboxy=-1;

	while(done)
		{
			if(needsRefresh==true)
				{
					getDiskList(diskData);
					XdbeSwapBuffers(display,&swapInfo,1);
					needsRefresh=false;
				}

			XCheckWindowEvent(display,rootWin,StructureNotifyMask |ExposureMask | ButtonPress| ButtonReleaseMask|PointerMotionMask | EnterWindowMask | LeaveWindowMask,&ev);

			switch(ev.type)
				{
				case MapNotify:
					break;
				case Expose:
					break;
				case ConfigureNotify:
					break;
				case VisibilityNotify:
					break;
				case ButtonPress:
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
							if(fdiskname!=NULL)
								free(fdiskname);
							fdiskname=strdup(diskName);
							if(fdiskuuid!=NULL)
								free(fdiskuuid);
							fdiskuuid=strdup(diskUUID);
							if(fdisktype!=NULL)
								free(fdisktype);
							fdisktype=strdup(diskType);
							oldx=diskXPos;
							oldy=diskYPos;
							oldboxx=ev.xbutton.x;
							oldboxy=ev.xbutton.y;
						}
					else
						{
							if(fdiskname!=NULL)
								free(fdiskname);
							fdiskname=NULL;
							if(fdiskuuid!=NULL)
								free(fdiskuuid);
							if(fdisktype!=NULL)
								free(fdisktype);
							fdisktype=NULL;
							fdiskuuid=NULL;
							diskName=NULL;
							diskUUID=NULL;
							diskType=NULL;
							oldx=-1;
							oldy=-1;
						}

					break;
				case ButtonRelease:
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
									makeDiskInfofile(NULL,fdiskname,fdiskuuid,newx,newy,fdisktype);
									needsRefresh=true;
								}
							if(fdiskname!=NULL)
								free(fdiskname);
							fdiskname=NULL;
							if(fdiskuuid!=NULL)
								free(fdiskuuid);
							if(fdisktype!=NULL)
								free(fdisktype);
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
						char	*dn=NULL;
						char	*du=NULL;
						int		dx=0;
						int		dy=0;
						char	*dtype=NULL;
						args	diskdata[]=
							{
								{"diskname",TYPESTRING,&dn},
								{"diskuuid",TYPESTRING,&du},
								{"diskx",TYPEINT,&dx},
								{"disky",TYPEINT,&dy},
								{"type",TYPESTRING,&dtype},
								{NULL,0,NULL}
							};

						if(foundIcon==true && buttonDown==true)
							{
								alarm(0);
								if((ev.xmotion.x>oldboxx+iconSize) || (ev.xmotion.x<oldboxx-iconSize) || (ev.xmotion.y>oldboxy+iconSize) || (ev.xmotion.y<oldboxy-iconSize))
									{
										oldboxx=ev.xmotion.x;
										oldboxy=ev.xmotion.y;
										dx=0;
										dy=0;

										XSetForeground(display,gc,labelBackground);
										XSetFillStyle(display,gc,FillSolid);
										newboxx=((ev.xmotion.x-gridBorder)/gridSize)*gridSize+(gridBorder/2);
										newboxy=((ev.xmotion.y-gridBorder)/gridSize)*gridSize+(gridBorder/2);
										XDrawRectangle(display,drawOnThis,gc,newboxx,newboxy,gridSize,gridSize);
										getDiskList(diskdata);
										XdbeSwapBuffers(display,&swapInfo,1);
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

