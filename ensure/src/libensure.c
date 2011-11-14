#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>


#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Evas.h>


#define __UNUSED__	__attribute__((unused))

/* The color of the highlight */
enum {
	HIGHLIGHT_R = 255,
	HIGHLIGHT_G = 128,
	HIGHLIGHT_B = 128,
	HIGHLIGHT_A = 255,

	/* How much padding around the highlight box */
	PADDING = 2,
};

static Eina_Bool libensure_dump(void);
static Eina_Bool libensure_command_recv(void *data __UNUSED__, Ecore_Fd_Handler *fdh);
static void libensure_highlight(uintptr_t addr);
static Eina_Bool libensure_highlight_hide(void *rv);

static void libensure_objdump(Evas_Object *o, Evas_Object *parent);

static FILE *outfile;

static int verbose = -1;

__attribute__((constructor)) void
libensure_init(void){
	int sendfd, commandfd;
	const char *p;

	if (verbose == -1)
		verbose = !!getenv("LIBENSURE_DEBUG");

	ecore_init();

	p = getenv("ENSURE_FD");
	if (p){
		if (verbose) fprintf(stderr,"ENSURE_FD=%s",p);
		sscanf(p, "%d:%d", &sendfd, &commandfd);
		if (verbose) fprintf(stderr,"LibEnsure: Send: %d Command %d!\n",
				sendfd, commandfd);
		outfile = fdopen(sendfd, "w");
	} else {
		fprintf(stderr,"Warning: No send/command fd\n");
		exit(0);
	}

	ecore_main_fd_handler_add(commandfd,ECORE_FD_READ|ECORE_FD_ERROR,
			libensure_command_recv, NULL,
			NULL, NULL);

	if (!outfile) {
		if(verbose) fprintf(stderr,"LibEnsure: Warning using stdout\n");
		outfile = stdout;
	}
	fprintf(outfile,"ensure init\n");
	if (verbose) fprintf(stderr, "LibEnsure: Init done\n");

	return;
}

static Eina_Bool
libensure_command_recv(void *data __UNUSED__, Ecore_Fd_Handler *fdh){
	char buf[100];
	char *save;
	int fd;

	fd = ecore_main_fd_handler_fd_get(fdh);

	if (read(fd, buf, sizeof(buf)) != sizeof(buf)){
		perror("read");
		return true;
	}
	strtok_r(buf, "\n\r", &save);

	if (strncmp(buf, "Check", 5) == 0){
		libensure_dump();
	} else if (strncmp(buf, "Display:", 8) == 0){
		uintptr_t id = strtoul(buf + 8, NULL, 16);
		libensure_highlight(id);
	} else {
		printf("Unknown command: %s\n",buf);
	}

	return true;
}


static Eina_Bool
libensure_dump(void){
	Eina_List *os,*ot;
	Evas_Object *o;
	Eina_List *ees,*l;
	Ecore_Evas *ee;
printf("libensure dump called\n");

	fprintf(outfile,"Ensure dump!\n");
	if (verbose) fprintf(stderr,"Starting ensure dump\n");

	ees = ecore_evas_ecore_evas_list_get();

	EINA_LIST_FOREACH(ees, l, ee){
		Evas *e;
		const char *name;
		int w,h;
		e = ecore_evas_get(ee);
		evas_output_size_get(e,&w,&h);
		name = ecore_evas_title_get(ee);
		if (!name) name = "";
		if (verbose) fprintf(stderr,"E: %p '%s' %d %d\n",e,name,w,h);
		fprintf(outfile,"E: %p '%s' %d %d\n",e,name,w,h);
		os = evas_objects_in_rectangle_get(e,SHRT_MIN, SHRT_MIN,
				USHRT_MAX, USHRT_MAX, true, true);
		EINA_LIST_FOREACH(os, ot, o){
			libensure_objdump(o, NULL);
		}

	}

	fprintf(outfile,"Ensure done\n");
	fflush(outfile);

	return 1;
}

static void
libensure_objdump(Evas_Object *o, Evas_Object *parent){
	Eina_List *children, *tmp;
	Evas_Object *child,*clip;
	const char *type;
	int x,y,w,h,a,r,g,b;

	type = evas_object_type_get(o);
	fprintf(outfile,"Object: %p '%s' ",o,type);
	if (verbose) fprintf(stderr,"Object: %p '%s'\n",o,type);

//	type = evas_object_name_get(o);
//	if (type) fprintf(outfile, "Name: '%s' ",type);

	if (parent) fprintf(outfile,"Parent: %p ",parent);
	evas_object_geometry_get(o,&x,&y,&w,&h);
	fprintf(outfile,"Geometry: %+d%+d(%dx%d) ",x,y,w,h);
	clip = evas_object_clip_get(o);
	if (clip) fprintf(outfile, "Clip: %p ",parent);
	evas_object_color_get(o, &r,&g,&b,&a);
	fprintf(outfile, "Color (%d,%d,%d,%d) ",r,g,b,a);

	/* Type specific things here */
	if (strcmp(type, "image") == 0){
		const char *file,*key;
		Evas_Object *src;
		int err;
		evas_object_image_file_get(o,&file,&key);
		if (key)
			fprintf(outfile, "Image: '%s' '%s' ",file,key);
		else
			fprintf(outfile, "Image: '%s' ",file);
		err = evas_object_image_load_error_get(o);
		if (err != EVAS_LOAD_ERROR_NONE){
			fprintf(outfile, "ImageErr: '%s' ",
					evas_load_error_str(err));
		}
		src = evas_object_image_source_get(o);
		if (src)
			fprintf(outfile, "ImageSrc: '%p' ",src);
	} else if (strcmp(type, "text") == 0){
		int size;
		const char *font;
		fprintf(outfile,"Text: [[%s]] ",evas_object_text_text_get(o));
		evas_object_text_font_get(o, &font,&size);
		fprintf(outfile,"Font: '%s' %d ",font,size);
		font = evas_object_text_font_source_get(o);
		if (font) fprintf(outfile,"FontSource: '%s' %d ",font,size);
	} else if (strcmp(type, "rectangle") == 0){

	} else if (strcmp(type, "edje") == 0){
		const char *file, *group;
		int err;
		edje_object_file_get(o,&file,&group);
		fprintf(outfile, "Edje: '%s' '%s' ",file,group);
		err = edje_object_load_error_get(o);
		if (err != EDJE_LOAD_ERROR_NONE){
			fprintf(outfile,"EdjeErr: '%s' ",
					edje_load_error_str(err));
		}
	}


	fprintf(outfile,"\n");

	children = evas_object_smart_members_get(o);
	EINA_LIST_FOREACH(children, tmp, child){
		libensure_objdump(child,o);
	}

}

__attribute__((destructor)) void
libensure_fini(void){
	ecore_shutdown();
}

/**
 * Handler to highlight an object on the canvas.
 *
 * The current implementation shows a reddish rectangle which slowly fades
 * away.
 */
static void
libensure_highlight(uintptr_t addr){
	Evas *e;
	Evas_Object *obj;
	Evas_Object *r;
	int x,y,w,h;

	/* FIXME: Should check the object is valid */
	obj = (void *)addr;

	e = evas_object_evas_get(obj);
	if (!e) return;

	evas_object_geometry_get(obj,&x,&y,&w,&h);

	r = evas_object_rectangle_add(e);
	evas_object_move(r,x-PADDING,y - PADDING);
	evas_object_resize(r,w + 2*PADDING, h + 2*PADDING);
	evas_object_color_set(r,HIGHLIGHT_R,HIGHLIGHT_G,HIGHLIGHT_B,
			HIGHLIGHT_A);
	evas_object_show(r);
	ecore_timer_add(0.1, libensure_highlight_hide, r);
}

/* Slowly fade the object out */
static Eina_Bool
libensure_highlight_hide(void *rv){
	int r,g,b,a;
	double na;

	evas_object_color_get(rv,&r,&g,&b,&a);
	if (a < 20){
		evas_object_del(rv);
		return false;
	}

	na = a - 20;
	r = na / a * r;
	g = na / a * g;
	b = na / a * b;
	evas_object_color_set(rv,r,g,b,na);

	return true;
}
