
#ifndef ETOX_H
#define ETOX_H 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <Evas.h>

typedef struct {

	double w,h;
	double x,y;

} E_Clip_Rect;

typedef struct {

	int r,g,b;

} E_Color;

typedef struct
{

   E_Color fg;
   E_Color ol;
   E_Color sh;

} E_Text_Color;

#define STYLE_TYPE_FOREGROUND 0
#define STYLE_TYPE_SHADOW     1
#define STYLE_TYPE_OUTLINE    2

#define ALIGN_LEFT   0
#define ALIGN_RIGHT  1
#define ALIGN_CENTER 2
#define ALIGN_TOP    3
#define ALIGN_BOTTOM 4

typedef struct {

	char type;
	int alpha;
	double x,y;

} E_Style_Bit;

typedef struct {

	E_Style_Bit *bits;
	int num_bits;
	char *name;
	int in_use;

} E_Font_Style;

typedef struct {

	char *text;
	char underlined;
	double x,y,w,h;
	char *font;
	Evas_Object **evas_list;
	int num_evas;
	E_Font_Style *font_style;
	int font_size;

} Etox_Bit;

typedef struct {

	char *name;
	char *text;
	int text_len;

	E_Clip_Rect **rect_list;
	int num_rects;
	char rendered;
	char visible;
	double x,y,w,h;
	int layer;
	char *font;
	E_Font_Style *font_style;
	int font_size;
	Etox_Bit **bit_list;
	int num_bits;
	Evas *evas;
        int align;
        E_Text_Color color;
        double padding;
        char vertical_align;
        int alpha_mod;
   
} Etox;

#ifdef __cplusplus
extern "C"
{
#endif

	E_Font_Style *E_load_font_style(char *path);
	void E_Font_Style_free(E_Font_Style *style);
	char etox_set_font_style(Etox *e, E_Font_Style *font_style);
        void etox_add_path_to_font_style_path(char *path);
        void etox_remove_path_to_font_style_path(char *path);
        char **etox_list_font_style_path(int *number_return);
	char etox_clip_rect_new(Etox *e, double x, double y, double w,double h);
	char *etox_get_text(Etox *e);
	char etox_show(Etox *e);
	char etox_hide(Etox *e);
	void etox_refresh(Etox *e);
	char etox_set_text(Etox *e, char *new_text);
	char etox_set_layer(Etox *e, int layer);
	int  etox_get_layer(Etox *e);
	void etox_set_color(Etox *e, E_Text_Color *cl);
        void etox_set_alpha_mod(Etox *e, int amod);
        int  etox_get_alpha_mod(Etox *e);
	void etox_set_color_component(Etox *e, char *arg, E_Color component);
	Etox_Bit *Etox_Bit_new(void);
	Etox *Etox_new(char *name);
	Etox *Etox_new_all(Evas *evas, char *name, double x, double y, double w, double h,
			   int layer, char *font, char *font_style, int font_size, E_Text_Color *cl,
			   double padding, int align, int vertical_align);
	void etox_free(Etox *e);
	void etox_clean(Etox *e);
	void etox_move(Etox *e, double x, double y);
	void etox_resize(Etox *e, double w, double h);
   
#ifdef __cplusplus
}
#endif

#endif
