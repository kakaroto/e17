#ifndef FORM_H
#define FORM_H

typedef struct Ewler_Form Ewler_Form;
#define EWLER_FORM(f) ((Ewler_Form *) f)

struct Ewler_Form {
	Ewl_Widget *window;
	Ewl_Widget *overlay;
	Ewl_Widget *layout;
	Ecore_List *selected;
	Ecore_Hash *elements, *elements_rev;
	Ewl_Widget *popup;
	char *filename;
	int has_been_saved;
	int dirty;
	int cnt;
};

typedef void (*Ewler_Form_Key_Cb)( Ewler_Form *form );

void ewler_forms_init( void );
void ewler_forms_close( void );

void form_new( void );

void form_selected_clear( Ewler_Form *form );
void form_selected_append( Ewler_Form *form, Ewl_Widget *w );
void form_set_dirty( Ewler_Form *form );
void form_set_widget_selected( void );
Ewl_Widget *form_get_element_by_name( Ewler_Form *form, char *name );
void form_add_widget( Ewler_Form *form, char *name, Ewl_Widget *w );

void form_save_file( Ewler_Form *form, int save_as );
void form_open_file( char *filename );
int form_is_open( char *filename );

#endif
