#include <limits.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/signalfd.h>
#include <unistd.h>


#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Evas.h>


#define ensure_unused	__attribute__((unused))

static Eina_Bool libensure_dump(void);
static Eina_Bool ecore_signal(void *data, int tyoe, void *event);
static Eina_Bool ecore_fd_signal(void *data ensure_unused, Ecore_Fd_Handler *fdh);

static void libensure_objdump(Evas_Object *o, Evas_Object *parent);

static FILE *outfile;

static int verbose = -1;

__attribute__((constructor)) void
libensure_init(void){
	int fd;
	int sendfd;
	const char *p;
	sigset_t sigusr2;

	if (verbose == -1)
		verbose = !!getenv("LIBENSURE_DEBUG");

	ecore_init();

	sigemptyset(&sigusr2);
	sigaddset(&sigusr2, SIGUSR2);

	fd = signalfd(-1, &sigusr2, SFD_CLOEXEC | SFD_NONBLOCK);
	if (fd == -1){
		perror("signalfd");
		exit(1);
	}
	if (verbose) fprintf(stderr,"Signal fd is %d\n",fd);
	ecore_main_fd_handler_add(fd,ECORE_FD_READ|ECORE_FD_ERROR,
			ecore_fd_signal, NULL,
			NULL, NULL);

	ecore_event_handler_add(ECORE_EVENT_SIGNAL_USER, ecore_signal, NULL);

	p = getenv("ENSURE_FD");
	if (p){
		sendfd = strtol(p, NULL, 0);
		if (verbose) fprintf(stderr,"LibEnsure: Using fd %d!\n",sendfd);
		outfile = fdopen(sendfd, "w");
	}

	if (!outfile) {
		if(verbose) fprintf(stderr,"LibEnsure: Warning using stdout\n");
		outfile = stdout;
	}
	fprintf(outfile,"ensure init\n");
	if (verbose) fprintf(stderr, "LibEnsure: Init done\n");

	return;
}

static Eina_Bool
ecore_signal(void *data, int tyoe, void *event){
	Ecore_Event_Signal_User *user = event;
	if (!event || user->number != 2) return 1;

	libensure_dump();

	return 0;
}

static Eina_Bool
ecore_fd_signal(void *data ensure_unused, Ecore_Fd_Handler *fdh){
	struct signalfd_siginfo siginfo;
	int fd;

	fd = ecore_main_fd_handler_fd_get(fdh);

	read(fd, &siginfo, sizeof(struct signalfd_siginfo));

	libensure_dump();
	return 0;
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
