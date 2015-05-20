/******************************************************
*
*     ©keithhedger Tue 12 May 10:46:15 BST 2015
*     kdhedger68713@gmail.com
*
*     disks.cpp
*
******************************************************/

#include <X11/extensions/shape.h>

#include <stdio.h>
#include <stdlib.h>

#include <libudev.h>

#include "config.h"

#include "prefs.h"
#include "files.h"
#include "disks.h"

disks		*attached=NULL;
saveDisks	*saved=NULL;
int			numberOfDisksAttached=-1000;
const char	*iconDiskType[]= {"harddisk","harddisk-usb","dev-dvd","dev-dvd","harddisk-usb"};

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
#ifdef _USESUIDHELPER_
											asprintf(&command,"%s %s /media/%s",HELPERAPP,diskUUID,diskName);
#else
											asprintf(&command,"udevil mount `findfs UUID=%s`",diskUUID);
#endif
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

char* getUSBData(char *device)
{
	char	*newdevice=NULL;

	if((device!=NULL) && (strstr(device,"iPod")!=NULL))
		{
			asprintf(&newdevice,"multimedia-player");
			return(newdevice);
		}
	if((device!=NULL) && (strstr(device,"SD/MMC")!=NULL))
		{
			asprintf(&newdevice,"media-sm");
			return(newdevice);
		}
	return(newdevice);
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
	char	*model;
	char	*disktype;
	char	*diskfilepath;
	int		posx;
	int		posy;

	posx=0;
	posy=0;

	asprintf(&command,"lsblk -n --output=TYPE,NAME,UUID,LABEL,TRAN,MODEL -lps");
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
					fgets(line,1024,fp);
					sscanf(line,"%*s %*s %as %a[^\n]s",&type,&model);
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
							char *extrainfo=NULL;
							extrainfo=getUSBData(model);
							free(type);
							if(extrainfo!=NULL)
								asprintf(&type,extrainfo);
							else
								asprintf(&type,"harddisk-usb");
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

#if 0
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
#endif
void deleteDiskInfo(void)
{
	for(int j=0; j<numberOfDisksAttached; j++)
		{
			if(attached[j].uuid!=NULL)
				free(attached[j].uuid);
			if(attached[j].label!=NULL)
				free(attached[j].label);
			if(attached[j].mountpoint!=NULL)
				free(attached[j].mountpoint);
			if(attached[j].dev!=NULL)
				free(attached[j].dev);
			if(attached[j].sysname!=NULL)
				free(attached[j].sysname);
		}
	free(attached);

}

void drawIcons(void)
{
	FILE	*tp;
	char	*com;
	char	line[BUFFERSIZE];
	XColor		colour;
	int			fontheight;
	int			stringwidth;

	int			boxx,boxy,boxw,boxh;
	XRectangle	rect;
	int			diskx,disky;
	bool		mounted=false;

	XDestroyRegion(rg);
	rg=XCreateRegion();

	for(int j=0; j<numberOfDisksAttached; j++)
		{
			if(attached[j].ignore==false)
				{
					asprintf(&com,"findmnt -fn $(findfs UUID=%s)",attached[j].uuid);
					line[0]=0;
					tp=popen(com,"r");
					free(com);
					fgets(line,BUFFERSIZE,tp);
					pclose(tp);

					diskx=attached[j].x*gridSize+gridBorder;
					disky=attached[j].y*gridSize+gridBorder;

					if(strlen(line)>0)
						mounted=true;
					else
						mounted=false;

					drawImage((char*)iconDiskType[attached[j].type],attached[j].label,diskx,disky,mounted);

					rect.x=diskx;
					rect.y=disky;
					rect.width=iconSize;
					rect.height=iconSize;
					XUnionRectWithRegion(&rect,rg, rg);

					XSetClipMask(display,gc,0);

					fontheight=labelFont->ascent+labelFont->descent;
					stringwidth=XTextWidth(labelFont,attached[j].label,strlen(attached[j].label));

					boxx=diskx+(iconSize/2)-(stringwidth/2)-1;
					boxy=disky+iconSize+1;
					boxw=stringwidth+2;
					boxh=fontheight-2;

					XSetForeground(display,gc,labelBackground);
					XSetFillStyle(display,gc,FillSolid);
					XFillRectangle(display,drawOnThis,gc,boxx,disky+iconSize,boxw,boxh);

					XSetForeground(display,labelGC,labelForeground);
					XSetBackground(display,labelGC,labelBackground);

					XDrawString(display,drawOnThis,labelGC,boxx+1,disky+iconSize+boxh-1,attached[j].label,strlen(attached[j].label));
				}
		}
}

void getDiskPos(char* uuid,int* xptr,int* yptr)
{
	for (int j=0;j<savedFileCount;j++)
		{
			if(strcmp(uuid,saved[j].uuid)==0)
				{
					*xptr=saved[j].x;
					*yptr=saved[j].y;
					return;
				}
		}
}

void scanForMountableDisks(void)
{
	struct udev *udev;
	char		buffer[BUFFERSIZE];
	FILE		*fp;
	int			numofdisks;
	const char	*ptr;
	int			xpos=0,ypos=0;
	udev_device *usbdev;

	/* Create the udev object */
	udev=udev_new();
	if (!udev)
		{
			printf("Can't create udev\n");
			exit(1);
		}

	fp=popen(READFROM "|wc -l","r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,BUFFERSIZE,fp);
			numofdisks=atoi(buffer);
			pclose(fp);
		}

	if(numberOfDisksAttached<numofdisks)
		{
			deleteDiskInfo();
			attached=(disks*)calloc(numofdisks,sizeof(disks));
			numberOfDisksAttached=numofdisks;
		}

	fp=popen(READFROM,"r");
	if(fp!=NULL)
		{
			for(int j=0; j<numofdisks; j++)
				{
					buffer[0]=0;
					fgets(buffer,BUFFERSIZE,fp);
					buffer[strlen(buffer)-1]=0;
					udev_device	*thedev=udev_device_new_from_subsystem_sysname(udev,"block",buffer);
					if(thedev!=NULL)
						{
							attached[j].ignore=true;
							if(udev_device_get_property_value(thedev,"ID_FS_UUID")!=NULL)
								{
//get uuid
									attached[j].uuid=strdup(udev_device_get_property_value(thedev,"ID_FS_UUID"));
//partname
									attached[j].sysname=strdup(buffer);
									asprintf(&attached[j].dev,"/dev/%s",buffer);
//is a file system
									if(strcmp(udev_device_get_property_value(thedev,"ID_FS_USAGE"),"filesystem")==0)
										{
											getDiskPos(attached[j].uuid,&xpos,&ypos);
											attached[j].x=xpos;
											attached[j].y=ypos;
											//ypos++;
											//if(ypos>MAXGRIDY)
											//	{
											//		ypos=0;
											//		xpos++;
											//	}
											attached[j].ignore=false;
											ptr=udev_device_get_property_value(thedev,"ID_FS_LABEL");
											if(ptr!=NULL)
												attached[j].label=strdup(ptr);
											else
												{
													ptr=udev_device_get_property_value(thedev,"ID_SERIAL");
													attached[j].label=strndup(ptr,16);
												}

											if(udev_device_get_property_value(thedev,"ID_CDROM_MEDIA_DVD")!=NULL)
												{
													attached[j].dvd=true;
													attached[j].type=DVD;
												}

											usbdev=udev_device_get_parent_with_subsystem_devtype(thedev,"usb","usb_device");
											if(usbdev!=NULL)
												{
													attached[j].usb=true;
													attached[j].type=USB;
												}
										}
								}
							udev_device_unref (thedev);
						}
				}
			pclose(fp);
		}
	udev_unref(udev);
	/*
		for(int j=0; j<numberOfDisksAttached; j++)
			{
				if(attached[j].ignore==false)
					printf("j=%i,uuid=%s , dev=%s , label=%s , sysname=%s , x=%i , y=%i\n",j,attached[j].uuid,attached[j].dev,attached[j].label,attached[j].sysname,attached[j].x,attached[j].y);
			}
	*/

}

