#ifndef EBITS_PRIVATE_H
#define EBITS_PRIVATE_H 1

#define EDITOR 1
#define _EBITS_INTERNAL
#include <Ebits.h>
#include <Edb.h>
#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define UN(_blah) _blah = 0

typedef struct _Ebits_Object_Description *Ebits_Object_Description;
typedef struct _Ebits_Object_State Ebits_Object_State;
typedef struct _Ebits_Object_Bit_Description *Ebits_Object_Bit_Description;
typedef struct _Ebits_Callback *Ebits_Callback;
typedef struct _Ebits_Object_Bit_State *Ebits_Object_Bit_State;
typedef struct _Ebits_State_Description *Ebits_State_Description;

typedef struct _Ebits_State_Source_Description *Ebits_State_Source_Description;

Ebits_Object    ebits_new(void);

#ifdef EDITOR
Ebits_Object_Bit_State ebits_get_bit_name(Ebits_Object o, char *name);
void            ebits_save(Ebits_Object o, char *file);
Ebits_Object_Bit_State ebits_new_bit(Ebits_Object o, char *file);
Ebits_Object_Description ebits_new_description(void);
void            ebits_del_bit(Ebits_Object o, Ebits_Object_Bit_State state);
void            ebits_set_state(Ebits_Object o, char *st);
void            _ebits_evaluate(Ebits_Object_Bit_State state);
void            ebits_add_state_name(Ebits_Object o, char *name);
void            ebits_del_state_name(Ebits_Object o, char *name);
void            ebits_add_bit_state(Ebits_Object_Bit_State bit, char *state,
				    char *image);
#endif


/* Enum for the way an image is used to fill a bit: */
typedef enum _Ebits_Fill_Mode {
	EBITS_FILL_SCALE = 0,	/* scale to fit                               */
	EBITS_FILL_TILE = 1,	/* tile at image's size                       */
	EBITS_FILL_FITTED_TILE = 2	/* tile so that image fits n times, n integer */
} Ebits_Fill_Mode;

struct _Ebits_State_Description {
	char           *image;
	char           *name;
	Ebits_State_Source_Description ss_d;
};

#ifdef EDITOR
struct _Ebits_State_Source_Description {
	int             saved;
	Imlib_Image     image;
	char           *name;
	Ebits_State_Description state_d;
};
#endif /* EDITOR */

struct _Ebits_Object_Description {
	char           *file;
	int             references;
	struct {
		int             w, h;
	} min          , max;
	struct {
		int             x, y;
	} step;
	struct {
		int             l, r, t, b;
	} padding      , inset;
	Evas_List       bits;
	struct {
		int             caculated;
		int             w;
		int             h;
	} real_min_size;

	Evas_List       state_names;
};

struct _Ebits_Object_Bit_Description {
	char           *name;
	char           *class;
	char           *color_class;
	struct {
		char           *image;
	} normal       , hilited, clicked, disabled;
	Evas_List       state_description;
	struct {
		int             l, r, t, b;
	} border;
	struct {
		Ebits_Fill_Mode w, h;
	} tile;
	struct {
		char           *name;
		int             x, y;
		double          rx, ry;
		int             ax, ay;
	} rel1         , rel2;
	struct {
		double          w, h;
	} align;
	struct {
		int             x, y;
	} aspect       , step;
	struct {
		int             w, h;
	} min          , max;
	Evas_List       sync;
};

struct _Ebits_Object_State {
	double          x, y, w, h;
	int             layer;
	int             visible;
	Evas_Object     clip;
	Evas            evas;
};

struct _Ebits_Callback {
	Evas_Callback_Type type;
	void            (*func) (void *_data, Ebits_Object _o, char *_c, int _b,
				 int _x, int _y, int _ox, int _oy, int _ow,
				 int _oh);
	void           *data;
};

struct _Ebits_Object_Bit_State {
	Ebits_Object    o;
	Evas_Object     object;
	Ebits_Object_Bit_Description description;
	int             r, g, b, a;
	int             recalc;
	int             calculated;
	int             calc_pending;
	int             x, y, w, h;
	int             mouse_in;
	char           *state;
	int             syncing;
	int             want_w, want_h;
	Evas_List       callbacks;

	/* callbacks for when you embed an object in a bit */
	void           *func_data;

	void            (*func_show) (void *_data);
	void            (*func_hide) (void *_data);
	void            (*func_move) (void *_data, double x, double y);
	void            (*func_resize) (void *_data, double w, double h);
	void            (*func_raise) (void *_data);
	void            (*func_lower) (void *_data);
	void            (*func_set_layer) (void *_data, int l);
	void            (*func_set_clip) (void *_data, Evas_Object clip);
	void            (*func_get_min_size) (void *_data, double *w,
					      double *h);
	void            (*func_get_max_size) (void *_data, double *w,
					      double *h);
	void            (*func_set_color_class) (void *_data, char *cc, int r,
						 int g, int b, int a);

#ifdef EDITOR
	struct {
		int             saved;
		Imlib_Image     image;
	} normal       , hilited, clicked, disabled;

	Evas_List       state_source_description;
#endif
};

struct _Ebits_Object {
	Ebits_Object_Description description;
	Ebits_Object_State state;
	Evas_List       bits;
};


#endif
