#ifndef ETOX_PRIVATE_H
#define ETOX_PRIVATE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <Evas.h>
#include <Ewd.h>
#include "etox-config.h"

#define ET_X_TO_EV(A) (A + e->x)
#define ET_Y_TO_EV(A) (A + e->y)
#define EV_X_TO_ET(A) (A - e->x)
#define EV_Y_TO_ET(A) (A - e->y)

#ifdef DEBUG

#define _etox_rebuild(A) \
printf("ETOX: %s() (%s:%d): Rebuilding..\n", __FUNCTION__, __FILE__, __LINE__); \
_D_etox_rebuild(A)
#define _etox_refresh(A) \
printf("ETOX: %s() (%s:%d): Refreshing..\n", __FUNCTION__, __FILE__, __LINE__); \
_D_etox_refresh(A)
#define D_PRINT(args...) \
printf("ETOX: %s() (%s:%d): ", __FUNCTION__, __FILE__, __LINE__); printf(args)

#else

#define D_PRINT(args...) 
  
#endif

enum _Etox_Object_Bit_Type
{
   ETOX_OBJECT_BIT_TYPE_NULL,
   ETOX_OBJECT_BIT_TYPE_STRING,
   ETOX_OBJECT_BIT_TYPE_NEWLINE,
   ETOX_OBJECT_BIT_TYPE_TAB
};

typedef enum _Etox_Object_Bit_Type Etox_Object_Bit_Type;

typedef struct _Etox_All_Bits *         Etox_All_Bits;

typedef struct _Etox *			Etox;

typedef struct _Etox_Bit *		Etox_Bit;
typedef struct _Etox_Align *    	Etox_Align;
typedef struct _Etox_Callback *         Etox_Callback;
typedef struct _Etox_Color *            Etox_Color;
typedef struct _Etox_Color_Bit *	Etox_Color_Bit;
typedef struct _Etox_Font *             Etox_Font;
typedef struct _Etox_Style *		Etox_Style;
typedef struct _Etox_Style_Bit * 	Etox_Style_Bit;
typedef struct _Etox_Text *		Etox_Text;

typedef struct _Etox_Object *		Etox_Object;

typedef struct _Etox_Object_Bit *	Etox_Object_Bit;
typedef struct _Etox_Object_String *	Etox_Object_String;
typedef struct _Etox_Object_Newline *	Etox_Object_Newline;
typedef struct _Etox_Object_Tab *	Etox_Object_Tab;

typedef struct _Etox_Obstacle *         Etox_Obstacle;

typedef struct _Etox_Sort *		Etox_Sort;


#include "Etox.h"


struct _Etox_All_Bits
{
  Etox_Align align;
  Etox_Callback callback;
  Etox_Color color;   
  Etox_Font font;
  Etox_Style style;
  Etox_Text text;
};


struct _Etox
{
  Evas evas;
  char *name;
  double x, y, w, h;
  int a; 
  double padding; 

  int show;
  int raise;
  int lower;
  int layer;
  Evas_Object clip;

  struct _Etox_All_Bits def;

  /* This is the list of Etox_Bits as passed in by the user. */
  Ewd_List *bits;
  Ewd_List *obstacles;

  /* This is a list of Etox_Object_Bits. It only needs to be built once
   * initially and then everytime the text changes. The dirty flag is set
   * whenever that happens.*/
  struct _etox_object_bits
  {
     Ewd_List *list;
     int dirty;
  } etox_object_bits;

  /* This is a list of Etox_Objects. Those continuous chunks that group
   * together several Etox_Object_Bits. They need to be recalculated once
   * initially and on every change of layout. */
  struct _etox_objects
  {
    double h; 
    Ewd_List *list;
    int dirty;
  } etox_objects;

  /* These are the actual evas objects that are rendered. They change whenever
   * the above etox_objects change. */
  struct _evas_objects
  {
    Ewd_List *list;
  } evas_objects;
};


struct _Etox_Bit
{
  Etox_Bit_Type type;
  void *body;
}; 
 
struct _Etox_Align
{
  Etox_Align_Type v, h;
};

struct _Etox_Callback
{
  /* TODO */
};

struct _Etox_Color
{
  Ewd_List *bits;
};

struct _Etox_Color_Bit
{  
  char *name;
  int r, g, b, a;  
};

struct _Etox_Font
{
  char *name;
  int size;
  double ascent, descent;
};

struct _Etox_Style
{
  char *name;
  double offset_w, offset_h;
  Ewd_List *bits;
};

struct _Etox_Style_Bit
{
  Etox_Style_Type type;
  double x, y;
  int a;
};

struct _Etox_Text
{
  char *str;
};


struct _Etox_Object
{
  double x, y, w;
  Ewd_List *bits;
};


struct _Etox_Object_Bit
{
  Etox_Object_Bit_Type type;
  double x, y, w, h;
  void *body;
  struct _evas_objects_list
  {
    Ewd_List *fg;
    Ewd_List *sh;
    Ewd_List *ol;
  } evas_objects_list;
};

struct _Etox_Object_String
{
  char *str;
  Etox_Align align;
  Etox_Callback callback;
  Etox_Color color;
  Etox_Font font;
  Etox_Style style;
};

struct _Etox_Object_Newline
{
  /* empty */
};

struct _Etox_Object_Tab
{
  Etox_Align align;
  Etox_Callback callback;
  Etox_Font font;
};

struct _Etox_Obstacle
{
  double x, y, w, h;
};


struct _Etox_Sort
{
  void **value;
  int data_added;
  int size;
};


/** Internal Functions **/

#ifdef DEBUG
void		_D_etox_rebuild(Etox e);
void		_D_etox_refresh(Etox e);
#else
void		_etox_rebuild(Etox e);
void		_etox_refresh(Etox e);
#endif

void		_etox_create_etox_objects(Etox e);
void		_etox_create_evas_objects(Etox e);

Etox_Object		_etox_object_new(Etox e, double x, double y);
void			_etox_object_free(Etox_Object obj);
int			_etox_object_get_available_size(Etox_Object obj, 
							double *w);
int			_etox_object_add_bit(Etox_Object obj, 
					     Etox_Object_Bit obj_bit);
Etox_Object_Bit		_etox_object_bit_new(void);
Etox_Object_Bit		_etox_object_bit_clone(Etox_Object_Bit);
void			_etox_object_bit_free(Etox_Object_Bit bit);
void			_etox_object_bit_set_body(Etox e, 
					  	Etox_Object_Bit bit, 
  				          	void *body,
                		          	Etox_Object_Bit_Type type);
void 			_etox_object_bit_get_char_geometry_at(Etox e,
                                                              Etox_Object_Bit obj_bit,	
                                                              int index,
                                                              double *char_x, 
                                                              double *char_y,
                                                              double *char_w, 
                                                              double *char_h);
int			_etox_object_bit_get_char_geometry_at_position(Etox e,
                                                                       Etox_Object_Bit obj_bit,
                                                                       double x, double y,
                                                                       double *char_x,
                                                                       double *char_y,
                                                                       double *char_w,
                                                                       double *char_h);
Etox_Object_String      _etox_object_string_new(char *str,
                                                Etox_Align align,
                                                Etox_Callback callback,
                                                Etox_Color color,
                                                Etox_Font font,
                                                Etox_Style style);
void                    _etox_object_string_free(Etox_Object_String string);
void                    _etox_object_string_set_string(
                                         Etox_Object_String string, char *t);
Etox_Object_String      _etox_object_string_clone(Etox_Object_String string);
Etox_Object_Newline	_etox_object_newline_new(void);
void			_etox_object_newline_free(Etox_Object_Newline nl);
Etox_Object_Tab		_etox_object_tab_new(Etox_Align align,
                     			     Etox_Callback callback,
                     			     Etox_Font font);
void			_etox_object_tab_free(Etox_Object_Tab tab);
Etox_Object_Tab		_etox_object_tab_clone(Etox_Object_Tab tab);

void		_etox_align_etox_object(Etox e, Etox_Object obj);

Etox_Color_Bit	_etox_color_get_bit(Etox_Color color, char *member);
void		_etox_color_bit_free(Etox_Color_Bit bit);

char *		_etox_loadfile_atword(char *s, int num);
int		_etox_loadfile_is_whitespace(const char *s);
char *		_etox_loadfile_get_line(char *s, int size, FILE *f);
int		_etox_loadfile_is_good(char *path);

char *		_etox_get_string_that_fits(Etox e, char *long_str, double w,
		                           Etox_All_Bits bits);
void		_etox_get_string_width(Etox e, Etox_Font font, char *str, 
                                       double *w);
void		_etox_get_font_ascent_descent(Etox e, Etox_Font font,
                                              double *ascent, double *descent); 
void		_etox_get_style_offsets(Etox_Style style, double *offset_w, 
                                        double *offset_h);

Etox_Align	_etox_bit_align_new(Etox_Align_Type v, Etox_Align_Type h);
void		_etox_bit_align_free(Etox_Align align);
Etox_Callback	_etox_bit_callback_new();
void		_etox_bit_callback_free(Etox_Callback cb);
Etox_Font	_etox_bit_font_new(char *name, int size, Etox e);
void		_etox_bit_font_free(Etox_Font font);
Etox_Text	_etox_bit_text_new(char *str);
void		_etox_bit_text_free(Etox_Text text);
void		_etox_bit_free(Etox_Bit bit);

Etox_Sort	_etox_sort_new(int size);
void		_etox_sort_free(Etox_Sort sort);
int		_etox_sort_get_size(Etox_Sort sort);
void		_etox_sort_set_data_from_list(Etox_Sort sort, Ewd_List *list);
void *		_etox_sort_get_data(Etox_Sort sort, int index);
void		_etox_sort_swap(Etox_Sort sort, int left, int right);
void		_etox_sort_now(Etox_Sort sort, int left, int right,
		               int (*compare)(void *, void *));

void		_etox_align_etox_object(Etox e, Etox_Object obj);

#endif
