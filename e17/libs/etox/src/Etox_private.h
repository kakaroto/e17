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


typedef struct _Etox_All_Bits *         Etox_All_Bits;

typedef struct _Etox *			Etox;

typedef struct _Etox_Bit *		Etox_Bit;
typedef struct _Etox_Align *            Etox_Align;
typedef struct _Etox_Callback *         Etox_Callback;
typedef struct _Etox_Color *            Etox_Color;
typedef struct _Etox_Color_Bit *	Etox_Color_Bit;
typedef struct _Etox_Font *             Etox_Font;
typedef struct _Etox_Style *		Etox_Style;
typedef struct _Etox_Style_Bit * 	Etox_Style_Bit;
typedef struct _Etox_Text *		Etox_Text;

typedef struct _Etox_Obstacle *		Etox_Obstacle;

typedef struct _Etox_Object *		Etox_Object;


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

  Ewd_List *bits;
  Ewd_List *obstacles;

  Ewd_DList *etox_objects;
  Ewd_DList *evas_objects;
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
  int bleh;
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


struct _Etox_Obstacle
{
  double x, y, w, h;
};


struct _Etox_Object
{
  double x, y, w, h;
  char *str;
  Ewd_List *ev_objects;

  struct _Etox_All_Bits bit;
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

Etox_Object	_etox_object_new(double x, double y, Etox_All_Bits bits);
void		_etox_object_free(Etox_Object obj);
int		_etox_object_get_available_size(Etox e, Etox_Object obj);
int		_etox_object_get_string_that_fits(Etox e, Etox_Object obj);
void		_etox_object_move(Etox e, Etox_Object obj);
void		_etox_object_finish(Etox e, Etox_Object obj);

Etox_Color_Bit	_etox_color_get_bit(Etox_Color color, char *member);
void		_etox_color_bit_free(Etox_Color_Bit bit);

char *		_etox_loadfile_atword(char *s, int num);
int		_etox_loadfile_is_whitespace(const char *s);
char *		_etox_loadfile_get_line(char *s, int size, FILE *f);
int		_etox_loadfile_is_good(char *path);

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

#endif
