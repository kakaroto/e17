#ifndef EWLER_H
#define EWLER_H

#include <Ewl.h>
#include <EXML.h>

#include "config.h"

#define false	0
#define true	1

#define PACKAGE_IMAGE_DIR	PACKAGE_DATA_DIR"/images"

#define EWL_CALLBACK_DEFN(func) \
	void func(Ewl_Widget *w, void *ev, void *user_data)

typedef struct _Ewler_Options Ewler_Options;
typedef struct _Ewler_Spec_Widget Ewler_Spec_Widget;
typedef struct _Ewler_Spec_Elem Ewler_Spec_Elem;
typedef struct _Ewler_Elem Ewler_Elem;
typedef struct _Ewler_Widget Ewler_Widget;

typedef struct _Ewler_Project Ewler_Project;
typedef struct _Ewler_Project_File Ewler_Project_File;

typedef struct _Ewler_Tool Ewler_Tool;

extern Ewler_Options options;

/* Widget Specification functions */
void ewler_spec_init( void );
Ecore_List *ewler_specs_get( void );
int ewler_spec_read( char *filename );
Ewl_Widget *ewler_spec_call_ctor( char *widget_name );
Ewler_Spec_Widget *ewler_spec_get( char *widget_name );
int ewler_spec_is( Ewler_Spec_Widget *spec, char *type );

/* Ewler Utility functions */
void ewler_fatal( const char *fmt, ... );
void ewler_error( const char *fmt, ... );
void usage( void );
char *strip( char *s );
void ewler_shutdown( void );
int ewler_is_done( void );

/* Ewler UI functions */
void ewler_ui_init( void );
void ewler_ui_project_update( void );

/* Ewler Widget functions */
Ewl_Widget *ewler_widget_dragging_get( void );
Ewler_Widget *ewler_widget_new( char *widget_name, Ewler_Project_File *form );
Ewler_Widget *ewler_widget_from_bg( Ewl_Widget *bg );
void ewler_widget_select( Ewler_Widget *w );
void ewler_widget_deselect( Ewler_Widget *w );
char *ewler_widget_elem_to_s( Ewler_Elem *elem );
void ewler_widget_attr_set_from_xml( Ewler_Widget *w, Ecore_Hash *attrs,
																		 EXML *xml );

/* Ewler Project functions */
void ewler_project_init( void );
Ecore_List *ewler_project_files_get( void );
int ewler_project_file_new( char *filename, int type );
int ewler_project_file_open( char *filename, int type );
int ewler_project_file_save( Ewler_Project_File *file );
int ewler_project_file_del( char *filename );
int ewler_project_file_rename( char *old, char *filename );

/* Ewler Form functions */
EWL_CALLBACK_DEFN(ewler_form_focus_cb);
EWL_CALLBACK_DEFN(ewler_form_close_cb);
EWL_CALLBACK_DEFN(ewler_form_mouse_move_cb);
EWL_CALLBACK_DEFN(ewler_form_mouse_down_cb);
Ewler_Project_File *ewler_form_active_get( void );
void ewler_form_active_set( Ewler_Project_File *form );
int ewler_form_save( Ewler_Project_File *form );
void ewler_form_select_clear( void );
Ecore_List *ewler_form_select_get( void );

/* Ewler Toolbar function */
void ewler_toolbar_init( void );
void ewler_toolbar_toggle( void );
void ewler_toolbar_current_set( Ewl_Widget *tool );
Ewl_Widget *ewler_toolbar_current_get( void );

/* Ewler Tool functions */
#define EWLER_TOOL(t) ((Ewler_Tool *) t)

Ewl_Widget *ewler_tool_new( char *icon_file, char *name, char *cursor,
														Ewler_Spec_Widget *widget );
int ewler_tool_init( Ewler_Tool *t, char *icon_file, char *name, char *cursor,
										 Ewler_Spec_Widget *widget );

/* Ewler Inspector functions */
void ewler_inspector_init( void );
void ewler_inspector_toggle( void );
void ewler_inspector_widget_set( Ewler_Widget *w );
void ewler_inspector_update( void );

struct _Ewler_Options {
	int debug;
	Ecore_List *files;
};

#define EWLER_SPEC_ELEM_INT_TYPE			0
#define EWLER_SPEC_ELEM_STRING_TYPE		1
#define EWLER_SPEC_ELEM_POINTER_TYPE	2
#define EWLER_SPEC_ELEM_STRUCT_TYPE		3
#define EWLER_SPEC_ELEM_ENUM_TYPE			4

struct _Ewler_Spec_Elem {
	char *name;
	int type;

	char *set_name;
	char *get_name;

	int (*get_func)(Ewl_Widget *o, ...);
	void (*set_func)(Ewl_Widget *o, ...);

	union {
		struct {
			int has_default;
			int default_value;
		} int_type;

		/* no data required for strings */

		char *pointer_type;

		Ecore_Hash *struct_child_elems;

		struct {
			Ecore_Hash *map;
			Ecore_Hash *map_rev;
			int has_default;
			int default_value;
		} enum_type;
	} type_info;
};

struct _Ewler_Spec_Widget {
	char *name;
	char *super;
	int visible;
	int count;
	
	char *ctor_name;
	int ctor_nargs;
	Ewl_Widget *(*ctor_func)();

	Ecore_Hash *elem_specs;
};

struct _Ewler_Elem {
	Ewler_Spec_Elem *spec;
	int changed; /* changed from realize call */

	union {
		int int_val;
		char *string_val;
		char *pointer_val;
		Ecore_Hash *struct_val;
		int enum_val;
	} v;
};

/* actual instantiated widget */
struct _Ewler_Widget {
	char *name;

	int selected;
	int dragging;
	int realized;
	char *source;

	struct {
		int x, y, u, v;
	} corners;

	Ewler_Spec_Widget *spec;
	Ewl_Widget *ewl_w;
	Ewl_Widget *bg, *fg;
	Ecore_Hash *attrs;
	Ewler_Project_File *form;
};

#define EWLER_PROJECT_FILE_FORM	0
#define EWLER_PROJECT_FILE_SRC	1
#define EWLER_PROJECT_FILE_HDR	2

#define FORM(f) (f)->type_info.form

struct _Ewler_Project_File {
	char *filename;
	int type;

	int dirty;

	union {
		struct {
			Ecore_List *includes;
			EXML *xml;
			Ewler_Widget *top;
			Ewl_Widget *window;
			Ewl_Widget *menu;
		} form;
	} type_info;
};

#define EWLER_PROJECT_BASIC	0
#define EWLER_PROJECT_AUTO	1

struct _Ewler_Project {
	char *filename; /* project specification filename */
	char *path; /* project path, default location for new project entries */
	char *descr; /* project description */
	
	int type; /* specifies auto* comp, basic, or other */
	int dirty;

	char *cflags;
	char *ldflags;

	Ecore_List *files;
};

struct _Ewler_Tool {
	Ewl_Button box;
	char *icon_file; /* icon resource image */
	char *name; /* tool name, to appear in toolbar window */
	char *cursor; /* cursor to enable in project view when selected */

	Ewler_Spec_Widget *widget; /* resource for placement callbacks */
};
	
#endif
