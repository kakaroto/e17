#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <Edje.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Eet.h>
#include <E_Lib.h>
#include <sys/stat.h>

#include "E_Preview.h"

typedef struct _e_preview E_Preview; 
struct _e_preview {

	Evas_Coord x, y, w, h;	

	char	* theme;
	char	* themename;

	/* preview objects */
	Evas_Object	* wallpaper;
	Evas_Object	* window;
	Evas_Object	* clock;
	Evas_Object	**pager;
	Evas_Object	* start;

}; 


static Evas_Smart * _e_preview_smart_get();
static Evas_Object * _e_preview_new(Evas *);
static void e_preview_init(Evas_Object *);
static void _e_preview_redraw(Evas_Object *);
static void _e_preview_add(Evas_Object *);
static void _e_preview_del(Evas_Object *);
static void _e_preview_move(Evas_Object *, Evas_Coord, Evas_Coord);
static void _e_preview_resize(Evas_Object *, Evas_Coord, Evas_Coord);
static void _e_preview_show(Evas_Object *);
static void _e_preview_hide(Evas_Object *);
static void _e_preview_clip_set(Evas_Object *, Evas_Object *);

static char * _e_thumb_file_id(char *file);


Evas_Object * e_preview_new(Evas *evas) {

	Evas_Object * preview = NULL;
	E_Preview * data = NULL;

	if ((preview = _e_preview_new(evas))) {
		if ((data = evas_object_smart_data_get(preview)))
			return preview;
		else
			evas_object_del(preview);
	}

	return NULL;
}


static Evas_Object * _e_preview_new(Evas * evas) {

	Evas_Object * e_preview_object;

	e_preview_object = evas_object_smart_add(evas, _e_preview_smart_get());

	return e_preview_object;
}


static Evas_Smart * _e_preview_smart_get() {

	static Evas_Smart * smart = NULL;

	if (smart) 
		return smart;

	smart = evas_smart_new(
			"e_preview",
			_e_preview_add,
			_e_preview_del,
			NULL, NULL, NULL, NULL, NULL,
			_e_preview_move,
			_e_preview_resize,
			_e_preview_show,
			_e_preview_hide,
			NULL, 
			_e_preview_clip_set,
			NULL,
			NULL
			);
	return smart;
}

static void _e_preview_add(Evas_Object *o) {

	E_Preview * data = NULL;
	Evas * evas = NULL;

	evas = evas_object_evas_get(o);

	data = (E_Preview *)malloc(sizeof(E_Preview));
	memset(data, 0, sizeof(E_Preview));
	
	data->x = 0;
	data->y = 0;
	data->w = 0;
	data->h = 0;

	data->wallpaper = edje_object_add(evas);
	data->window    = edje_object_add(evas);
	data->clock     = edje_object_add(evas);
	data->start     = edje_object_add(evas);
	data->pager     = calloc(3, sizeof(Evas_Object *));
	data->pager[0]  = edje_object_add(evas);
	data->pager[1]  = edje_object_add(evas);
	data->pager[2]  = edje_object_add(evas);

	data->theme     = NULL;
	data->themename = NULL;
	
	evas_object_smart_data_set(o, data);
}

static void _e_preview_del(Evas_Object *o) {
	E_Preview * data;

	if ((data = evas_object_smart_data_get(o))) {
		evas_object_del(data->wallpaper);
		evas_object_del(data->window);
		evas_object_del(data->clock);
		evas_object_del(data->start);
		evas_object_del(data->pager[0]);
		evas_object_del(data->pager[1]);
		evas_object_del(data->pager[2]);
		free(data->pager);
		free(data->theme);
		free(data->themename);
		free(data);
	}
}

static void _e_preview_move(Evas_Object *o, Evas_Coord x, Evas_Coord y) {
	E_Preview *data;

	if ((data = evas_object_smart_data_get(o))) {
		data->x = x;
		data->y = y;
		_e_preview_redraw(o);
	}
}

static void _e_preview_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h) {
	E_Preview *data;

	if ((data = evas_object_smart_data_get(o))) {

		data->w = w;
		data->h = h;

		_e_preview_redraw(o);
		
	}
}

static void _e_preview_show(Evas_Object *o) {
	E_Preview *data;

	if ((data = evas_object_smart_data_get(o))) {
		evas_object_show(data->wallpaper);
		evas_object_show(data->window);
		evas_object_show(data->clock);
		evas_object_show(data->start);
		evas_object_show(data->pager[0]);
		evas_object_show(data->pager[1]);
		evas_object_show(data->pager[2]);
	}
}

static void _e_preview_hide(Evas_Object *o) {
	E_Preview *data;

	if ((data = evas_object_smart_data_get(o))) {
		evas_object_hide(data->wallpaper);
		evas_object_hide(data->window);
		evas_object_hide(data->clock);
		evas_object_hide(data->start);
		evas_object_hide(data->pager[0]);
		evas_object_hide(data->pager[1]);
		evas_object_hide(data->pager[2]);
	}
}

int e_preview_is_theme(Evas * evas, const char * theme) {

	char * file = malloc(sizeof(char) * PATH_MAX);
	snprintf(file, PATH_MAX, "%s/.e/e/themes/%s", getenv("HOME"), theme);

	Evas_Object * o = NULL;
	o = edje_object_add(evas);
	edje_object_file_set(o, file, "widgets/border/default/border");
	if (edje_object_part_exists(o, "widgets/border/default/border")) 
		return 1;

	return 0;
			 
}

void e_preview_theme_set(Evas_Object *object, const char * theme) {

	E_Preview *data;
	Evas * evas = NULL;
	
	evas = evas_object_evas_get(object);

	if ((data = evas_object_smart_data_get(object))) {

		int size = strlen(theme) - 4;
		data->themename = malloc(sizeof(char) * size);
		data->themename = strncpy(data->themename,
				theme, size);
		data->themename[size] = 0;


		data->theme = malloc(sizeof(char) * PATH_MAX);
		snprintf(data->theme, PATH_MAX,
				"%s/.e/e/themes/%s",
				getenv("HOME"), theme);

		/* check if we should try to fall back */
		struct stat status;
		if (stat(data->theme, &status)) {
			snprintf(data->theme, PATH_MAX,
					"%s/share/enlightenment/data/themes/%s",
					E17PREFIX, theme);
			/* I should check here too and fail */
		}

		/* set the theme file to the edjes */
		edje_object_file_set(data->wallpaper,
				     data->theme,
				     "desktop/background");

		edje_object_file_set(data->window,
				     data->theme,
				     "widgets/border/default/border");
		
		if (! data->clock ) data->clock = edje_object_add(evas);
		if (! edje_object_file_set(data->clock, data->theme, "modules/clock/main")) {
			evas_object_del(data->clock);
			data->clock = NULL;
		}

		if (! data->start ) data->start = edje_object_add(evas);
		if (! edje_object_file_set(data->start, data->theme, "modules/start/main")) {
			evas_object_del(data->start);
			data->start = NULL;
		}

		if (edje_object_file_set(data->pager[0], data->theme, "modules/pager/main")) {
			edje_object_file_set(data->pager[1], data->theme, "modules/pager/desk");
			edje_object_file_set(data->pager[2], data->theme, "modules/pager/window");
		}

		e_preview_init(object);
	}

}

void __emit(void * d, Evas * e, Evas_Object * o, void * ev) { 

	char * sig;
	sig = (char *) d;
	edje_object_signal_emit(o, sig, "");

}


static void e_preview_init(Evas_Object *o) {
	E_Preview *data;

	if ((data = evas_object_smart_data_get(o))) {
	
		edje_object_part_text_set(data->window, 
				"title_text",
				data->themename);
		
/* this could be used for more gadgets. */
#define EMIT(A, B, C)\
	evas_object_event_callback_add(A, EVAS_CALLBACK_MOUSE_IN , __emit, strdup(B));\
	evas_object_event_callback_add(A, EVAS_CALLBACK_MOUSE_OUT, __emit, strdup(C));
	
		EMIT(data->window, "active", "passive");

#undef EMIT

		edje_object_signal_emit(data->pager[1], "load", "");
		
		edje_object_part_swallow(data->pager[0],
				"items", data->pager[1]);

		edje_object_part_swallow(data->pager[1],
				"items", data->pager[2]);
	}
	
}

static void _e_preview_redraw(Evas_Object *o) {
	E_Preview *data;

	if ((data = evas_object_smart_data_get(o))) {

		if (data->theme == NULL) return;

		/* wallpaper */
		evas_object_move(data->wallpaper, data->x, data->y);
		evas_object_resize(data->wallpaper, data->w, data->h);

		/* main window */
		evas_object_move(data->window, 
				data->x + (data->w * 0.1),
				data->y + (data->h * 0.05));
		evas_object_resize(data->window, 
				data->w * 0.8,
				data->h * 0.75);

		/* clock */
		if (data->clock) {
			evas_object_move(data->clock, 
				data->x + (data->w * 0.9),
				data->y + (data->h * 0.9));
			evas_object_resize(data->clock,
				data->w * 0.1,
				data->h * 0.1);
		}
		
		/* start */
		if (data->start) {
			evas_object_move(data->start,
				data->x, data->y + (data->h * 0.9));
			evas_object_resize(data->start,
				data->w * 0.1, data->h * 0.1);
		}
		
		/* pager */
		evas_object_move(data->pager[0], 
				data->x + (data->w * 0.3),
				data->y + (data->h * 0.9));
		evas_object_resize(data->pager[0],
				data->w * 0.1,
				data->h * 0.1);

	}
}

int e_preview_thumb_image(const char * thumb, Evas_Object * obj) {


	Eet_File * ef;
	ef = eet_open(thumb, EET_FILE_MODE_READ);
	if (ef) {
		int * pixels;
		pixels = (int *) eet_data_image_read(ef, "/thumbnail/data", 
				NULL, NULL, NULL, NULL, NULL, NULL);
		evas_object_image_data_set(obj, pixels);
		eet_close(ef);
	}

	return 1;

}

int e_preview_thumb_init(void) {

	char  path[PATH_MAX];
	snprintf(path, sizeof(path), "%s/.e/e/fileman/thumbnails", getenv("HOME"));
	if (!ecore_file_exists(path))
		ecore_file_mkpath(path);
	else
		return 0;
	
	return 1;

}

int e_preview_thumb_check(const char * theme) {

	struct stat themestatus;
	struct stat thumbstatus;
	char themefile[PATH_MAX];
	snprintf(themefile, PATH_MAX, "%s/.e/e/themes/%s", getenv("HOME"), theme);

	char * thumb = e_preview_thumb_file_get(theme);

	if (stat(themefile, &themestatus) == 0) {
		if (stat(thumb, &thumbstatus) == 0) {
			if (themestatus.st_mtime > thumbstatus.st_mtime) /* not uptodate */
				return 1;
		} else {
			return 1;
		}
	}
		

	
	return 0;
}

char * e_preview_thumb_file_get(const char * theme) {

	char * file = (char *)malloc(sizeof(char) * PATH_MAX);
	snprintf(file, PATH_MAX, "%s/.e/e/themes/%s", getenv("HOME"), theme);
	
	char * thumb = (char *)malloc(sizeof(char) * PATH_MAX);
	snprintf(thumb, PATH_MAX, "%s/.e/e/fileman/thumbnails/%s", getenv("HOME"), _e_thumb_file_id(file));

	return thumb;
}

int e_preview_thumb_generate(const char * theme) {

	printf("Generating thumb.\n");
	Ecore_Evas *ee = NULL;
	Evas *e = NULL;
	Evas_Object * preview;
	
	ee = ecore_evas_buffer_new(640, 480);
	e = ecore_evas_get(ee);
	preview = e_preview_new(e);
	e_preview_theme_set(preview, theme);
	evas_object_move(preview, 0, 0);
	evas_object_resize(preview, 640, 480);
	evas_object_show(preview);

	const int *pixels;
	pixels = ecore_evas_buffer_pixels_get(ee);
	char * out = e_preview_thumb_file_get(theme);
	
	Eet_File * ef;
	ef = eet_open(out, EET_FILE_MODE_WRITE);
	if (ef) {
		char path[PATH_MAX];
		snprintf(path, PATH_MAX, "%s/.e/e/themes/%s", getenv("HOME"), theme);
		eet_write(ef, "/thumbnail/orig_path", path, strlen(path), 1);
		eet_data_image_write(ef, "/thumbnail/data", (void *) pixels, 640, 480, 1, 0, 91, 1);
		eet_close(ef);
	}
	
	return 1;
}

static void _e_preview_clip_set(Evas_Object *o, Evas_Object *clip){
	E_Preview *data;
	if ((data = evas_object_smart_data_get(o))) {
		evas_object_clip_set(data->wallpaper, clip);
	}
}


/* return hash for a file  (from E) */
static char * _e_thumb_file_id(char *file) {
   char                s[256], *sp;
   const char         *chmap =
     "0123456789abcdef"
     "ghijklmnopqrstuv"
     "wxyz`~!@#$%^&*()"
     "[];',.{}<>?-=_+|";
   unsigned int        id[4], i;
   struct stat         st;

   if (stat(file, &st) < 0)
     return NULL;

   id[0] = st.st_ino;
   id[1] = st.st_dev;
   id[2] = (st.st_size & 0xffffffff);
   id[3] = (st.st_mtime & 0xffffffff);

   sp = s;
   for (i = 0; i < 4; i++)
     {
        unsigned int t, tt;
        int j;

        t = id[i];
        j = 32;
        while (j > 0)
          {
             tt = t & ((1 << 6) - 1);
             *sp = chmap[tt];
             t >>= 6;
             j -= 6;
             sp++;
          }
     }
   *sp = 0;
   return strdup(s);
}

