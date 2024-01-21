/* $Id: demo.c 15 2023-12-31 11:35:50Z nishi $ */
/* Demo */
#include "ppmlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/Label.h>
#include <X11/Xaw/Viewport.h>

int w, h, type;
double scale;
bool made = false;
Pixmap img;
Widget top;
GC gc;

void data_process(int x, int y, int r, int g, int b){
	if(!made){
		img = XCreatePixmap(XtDisplay(top), DefaultRootWindow(XtDisplay(top)), w * scale, h * scale, 24);
		made = true;
		Widget vp = XtVaCreateWidget("viewport", viewportWidgetClass, top,
			XtNallowHoriz, True,
			XtNallowVert, True,
			XtNuseRight, True,
			XtNuseBottom, True,
		NULL);
		Widget a = XtVaCreateWidget("label", labelWidgetClass, vp,
			XtNbitmap, img,
		NULL);
		XtManageChild(vp);
		XtManageChild(a);
		XtRealizeWidget(top);
	}
	XSetForeground(XtDisplay(top), gc, (r << 16) | (g << 8) | (b));
	XFillRectangle(XtDisplay(top), img, gc, x * scale, y * scale, scale > 1 ? (scale + 1) : 1, scale > 1 ? (scale + 1) : 1);
}

int main(int argc, char** argv){
	if(argc < 2){
		fprintf(stderr, "%s file\n", argv[0]);
		return 1;
	}
	struct stat s;
	if(stat(argv[1], &s) == 0){
		char* data = malloc(s.st_size);
		FILE* f = fopen(argv[1], "r");
		fread(data, s.st_size, 1, f);
		fclose(f);
		scale = atof(argv[2] ? argv[2] : "1");
		XtAppContext ctx;
		top = XtVaAppInitialize(&ctx, "Demo", NULL, 0, &argc, argv, NULL,
		NULL);
		gc = XCreateGC(XtDisplay(top), DefaultRootWindow(XtDisplay(top)), 0, 0);
		XSetGraphicsExposures(XtDisplay(top), gc, True);
		ppmlib_parse(data, s.st_size, &w, &h, &type, NULL, data_process);
		if(made){
			printf("Type %d\n", type);
		}else{
			Widget a = XtVaCreateWidget("label", labelWidgetClass, top,
				XtNlabel, "Could not load the image",
			NULL);
			XtManageChild(a);
		}
		XtAppMainLoop(ctx);
	}else{
		XtAppContext ctx;
		Widget top = XtVaAppInitialize(&ctx, "Demo", NULL, 0, &argc, argv, NULL, NULL);
		Widget a = XtVaCreateWidget("label", labelWidgetClass, top,
			XtNlabel, "Could not open the image",
		NULL);
		XtManageChild(a);
		XtRealizeWidget(top);
		XtAppMainLoop(ctx);
	}
	return 0;
}
