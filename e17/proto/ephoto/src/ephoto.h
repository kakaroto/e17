#ifndef _EPHOTO_H_
#define _EPHOTO_H_

#include <Ewl.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Ecore_X.h>
#include <fnmatch.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "config.h"

void presentation_cb(Ewl_Widget *w, void *event, void *data);
void last_cb(Ewl_Widget *w, void *event, void *data);
void previous_cb(Ewl_Widget *w, void *event, void *data);
void next_cb(Ewl_Widget *w, void *event, void *data);
void first_cb(Ewl_Widget *w, void *event, void *data);
void slideshow_cb(Ewl_Widget *w, void *event, void *data);
int _change_picture_cb(void *data);
void destroyp_cb(Ewl_Widget *w, void *event, void *data);
void destroys_cb(Ewl_Widget *w, void *event, void *data);
void destroy_cb(Ewl_Widget *w, void *event, void *data);
void populatei_cb(Ewl_Widget *w, void *event, void *data);
void populatea_cb(Ewl_Widget *w, void *event, void *data);
void images_cb(Ewl_Widget *w, void *event, void *data);
void audio_cb(Ewl_Widget *w, void *event, void *data);
void up_cb(Ewl_Widget *w, void *event, void *data);
void rad_cb(Ewl_Widget * w, void *event, void *data);
void iremove_cb(Ewl_Widget * w, void *event, void *data);
void realize_cb(Ewl_Widget *w, void *event, void *data);
void play_cb(Ewl_Widget *w, void *event, void *data);
void ephoto_men_cb(Ewl_Widget *w, void *event, void *data);
void destroywin_cb(Ewl_Widget *w, void *event, void *data);
void slideshow_save_cb(Ewl_Widget *w, void *event, void *data);
void save_cb(Ewl_Widget *w, void *event, void *data);
void slideshow_load_cb(Ewl_Widget *w, void *event, void *data);
void loadclicked_cb(Ewl_Widget *w, void *event, void *data);
void load_cb(Ewl_Widget *w, void *event, void *data);

typedef struct _Main Main;
typedef struct _Slide Slide;
typedef struct _Present Present;
struct _Main
{
	Ewl_Widget	*win;
	Ewl_Widget	*save_win;
	Ewl_Widget	*load_win;
	Ewl_Widget	*scrollpane1;
	Ewl_Widget	*hbox;
	Ewl_Widget	*hboxv;
	Ewl_Widget	*vbox;
	Ewl_Widget	*vbox2;
	Ewl_Widget  	*content;
	Ewl_Widget  	*images;
	Ewl_Widget  	*songs;
	Ewl_Widget  	*directory;
	Ewl_Widget  	*directorya;
	Ewl_Widget  	*audiotree;
	Ewl_Widget  	*imagetree;
	Ewl_Widget  	*image;
	Ewl_Widget  	*children[2];
	Ewl_Widget  	*row;
	Ewl_Widget	*ib;
	Ewl_Widget	*iscroll;
	Ewl_Widget	*slideshow;
	Ewl_Widget	*presentation;
	Ewl_Widget  	*text;
	Ewl_Widget	*slidetime;
	Ewl_Widget  	*settings;
	Ewl_Widget  	*wsize;
	Ewl_Widget  	*hsize;
	Ewl_Widget  	*atext;
	Ewl_Widget  	*texta;
	Ewl_Widget  	*texti;
	Ewl_Widget	*otext;
	Ewl_Widget  	*rad1;
	Ewl_Widget	*rad2;
	Ewl_Widget	*rad3;
	Ewl_Widget	*rad4;
	Ewl_Widget  	*rad5;
	Ewl_Widget  	*fullrad;
	Ewl_Widget  	*audiolen;
	Ewl_Widget  	*loopcheck;
	Ewl_Widget 	*i;
	Ewl_Widget  	*notebook;
	Ewl_Widget  	*vimage;
	Ewl_Widget  	*viewbox;
	Ewl_Widget  	*vbutton;
	Ewl_Widget  	*viewscroll;
	Ewl_Widget      *dirtree;
	Ewl_Widget      *spacer;
	Ewl_Widget	*menubar;
	Ewl_Widget	*menu;
	Ewl_Widget	*menu_item;
	Ewl_Widget	*avbox;
	void		*data;
	Ecore_List  	*imagelist;
	Ecore_List  	*presentlist;
};

struct _Slide
{
	Ewl_Widget	*audio1;
	Ewl_Widget	*wins;
	Ewl_Widget	*vboxs;
	Ewl_Widget	*hboxs;
	Ewl_Widget	*screen;
	Ewl_Widget  	*white;
	Ewl_Widget  	*black;
	Ewl_Widget  	*text;
	Ewl_Widget  	*cell;
	Ecore_Timer	*timer;
};

struct _Present
{
	Ewl_Widget 	*winp;
	Ewl_Widget 	*vbox1p;
	Ewl_Widget 	*vboxp;
	Ewl_Widget 	*vbox2p;
	Ewl_Widget 	*imagep;
	Ewl_Widget 	*hboxp;
	Ewl_Widget 	*next;
	Ewl_Widget 	*previous;
	Ewl_Widget 	*first;
	Ewl_Widget 	*last;
};
	
extern Main           *m;
extern Slide          *s;
extern Present        *p;
extern Ecore_List *files;
extern Ecore_List *imagefiles;
extern Ecore_List *audiofiles;

extern int audio;
extern int wino ;
extern int audiolen;
extern int mainwin;
extern int slidenum;
extern int arglength;
extern int argfit;
extern int argfullscreen;
extern int argloop;
extern char argwidth[PATH_MAX];
extern char argheight[PATH_MAX];
extern char *audios;

#endif
