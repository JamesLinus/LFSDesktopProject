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

//disks		*attached=NULL;
//saveDisks	*saved=NULL;
int			numberOfDisksAttached=-1000;
const char	*iconDiskType[]= {"harddisk","harddisk-usb","dev-cdrom","dev-dvd","media-removable","multimedia-player","flash","user-home"};

void freeAttached(int num)
{
#if 0
	if(attached[num].uuid!=NULL)
		free(attached[num].uuid);
	if(attached[num].label!=NULL)
		free(attached[num].label);
	if(attached[num].mountpoint!=NULL)
		free(attached[num].mountpoint);
	if(attached[num].dev!=NULL)
		free(attached[num].dev);
	if(attached[num].sysname!=NULL)
		free(attached[num].sysname);
	xySlot[attached[num].x][attached[num].y]=0;
	attached[num].uuid=NULL;
	attached[num].label=NULL;
	attached[num].mountpoint=NULL;
	attached[num].dev=NULL;
	attached[num].sysname=NULL;
	attached[num].x=0;
	attached[num].y=0;
#endif
}

void clearDeskEntry(int num,bool clearslot)
{
	if(deskIconsArray[num].label!=NULL)
		free(deskIconsArray[num].label);
	deskIconsArray[num].label=NULL;
	if(deskIconsArray[num].uuid!=NULL)
		free(deskIconsArray[num].uuid);
	deskIconsArray[num].uuid=NULL;
	if(deskIconsArray[num].mountpoint!=NULL)
		free(deskIconsArray[num].mountpoint);
	deskIconsArray[num].mountpoint=NULL;
	if(deskIconsArray[num].partname!=NULL)
		free(deskIconsArray[num].partname);
	deskIconsArray[num].partname=NULL;
	if(deskIconsArray[num].dev!=NULL)
		free(deskIconsArray[num].dev);
	deskIconsArray[num].dev=NULL;
	if(deskIconsArray[num].mime!=NULL)
		free(deskIconsArray[num].mime);
	deskIconsArray[num].mime=NULL;

	if(clearslot==true)
		xySlot[deskIconsArray[num].x][deskIconsArray[num].y]=0;

	deskIconsArray[num].dvd=false;
	deskIconsArray[num].cdrom=false;
	deskIconsArray[num].usb=false;
	deskIconsArray[num].file=false;
	deskIconsArray[num].iconhint=-1;
	deskIconsArray[num].installed=false;
}

void mountDisk(int what)
{
	char	*command;


	if(isDisk==false)
		{

			asprintf(&command,"xdg-open \"%s\"",deskIconsArray[foundDiskNumber].mountpoint);
			system(command);
			free(command);
			return;
		}
	else
		{
#ifdef _USESUIDHELPER_
			asprintf(&command,"%s \"%s\" \"/media/%s\" %i",HELPERAPP,deskIconsArray[foundDiskNumber].uuid,deskIconsArray[foundDiskNumber].label,what);
#else
			asprintf(&command,"udevil mount `findfs UUID=%s`",deskIconsArray[foundDiskNumber].uuid);
#endif
			system(command);
			free(command);
			if(what==1)
				{
					asprintf(&command,"findmnt -lno TARGET -S UUID=\"%s\"|xargs xdg-open",deskIconsArray[foundDiskNumber].uuid);
					system(command);
					free(command);
				}

			if(what==3)
				clearDeskEntry(foundDiskNumber,true);
		}
}

int getUSBData(const char *ptr)
{
	if(ptr!=NULL)
		{
			if(strcasestr(ptr,"apple")!=NULL)
				return(IPOD);
			if(strcasestr(ptr,"sandisk")!=NULL)
				return(CARD);
			if(strcasestr(ptr,"generic")!=NULL)
				return(CARD);
		}
	return(USB);
}

/*
void deleteDiskInfo(void)
{
	for(int j=0; j<numberOfDisksAttached; j++)
		{
			freeAttached(j);
//			if(attached[j].uuid!=NULL)
//				free(attached[j].uuid);
//			if(attached[j].label!=NULL)
//				free(attached[j].label);
//			if(attached[j].mountpoint!=NULL)
//				free(attached[j].mountpoint);
//			if(attached[j].dev!=NULL)
//				free(attached[j].dev);
//			if(attached[j].sysname!=NULL)
//				free(attached[j].sysname);
		}
	free(attached);
}

bool getDiskPos(char* uuid,int* xptr,int* yptr)
{
	for (int j=0;j<savedFileCount;j++)
		{
			if(strcmp(uuid,saved[j].uuid)==0)
				{
					*xptr=saved[j].x;
					*yptr=saved[j].y;
					return(true);
				}
		}
	return(false);
}
*/
#if 0
void scanForMountableDisks(void)
{
	struct udev *udev;
	char		buffer[BUFFERSIZE];
	FILE		*fp;
	int			numofdisks=0;
	const char	*ptr;
	int			xpos=0,ypos=0;
	udev_device *usbdev;
	udev_device	*thedev;

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
			for(int j=0;j<numberOfDisksAttached;j++)
				{
					buffer[0]=0;
					fgets(buffer,BUFFERSIZE,fp);
					buffer[strlen(buffer)-1]=0;
					thedev=udev_device_new_from_subsystem_sysname(udev,"block",buffer);
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

											if(udev_device_get_property_value(thedev,"ID_CDROM_MEDIA_CD")!=NULL)
												{
													attached[j].dvd=true;
													attached[j].type=CDROM;
												}

											usbdev=udev_device_get_parent_with_subsystem_devtype(thedev,"usb","usb_device");
											if(usbdev!=NULL)
												{
													attached[j].usb=true;
													attached[j].type=getUSBData(udev_device_get_property_value(thedev,"ID_VENDOR"));
													if(udev_device_get_property_value(thedev,"ID_DRIVE_THUMB")!=NULL)
														attached[j].type=STICK;
												}

											if(getDiskPos(attached[j].uuid,&xpos,&ypos))
												{
													attached[j].x=xpos;
													attached[j].y=ypos;
												}
											else
												{
													xpos=0;
													ypos=0;
													while(xySlot[xpos][ypos]!=0)
														{
															ypos++;
															if(ypos>MAXGRIDY)
																{
																	ypos=0;
																	xpos++;
																}
														}
													attached[j].x=xpos;
													attached[j].y=ypos;
													sprintf(buffer,"%s/%s",diskInfoPath,attached[j].uuid);
													saveInfofile(DISKFOLDER,attached[j].label,NULL,NULL,attached[j].uuid,(char*)iconDiskType[attached[j].type],attached[j].x,attached[j].y);
													xySlot[xpos][ypos]=1;
													getSavedDiskData();
												}
											xySlot[xpos][ypos]=1;
										}
							else
								{
									if(firstRun==false)
										freeAttached(j);
								}
									
								}
							else
								{
									if(firstRun==false)
										freeAttached(j);
								}
							udev_device_unref(thedev);
						}
				}
			pclose(fp);
		}
	udev_unref(udev);
}
#endif
void fillDesk(void)
{
	struct udev		*udev;
	udev_device 	*thedev;
	udev_device 	*usbdev;
	char			buffer[BUFFERSIZE];
	char			path[BUFFERSIZE];
	char			*uuid=NULL;
	int				iconhint=0;
	bool			iscd=false;
	bool			isdvd=false;
	bool			isusb=false;
	FILE			*fp;
	const char		*ptr;

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		printf("Can't create udev\n");
		exit(1);
	}

	deskIconsCnt=1;

	fp=popen("ls -1 /dev/disk/by-uuid","r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			while(fgets(buffer,BUFFERSIZE,fp))
				{
					clearDeskEntry(deskIconsCnt,false);
					buffer[strlen(buffer)-1]=0;
					asprintf(&uuid,"%s",buffer);
					sprintf(path,"/dev/disk/by-uuid/%s",buffer);
					buffer[readlink(path,buffer,BUFFERSIZE)]=0;
					ptr=strrchr(buffer,'/');
					ptr++;
					deskIconsArray[deskIconsCnt].partname=strdup(ptr);
					thedev=udev_device_new_from_subsystem_sysname(udev,"block",ptr);
					if(thedev==NULL)
						{
							printf("no dev for %s from %s\n",ptr,buffer);
							exit(1);
						}
					else
						{
							if(strcmp(udev_device_get_property_value(thedev,"ID_FS_USAGE"),"filesystem")==0)
								{
									ptr=udev_device_get_property_value(thedev,"ID_FS_LABEL");
									if(ptr==NULL)
										ptr=udev_device_get_property_value(thedev,"ID_SERIAL");
									if(ptr==NULL)
										continue;

									//clearDeskEntry(deskIconsCnt);
									//deskIconsArray[deskIconsCnt].partname=temppartname;
									iconhint=0;
									deskIconsArray[deskIconsCnt].label=strdup(ptr);
									deskIconsArray[deskIconsCnt].uuid=uuid;

									isdvd=false;
									iscd=false;
									isusb=false;
									if(udev_device_get_property_value(thedev,"ID_CDROM_MEDIA_DVD")!=NULL)
										{
											iconhint=DVD;
											isdvd=true;
										}
									if(udev_device_get_property_value(thedev,"ID_CDROM_MEDIA_CD")!=NULL)
										{
											iconhint=CDROM;
											iscd=true;
										}
									
									usbdev=udev_device_get_parent_with_subsystem_devtype(thedev,"usb","usb_device");
									if(usbdev!=NULL)
										{
											isusb=true;
											if(udev_device_get_property_value(thedev,"ID_DRIVE_THUMB")!=NULL)
												iconhint=STICK;
											else
												iconhint=getUSBData(udev_device_get_property_value(thedev,"ID_VENDOR"));
										}
									asprintf(&deskIconsArray[deskIconsCnt].dev,"/dev/%s",deskIconsArray[deskIconsCnt].partname);
									deskIconsArray[deskIconsCnt].dvd=isdvd;
									deskIconsArray[deskIconsCnt].cdrom=iscd;
									deskIconsArray[deskIconsCnt].usb=isusb;
									deskIconsArray[deskIconsCnt].file=false;
									deskIconsArray[deskIconsCnt].iconhint=iconhint;
									sprintf(buffer,"%s/%s",diskInfoPath,deskIconsArray[deskIconsCnt].uuid);
									if(loadVarsFromFile(buffer,globalFileData))
										{
											deskIconsArray[deskIconsCnt].x=fileDiskXPos;
											deskIconsArray[deskIconsCnt].y=fileDiskYPos;
										}
									else
										{
											getFreeSlot(&deskIconsArray[deskIconsCnt].x,&deskIconsArray[deskIconsCnt].y);
										}
									deskIconsArray[deskIconsCnt].installed=true;
									xySlot[deskIconsArray[deskIconsCnt].x][deskIconsArray[deskIconsCnt].y]=1;
									deskIconsCnt++;
								}
						}
				}
		}
	pclose(fp);

//desktop files
	sprintf(buffer,"find %s -mindepth 1",desktopPath);
	char	buffer2[4096];
	char	*tptr;
	fp=popen(buffer,"r");
	while(fgets(buffer,BUFFERSIZE,fp))
		{
			char	*ptr;
			buffer[strlen(buffer)-1]=0;
			ptr=strrchr(buffer,'/');
			ptr++;
			sprintf(buffer2,"%s/%s",cachePath,ptr);
			if(fileExists(buffer2)==-1)
				{
					deskIconsArray[deskIconsCnt].mountpoint=strdup(buffer);
					ptr=strrchr(buffer,'/');
					ptr++;
					deskIconsArray[deskIconsCnt].label=strdup(ptr);
					tptr=getMimeType(buffer);
					ptr=strchr(tptr,'/');
					while(ptr!=NULL)
						{
							*ptr='-';
							ptr=strchr(tptr,'/');
						}
					ptr=strstr(tptr,"text-x-shellscript");
					if(ptr==NULL)
						deskIconsArray[deskIconsCnt].mime=strdup(tptr);
					else
						deskIconsArray[deskIconsCnt].mime=strdup("application-x-shellscript");
					free(tptr);
					getFreeSlot(&deskIconsArray[deskIconsCnt].x,&deskIconsArray[deskIconsCnt].y);
					xySlot[deskIconsArray[deskIconsCnt].x][deskIconsArray[deskIconsCnt].y]=1;
					saveInfofile(CACHEFOLDER,deskIconsArray[deskIconsCnt].label,deskIconsArray[deskIconsCnt].mime,deskIconsArray[deskIconsCnt].mountpoint,NULL,NULL,deskIconsArray[deskIconsCnt].x,deskIconsArray[deskIconsCnt].y);
					deskIconsArray[deskIconsCnt].file=true;
					deskIconsCnt++;
				}
			else
				{
					readDesktopFile(ptr);
				}
		}
	pclose(fp);
}