#ifndef ETOX_H
#define ETOX_H 1

#include <Evas.h>

#ifndef ETOX_PRIVATE_H
typedef void *		Etox;
typedef void *		Etox_Style;
typedef void *		Etox_Color;
typedef void *		Etox_Obstacle;
#endif


enum _Etox_Align
{
   ETOX_ALIGN_LEFT,
   ETOX_ALIGN_RIGHT,
   ETOX_ALIGN_CENTER,
   ETOX_ALIGN_TOP,
   ETOX_ALIGN_BOTTOM
};

enum _Etox_Style_Type
{
   ETOX_STYLE_TYPE_FOREGROUND,
   ETOX_STYLE_TYPE_SHADOW,
   ETOX_STYLE_TYPE_OUTLINE
};

typedef enum _Etox_Align        Etox_Align;
typedef enum _Etox_Style_Type   Etox_Style_Type;


#ifdef __cplusplus
extern "C"
{
#endif

Etox	etox_new(Evas evas, char *name);
Etox	etox_new_all(Evas evas, char *name,
		     double x, double y, double w, double h,
		     char *font, int font_size, char *style_path,
		     Etox_Color color, Etox_Align h_align, Etox_Align v_align,
		     int layer, double padding);
void	etox_free(Etox e);

void	etox_show(Etox e);
void	etox_hide(Etox e);
void	etox_raise(Etox e);
void	etox_lower(Etox e);

void	etox_move(Etox e, double x, double y);
void	etox_resize(Etox e, double w, double h);

void	etox_set_evas(Etox e, Evas evas);
void	etox_set_name(Etox e, char *name);
void	etox_set_font(Etox e, char *font, int size);
void	etox_set_style(Etox e, Etox_Style style);
void	etox_set_align(Etox e, Etox_Align h_align, Etox_Align v_align);
void	etox_set_layer(Etox e, int layer);
void	etox_set_padding(Etox e, double padding);
void	etox_set_text(Etox e, char *new_text);
void	etox_set_color(Etox e, Etox_Color color);
void	etox_set_alpha(Etox e, int alpha);
void	etox_set_clip(Etox e, Evas_Object clip);
void    etox_set_unclip(Etox e);

Evas    	etox_get_evas(Etox e);
char *		etox_get_name(Etox e);
Etox_Style	etox_get_style(Etox e);
int		etox_get_layer(Etox e);
double		etox_get_padding(Etox e);
char *		etox_get_text(Etox e);
Etox_Color	etox_get_color(Etox e);
int		etox_get_alpha(Etox e);
void		etox_get_geometry(Etox e, double *x, double *y, double *w, double *h);
void		etox_get_at(Etox e, int index, int *x, int *y, int *w, int *h);
int		etox_get_at_position(Etox e, int x, int y, int *index);

Etox_Obstacle	etox_obstacle_add(Etox e, 
                                  double x, double y, double w, double h);
void		etox_obstacle_set(Etox e, Etox_Obstacle obst, 
				  double x, double y, double w, double h);
void		etox_obstacle_del(Etox e, Etox_Obstacle obst);

Etox_Style	etox_style_new(char *path);
void		etox_style_free(Etox_Style style);

void		etox_style_add_path(char *path);
void		etox_style_del_path(char *path);
char **		etox_style_get_paths(int *number_return);

Etox_Color	etox_color_new(void);
void		etox_color_set_member(Etox_Color color, char *member,
                       	              int r, int g, int b, int a);
void		etox_color_free(Etox_Color color);


#ifdef __cplusplus
}
#endif

#endif
