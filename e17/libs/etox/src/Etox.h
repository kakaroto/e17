#ifndef ETOX_H
#define ETOX_H 1

#include <Evas.h>
#include <Ewd.h>

#ifndef ETOX_PRIVATE_H
typedef void *	Etox;
typedef void *	Etox_Style;
typedef void *	Etox_Color;
typedef void *	Etox_Obstacle;
typedef void *	Etox_Callback;
#endif


enum _Etox_Align_Type
{
   ETOX_ALIGN_TYPE_NULL,
   ETOX_ALIGN_TYPE_LEFT,
   ETOX_ALIGN_TYPE_RIGHT,
   ETOX_ALIGN_TYPE_CENTER,
   ETOX_ALIGN_TYPE_TOP,
   ETOX_ALIGN_TYPE_BOTTOM
};

enum _Etox_Callback_Type
{
   ETOX_CALLBACK_TYPE_NULL
};

enum _Etox_Style_Type
{
   ETOX_STYLE_TYPE_NULL,
   ETOX_STYLE_TYPE_FOREGROUND,
   ETOX_STYLE_TYPE_SHADOW,
   ETOX_STYLE_TYPE_OUTLINE
};

enum _Etox_Bit_Type
{
   ETOX_BIT_TYPE_NULL,
   ETOX_BIT_TYPE_ALIGN,
   ETOX_BIT_TYPE_CALLBACK,
   ETOX_BIT_TYPE_COLOR,
   ETOX_BIT_TYPE_FONT,
   ETOX_BIT_TYPE_STYLE,
   ETOX_BIT_TYPE_TEXT,
   ETOX_BIT_TYPE_ALIGN_END,
   ETOX_BIT_TYPE_CALLBACK_END,
   ETOX_BIT_TYPE_COLOR_END,
   ETOX_BIT_TYPE_FONT_END,
   ETOX_BIT_TYPE_STYLE_END
};

typedef enum _Etox_Align_Type   	Etox_Align_Type;
typedef enum _Etox_Callback_Type	Etox_Callback_Type;
typedef enum _Etox_Style_Type   	Etox_Style_Type;
typedef enum _Etox_Bit_Type		Etox_Bit_Type;

/**  callback stuff is unfinished.. */
#define ET_ALIGN(v, h) ETOX_BIT_TYPE_ALIGN, v, h
#define ET_CALLBACK(type, func, data) ETOX_BIT_TYPE_CALLBACK, type, func, data
#define ET_COLOR(color) ETOX_BIT_TYPE_COLOR, color
#define ET_FONT(font, size) ETOX_BIT_TYPE_FONT, font, size
#define ET_STYLE(style) ETOX_BIT_TYPE_STYLE, style
#define ET_TEXT(text) ETOX_BIT_TYPE_TEXT, text

#define ET_ALIGN_END ETOX_BIT_TYPE_ALIGN_END
#define ET_CALLBACK_END ETOX_BIT_TYPE_CALLBACK_END
#define ET_COLOR_END ETOX_BIT_TYPE_COLOR_END
#define ET_FONT_END ETOX_BIT_TYPE_FONT_END
#define ET_STYLE_END ETOX_BIT_TYPE_STYLE_END

#define ET_END NULL


#ifdef __cplusplus
extern "C"
{
#endif

Etox	etox_new(Evas evas, char *name);
Etox	etox_new_all(Evas evas, char *name,
                     double x, double y, double w, double h,              
                     int alpha, double padding,  
                     Etox_Align_Type v_align, Etox_Align_Type h_align,
                     Etox_Callback callback, Etox_Color color,
                     char *font, int font_size, Etox_Style style);
void	etox_free(Etox e);

void	etox_show(Etox e);
void	etox_hide(Etox e);
void	etox_raise(Etox e);
void	etox_lower(Etox e);

void	etox_move(Etox e, double x, double y);
void	etox_resize(Etox e, double w, double h);

void	etox_set_evas(Etox e, Evas evas);
void	etox_set_name(Etox e, char *name);
void    etox_set_alpha(Etox e, int alpha);
void    etox_set_padding(Etox e, double padding);

void	etox_set_layer(Etox e, int layer);
void	etox_set_clip(Etox e, Evas_Object clip);
void    etox_unset_clip(Etox e);

/* these funcs set the defaults.. */
void    etox_set_align(Etox e, Etox_Align_Type h_align, Etox_Align_Type v_align);
void    etox_set_color(Etox e, Etox_Color color);
void    etox_set_font(Etox e, char *font, int size);
void    etox_set_style(Etox e, Etox_Style style);

void    etox_set_text(Etox e, ...); 


Evas    	etox_get_evas(Etox e);
char *		etox_get_name(Etox e);
int             etox_get_alpha(Etox e);
double          etox_get_padding(Etox e);

int             etox_get_layer(Etox e);
Evas_Object	etox_get_clip(Etox e);

Etox_Align_Type	etox_get_align_v(Etox e);
Etox_Align_Type etox_get_align_h(Etox e);
Etox_Color      etox_get_color(Etox e);
char *		etox_get_font_name(Etox e);
int		etox_get_font_size(Etox e);
Etox_Style	etox_get_style(Etox e);

char *		etox_get_text(Etox e);

void		etox_get_geometry(Etox e, double *x, double *y, 
				  double *w, double *h);
void            etox_get_actual_geometry(Etox e, double *x, double *y, 
                                  	 double *w, double *h);

/* Following function isnt implemented yet. */
void		etox_get_at(Etox e, int index,
				double *x, double *y,
				double *w, double *h);
int		etox_get_at_position(Etox e, double x, double y,
                                     double *char_x, double *char_y, 
                                     double *char_w, double *char_h);


Etox_Obstacle	etox_obstacle_add(Etox e, 
                                  double x, double y, double w, double h);
void		etox_obstacle_set(Etox e, Etox_Obstacle obst, 
				  double x, double y, double w, double h);
void		etox_obstacle_del(Etox e, Etox_Obstacle obst);


Etox_Style	etox_style_new(char *name);
void		etox_style_free(Etox_Style style);

void		etox_style_add_path(char *path);
void		etox_style_del_path(char *path);
Ewd_List	*etox_style_get_paths(void);


Etox_Color	etox_color_new(void);
void		etox_color_set_member(Etox_Color color, char *member,
                       	              int r, int g, int b, int a);
void		etox_color_free(Etox_Color color);


/* NOTE: don't forget to free the stuff returned by these funcs.. */
char 	*etox_str_remove_beginning_spaces(char *str);
char 	*etox_str_remove_ending_spaces(char *str);
char 	*etox_str_chop_off_beginning_string(char *str, char *chop);
char 	*etox_str_chop_off_beginning_word(char *str);
char 	*etox_str_chop_off_ending_string(char *str, char *chop);
char 	*etox_str_chop_off_ending_word(char *str);


#ifdef __cplusplus
}
#endif

#endif
