#ifndef EWLER_H
#define EWLER_H

#define true	1
#define false	0

typedef int bool;

#define NEW(type) ((type *) calloc(sizeof(type), 1))

#define EWLER_ERROR(format...) \
	do { \
		fprintf(stderr, "%s: ", __FUNCTION__); \
		fprintf(stderr, format); \
	} while(0);

#define EWL_CALLBACK_DEFN(func) \
	void func(Ewl_Widget *w, void *ev_data, void *user_data)

typedef struct Ewler_Project Ewler_Project;

typedef struct Ewler_Widget_Spec Ewler_Widget_Spec;
typedef struct Ewler_Widget_Elem_Spec Ewler_Widget_Elem_Spec;
typedef struct Ewler_Widget Ewler_Widget;
typedef struct Ewler_Widget_Elem Ewler_Widget_Elem;

typedef struct Ewler_Form Ewler_Form;

/* callback.c */
extern int callbacks_init( void );
extern void callbacks_update( Ecore_List *selected );
extern EWL_CALLBACK_DEFN(callbacks_toggle);

/* file.c */
extern void file_project_save( Ewler_Project *p );
extern int file_project_open( Ewler_Project *p );
extern int file_form_save( Ewler_Form *f );
extern int file_form_open( Ewler_Form *f );
extern int file_spec_open( char *filename, Ecore_Hash *hash );

/* form.c */
#define EWLER_WIDGET_BREAK		0
#define EWLER_WIDGET_ALIGN_V	1
#define EWLER_WIDGET_ALIGN_H	2
#define EWLER_WIDGET_DELETE		3
#define EWLER_WIDGET_COPY			4

struct Ewler_Form {
	bool dirty;
	bool ever_saved;
	char *filename;

	Ewl_Widget *window;
	Ewler_Widget *overlay;
	Ewl_Widget *menu;
	
	Ecore_List *widgets;
	Ecore_List *selected;
};

extern int forms_init( void );
extern void form_menu( int x, int y );
extern void form_rename_widget( char *old, char *name );
extern void form_add( Ewl_Widget *w );
extern void form_remove( Ewler_Form *form, Ewler_Widget *w );
extern void form_destroy( Ewler_Form *form, Ewler_Widget *w );
extern Ewler_Form *form_get( char *name );
extern void form_name_set( Ewler_Form *form, char *name );
extern Ewl_Widget *form_active( void );
extern EWL_CALLBACK_DEFN(form_new);
extern EWL_CALLBACK_DEFN(form_open);
extern void form_close( Ewler_Form *form );
extern void form_deselect_all( void );
extern void form_select( Ewler_Widget *w );
extern void form_deselect( Ewler_Widget *w );
extern void form_toggle( Ewler_Widget *w );
extern Ecore_List *form_selected( void );

/* inspector.c */
extern int inspector_init( void );
extern void inspector_update( Ecore_List *selected );
extern char *elem_to_s( Ewler_Widget_Elem *elem );
extern EWL_CALLBACK_DEFN(inspector_toggle);

/* main.c */
extern void ewler_error( char *fmt, ... );
extern void ewler_shutdown( void );
extern EWL_CALLBACK_DEFN(ewler_exit);

/* project.c */
#define EWLER_FILE_SAVE 0
#define EWLER_FILE_DELETE	1
#define EWLER_FILE_SETTINGS	2

struct Ewler_Project {
	bool dirty;
	bool ever_saved;
	char *name; /* project name */
	char *path; /* path where we should save new files */
	char *filename; /* project filename (hopefully path/name.ewl) */
	Ecore_Hash *files;
};

extern int projects_init( char *filename );
extern void project_update( void );
extern void project_file_add( char *name, char *filename );
extern void project_file_delete( char *name );
extern char *project_file_get( char *name );

/* spec.c */
#define INITIAL_EWLER_SPEC	PACKAGE_DATA_DIR"/schemas/widgets.xml"

typedef enum Ewler_Widget_Elem_Type Ewler_Widget_Elem_Type;

enum Ewler_Widget_Elem_Type {
	EWLER_SPEC_ELEM_INT,
	EWLER_SPEC_ELEM_STRING,
	EWLER_SPEC_ELEM_STRUCT,
	EWLER_SPEC_ELEM_ENUM,
	EWLER_SPEC_ELEM_ENUM_VAL,
	EWLER_SPEC_ELEM_POINTER,
	EWLER_SPEC_ELEM_INVALID
};

struct Ewler_Widget_Spec {
	char *name;

	int nargs;
	char *constructor;
	Ewl_Widget *(*ctor_func)();
	Ecore_Hash *ctor_args;
	
	char *parent;
	Ewler_Widget_Spec *parent_spec;

	Ecore_Hash *elems;

	bool visible;
	int count;
	bool group;
};

struct Ewler_Widget_Elem_Spec {
	char *name;
	Ewler_Widget_Elem_Type type;

	char *set;
	void (*set_func)(Ewl_Widget *w, ...);

	char *get;
	void *(*get_func)();

	union {
		int idefault;
		char *sdefault;
		Ecore_Hash *children;
		struct {
			Ecore_Hash *map; /* enum values -> strings */
			Ecore_Hash *map_rev; /* strings -> enum values */
			int edefault;
		} edata;
	} info;

	bool modifiable;
	int index;
};

#define EWLER_WIDGET_MOUSE_IN		0x1
#define EWLER_WIDGET_MOUSE_DOWN	0x2

struct Ewler_Widget {
	Ewler_Widget_Spec *spec;

	Ewl_Widget *w;
	Ewl_Widget *fg;
	Ewl_Widget *bg;
	Ewler_Form *form;
	char *parent;

	bool selected;
	bool selectable;
	bool configured;
	bool grouped;

	struct {
		int state;
		int button;
		int state_x;
		int state_y;
	} mouse;

	struct {
		int x;
		int y;
		int u;
		int v;
	} corners;

	char *source;

	Ecore_Hash *elems;

	Ecore_List *callbacks[EWL_CALLBACK_MAX];
};

struct Ewler_Widget_Elem {
	Ewler_Widget_Elem_Spec *spec;

	Ewler_Widget *w;

	union {
		int ivalue;
		char *svalue;
		Ecore_Hash *children;
		int evalue;
	} info;

	bool changed;
	Ewl_Widget *text; /* text label for this data in inspector */
	Ewl_Widget *entry; /* the entry used for inputting data in inspector */
	Ecore_Hash *items; /* a map of enum string values to menu_items */
};

extern int specs_init( void );
extern void spec_elem_destroy( void *p );
extern Ewler_Widget *spec_new( char *name );
extern Ewler_Widget_Elem_Spec *spec_elem_get( char *widget, char *spec );

/* tool.c */
#define EWLER_IMAGE_DIR	PACKAGE_DATA_DIR"/images"

extern int tools_init( void );
extern EWL_CALLBACK_DEFN(tools_toggle);

/* widget.c */
#define EWLER_EET	PACKAGE_DATA_DIR"/themes/ewler.eet"

extern bool widget_under_cursor;

extern Ewl_Widget *widget_new( char *name );
extern void widget_select( Ewler_Widget *w );
extern void widget_deselect( Ewler_Widget *w );
extern void widget_toggle( Ewler_Widget *w );
extern void widget_clear_ui_hooks( Ewler_Widget *w );

#endif
