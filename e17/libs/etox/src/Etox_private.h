#ifndef ETOX_PRIVATE_H
#define ETOX_PRIVATE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <Evas.h>

typedef struct _Etox *			Etox;
typedef struct _Etox_Bit *		Etox_Bit;
typedef struct _Etox_Style *		Etox_Style;
typedef struct _Etox_Style_Bit * 	Etox_Style_Bit;
typedef struct _Etox_Color *		Etox_Color;
typedef struct _Etox_Color_Bit *	Etox_Color_Bit;
typedef struct _Etox_Obstacle *		Etox_Obstacle;

struct _Etox_Color_Bit
{
    char *name;
    int r, g, b, a;
};

struct _Etox_Color
{
    Evas_List bit_list;
};

struct _Etox
{
    char *name;
    char *text;
    int text_len;

    Etox_Obstacle *rect_list;
    int num_rects;
    char rendered;
    char visible;
    double x,y,w,h;
    int layer;
    char *font;
    Etox_Style font_style;
    int font_size;
    Etox_Bit *bit_list;
    int num_bits;
    Evas *evas;
    int align;
    Etox_Color color;
    double padding;
    char vertical_align;
    int alpha_mod;
    Evas_Object clip;
};

struct _Etox_Bit
{
    char *text;
    char underlined;
    double x,y,w,h;
    char *font;
    Evas_Object **evas_list;
    int num_evas;
    Etox_Style font_style;          
    int font_size;
};

struct _Etox_Style
{
    Etox_Style_Bit bits;
    int num_bits;
    char *name;
    int in_use;
};

struct _Etox_Style_Bit
{
    char type;
    int alpha;
    double x,y;
};

struct _Etox_Obstacle
{
    int num;
    double w,h;
    double x,y;
};


/** Internal Functions **/

int		_etox_search_tokens(const char *text, const char **needles, 
                                    int needles_count, char **beg, char **next);
void		_etox_available_size(Etox e, double beg_x, double beg_y, 
                                     double h, double padding, 
                                     double *av_x, double *av_y, double *av_w);
void		_etox_clean(Etox e);
void		_etox_refresh(Etox e);

Etox_Bit	_etox_bit_new(void);
void		_etox_bit_update_geometry(Etox e, Etox_Bit abit);
void		_etox_bit_set_face(Etox_Bit bit, char *font, int font_size, 
                                   Etox_Style style);
void		_etox_bit_create_objects(Etox_Bit abit, Etox e, 
                                         Etox_Color text_color);
void		_etox_bit_update_objects(Etox_Bit abit, Etox e);
void		_etox_bit_move_relative(Etox e, Etox_Bit abit, 
                                        double delta_x, double delta_y);
double		_etox_bit_dump_line(Etox_Bit *abits, int bit_count, Etox e, 
                                    char align, char vertical_align,
                                    double beg_x, double cur_w);

Etox_Color_Bit	_etox_color_get_bit(Etox_Color color, char *member);

char *		_etox_loadfile_atword(char *s, int num);
int		_etox_loadfile_is_whitespace(const char *s);
char *		_etox_loadfile_get_line(char *s, int size, FILE *f);
int		_etox_loadfile_is_good(char *path);

#endif
