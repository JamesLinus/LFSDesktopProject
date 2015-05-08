/******************************************************
*
*     ©keithhedger Mon  4 May 13:20:31 BST 2015
*     kdhedger68713@gmail.com
*
*     files.cpp
* 
******************************************************/

#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#include <Imlib2.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "prefs.h"

char	*diskInfoPath;
char	*cachePath;
Pixmap	diskIconsPixmap[20][2];
Pixmap	diskIconsPixmapMask[20][2];

int fileExists(char *name)
{
	struct stat buffer;
	//printf("------%s------\n",name);
	return (stat(name,&buffer));
}

void makeImage(char *imagepath,char *destname,int disktype)
{
	Imlib_Image					image;
	Pixmap						diskpixmap,diskpixmapmask;
	XRenderPictFormat			*format;
	XRenderPictFormat			*xformat;
	Picture						src_pic;
	Picture						dst_pic;
	Pixmap						alphapixmap;
	Picture						alphapicture;
	XRenderPictFormat			rpf;
	XRenderPictureAttributes	rpa;
	XRenderColor				clr;
	Pixmap						destpixmap;
	Imlib_Image					imfinal;
	int							w,h;
	char						*destpath;

	image=imlib_load_image(imagepath);
	if (image)
		{
			imlib_context_set_image(image);
			imlib_image_set_has_alpha(1);
			imlib_context_set_display(display);
			w=imlib_image_get_width();
			h=imlib_image_get_height();
			imlib_context_set_visual(visual);
			imlib_context_set_drawable(rootWin);
			imlib_context_set_anti_alias(1);
			imlib_render_pixmaps_for_whole_image(&diskpixmap,&diskpixmapmask);

			asprintf(&destpath,"%s/%s.png",cachePath,destname);
			if(fileExists(destpath)!=0)
				imlib_save_image(destpath);

			imlib_render_pixmaps_for_whole_image_at_size(&diskIconsPixmap[disktype][0],&diskIconsPixmapMask[disktype][0],ICONSIZE,ICONSIZE);
			free(destpath);
			imlib_free_image();

			rpf.type=PictTypeDirect;
			rpf.depth=8;
			rpf.direct.alphaMask=0xff;
			rpa.repeat=True;
			clr.alpha=((unsigned short)((255*128) << 8));
			clr.red=0x8000;
			clr.green=0x8000;
			clr.blue=0x8000;
			xformat=XRenderFindFormat(display,PictFormatType | PictFormatDepth | PictFormatAlphaMask,&rpf,0);
			alphapixmap=XCreatePixmap(display,rootWin,1,1,8);
			alphapicture=XRenderCreatePicture(display,alphapixmap,xformat,CPRepeat,&rpa);

			XRenderFillRectangle(display,PictOpSrc,alphapicture,&clr,0,0,1,1);
			format=XRenderFindVisualFormat(display,visual);

			xformat=XRenderFindStandardFormat(display,PictStandardARGB32);
			if(!xformat)
				printf("No argb\n");
			else
				printf("got argb\n");
			destpixmap=XCreatePixmap(display,XDefaultRootWindow(display),w,h,32);

			src_pic=XRenderCreatePicture(display,diskpixmap,format,0,0);
			dst_pic=XRenderCreatePicture(display,destpixmap,xformat,0,0);
			XRenderFillRectangle(display,PictOpSrc,dst_pic,&clr,0,0,w,h);
			XRenderComposite(display,PictOpOver,src_pic,alphapicture,dst_pic,0,0,0,0,0,0,w,h);

			imlib_context_set_drawable(destpixmap);
			imfinal=imlib_create_image_from_drawable(diskpixmapmask,0,0,w,h,1);
			
			imlib_context_set_image(imfinal);
			asprintf(&destpath,"%s/%s-offline.png",cachePath,destname);
			if(fileExists(destpath)!=0)
				imlib_save_image(destpath);

			imlib_render_pixmaps_for_whole_image_at_size(&diskIconsPixmap[disktype][1],&diskIconsPixmapMask[disktype][1],ICONSIZE,ICONSIZE);

			free(destpath);
		}
}
