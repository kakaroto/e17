#ifndef WIDGETS_H
#define WIDGETS_H

#define WIDGET_INTEGER_TYPE	1
#define WIDGET_STRING_TYPE	2
#define WIDGET_POINTER_TYPE	3
#define WIDGET_STRUCT_TYPE	4
#define WIDGET_ENUM_TYPE		5

/* elem flags */
#define ELEM_NO_MODIFY			0x01

/* The types defined for widget elements */
typedef union Widget_Type_Elem Widget_Type_Elem;

#define WIDGET_TYPE_BASE	\
	int w_type;	\
	char *name; \
	int w_flags; \
	char *get; \
	char *set; \
	char *cast;	\
	Widget_Type_Elem *parent; \
	int ctor_arg; \
	int index

struct Widget_Integer_Elem {
	WIDGET_TYPE_BASE;
	int flags; /* unsigned, short */
	long int default_value;
};

struct Widget_String_Elem {
	WIDGET_TYPE_BASE;
	const char *default_value;
};

struct Widget_Pointer_Elem {
	WIDGET_TYPE_BASE;
	char *ptr_type;
};

struct Widget_Struct_Elem {
	WIDGET_TYPE_BASE;
	Ecore_Hash *members;
};

struct Widget_Enum_Elem {
	WIDGET_TYPE_BASE;
	Ecore_Hash *map;
	Ecore_Hash *map_rev;
	int default_value;
};

union Widget_Type_Elem {
	struct {
		WIDGET_TYPE_BASE;
	} w;
	struct Widget_Integer_Elem w_int;
	struct Widget_String_Elem w_str;
	struct Widget_Pointer_Elem w_ptr;
	struct Widget_Struct_Elem w_struct;
	struct Widget_Enum_Elem w_enum;
};

/* The actual structures used to store widget info */
typedef union Widget_Data_Elem Widget_Data_Elem;

struct Widget_Integer_Data_Elem {
	Widget_Type_Elem *type;
	long int value;
};

struct Widget_String_Data_Elem {
	Widget_Type_Elem *type;
	char *value;
};

struct Widget_Pointer_Data_Elem {
	Widget_Type_Elem *type;
	void *value;
};

struct Widget_Struct_Data_Elem {
	Widget_Type_Elem *type;
	Ecore_List *members;
};

struct Widget_Enum_Data_Elem {
	Widget_Type_Elem *type;
	int value;
};

union Widget_Data_Elem {
	Widget_Type_Elem *type;
	struct Widget_Integer_Data_Elem w_int;
	struct Widget_String_Data_Elem w_str;
	struct Widget_Pointer_Data_Elem w_ptr;
	struct Widget_Struct_Data_Elem w_struct;
	struct Widget_Enum_Data_Elem w_enum;
};

typedef Ewl_Widget *(*Ewler_Ctor)();

int widgets_load( const char *filename );
Ecore_List *widgets_get_children( const char *parent );

char *widget_get_ctor( char *class );
Ecore_List *widget_create_info( const char *name );

int widget_get_error();
void widget_set_error( int err );

int widget_is_visible( char *name );
int widget_is_type( Ewl_Widget *w, char *type );
void widget_strset_info( Ecore_List *info, char *elem, char *value );
Widget_Data_Elem *widget_lookup_data( Ecore_List *info, char *elem );
Widget_Data_Elem *widget_lookup_ctor_arg( Ecore_List *info, int arg );
int widget_get_name( Ewl_Object *o );
char *widget_get_class( Ecore_List *w );
void widget_destroy_info( Ecore_List *info );
void widget_changed( Ewl_Widget *w );
Ecore_List *widget_get_children( Ecore_List *parent );
void widget_set_parent( Ecore_List *child, Ecore_List *parent );

#endif
