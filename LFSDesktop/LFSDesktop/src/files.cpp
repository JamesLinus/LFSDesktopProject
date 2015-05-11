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
#include <search.h>

#include "prefs.h"
#include "graphics.h"

char	*diskInfoPath;
char	*cachePath;
char	*prefsPath;

struct hsearch_data	hashtab;

int fileExists(char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}

char* pathToIcon(char* name)
{
	char	*command;
	FILE	*fp;
	char	buffer[2048];
	char	*retstr=NULL;

	asprintf(&command,"find /usr/share/icons/%s %s/.icons/%s -iname \"*%s*\"  2>/dev/null|sort -nr -t \"x\"  -k 2.1|head -n1",iconTheme,getenv("HOME"),iconTheme,name);

	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			fgets(buffer,2048,fp);
			sscanf(buffer,"%as",&retstr);
			printf(">>%s--%s<<\n",name,retstr);
			pclose(fp);
		}
	return(retstr);
}
	Pixmap						pmhash,pmhashmask;

void makeImage(char *imagepath,char *destname,diskIconStruct *hashdata)
{
	 hashdata->cairoImage=cairo_image_surface_create_from_png(imagepath);
}

void makeImageXXXXX(char *imagepath,char *destname,diskIconStruct *hashdata)
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
	Pixmap						pmhash,pmhashmask;

	image=imlib_load_image(imagepath);
	if (image)
		{
			imlib_context_set_image(image);
			//imlib_image_set_has_alpha(1);
			imlib_context_set_display(display);
			imlib_context_set_visual(visual);
			w=imlib_image_get_width();
			h=imlib_image_get_height();
			imlib_context_set_drawable(rootWin);
			imlib_context_set_anti_alias(1);
			imlib_context_set_mask_alpha_threshold(1);
			//imlib_context_set_dither(0);
			//imlib_context_set_dither_mask(0);
			imlib_render_pixmaps_for_whole_image_at_size(&diskpixmap,&diskpixmapmask,iconSize,iconSize);

			if(hashdata!=NULL)
				{
					Imlib_Image	image2;
					Pixmap		pm,pmmask;
					Pixmap		pmhash,pmhashmask;
					GC gc1;

					image2=imlib_load_image_immediately_without_cache(imagepath);
					if (image2)
						{
							printf("got image2 >>%s<<\n",imagepath);
							imlib_context_set_image(image2);
							imlib_context_set_display(display);
							imlib_context_set_visual(visual);

							pm=XCreatePixmap(display,drawOnThis,iconSize,iconSize,32);
							gc1=XCreateGC(display,pm,0,0);
							XSetBackground(display,gc1,0x0);
							XSetForeground(display,gc1,0x0);
							XFillRectangle(display,pm,gc1,0,0,iconSize,iconSize);
							imlib_context_set_drawable(pm);
							imlib_context_set_blend(1);
							imlib_context_set_mask_alpha_threshold(1);	
							imlib_render_pixmaps_for_whole_image_at_size(&pmhash,&pmhashmask,iconSize,iconSize);

							imlib_context_set_mask(pmhashmask);
							imlib_render_image_on_drawable_at_size(0,0,iconSize,iconSize);
							hashdata->pixmap=pm;
							hashdata->mask=pmhashmask;
							hashdata->gc=gc1;
						}
				
				//	imlib_render_pixmaps_for_whole_image_at_size(&pmhash,&pmhashmask,iconSize,iconSize);
				//	hashdata->pixmap=pmhash;
				//	hashdata->mask=pmhashmask;
				////	hashdata->image=image;
				}

			imlib_context_set_mask_alpha_threshold(1);
			imlib_render_pixmaps_for_whole_image(&diskpixmap,&diskpixmapmask);

//			if(hashdata!=NULL)
//				{
//					imlib_render_pixmaps_for_whole_image_at_size(&pmhash,&pmhashmask,iconSize,iconSize);
//					hashdata->pixmapOffline=pmhash;
//					hashdata->maskOffline=pmhashmask;
//				//	hashdata->image=image;
//				}



			//imlib_free_image();

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
			imlib_image_set_has_alpha(1);
			imlib_context_set_display(display);
			//w=imlib_image_get_width();
			//h=imlib_image_get_height();
			imlib_context_set_visual(visual);
			imlib_context_set_drawable(rootWin);
			imlib_context_set_anti_alias(1);
			imlib_context_set_mask_alpha_threshold(190);	

			if(hashdata!=NULL)
				{
					imlib_render_pixmaps_for_whole_image_at_size(&pmhash,&pmhashmask,iconSize,iconSize);
					hashdata->pixmapOffline=pmhash;
					hashdata->maskOffline=pmhashmask;
				}


		}
}




void makeImageXXX(char *imagepath,char *destname,diskIconStruct *hashdata)
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
	Pixmap						pmhash,pmhashmask;

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
			imlib_context_set_mask_alpha_threshold(7);
			//imlib_context_set_dither(0);
			//imlib_context_set_dither_mask(0);
			imlib_render_pixmaps_for_whole_image(&diskpixmap,&diskpixmapmask);

			if(hashdata!=NULL)
				{
					imlib_render_pixmaps_for_whole_image_at_size(&pmhash,&pmhashmask,iconSize,iconSize);
					hashdata->pixmap=pmhash;
					hashdata->mask=pmhashmask;
				//	hashdata->image=image;
				}

			imlib_context_set_mask_alpha_threshold(1);
			imlib_render_pixmaps_for_whole_image(&diskpixmap,&diskpixmapmask);

//			if(hashdata!=NULL)
//				{
//					imlib_render_pixmaps_for_whole_image_at_size(&pmhash,&pmhashmask,iconSize,iconSize);
//					hashdata->pixmapOffline=pmhash;
//					hashdata->maskOffline=pmhashmask;
//				//	hashdata->image=image;
//				}



			//imlib_free_image();

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
			imlib_image_set_has_alpha(1);
			imlib_context_set_display(display);
			//w=imlib_image_get_width();
			//h=imlib_image_get_height();
			imlib_context_set_visual(visual);
			imlib_context_set_drawable(rootWin);
			imlib_context_set_anti_alias(1);
			imlib_context_set_mask_alpha_threshold(190);	

			if(hashdata!=NULL)
				{
					imlib_render_pixmaps_for_whole_image_at_size(&pmhash,&pmhashmask,iconSize,iconSize);
					hashdata->pixmapOffline=pmhash;
					hashdata->maskOffline=pmhashmask;
				}


		}
}