
#ifndef ETOX_H
#define ETOX_H 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <Evas.h>


typedef struct {

	int w,h;
	int x,y;

} E_Clip_Rect;

typedef struct {

	int r,g,b;

} E_Color;

#define STYLE_TYPE_FOREGROUND 0
#define STYLE_TYPE_SHADOW     1
#define STYLE_TYPE_BACKGROUND 2


#define ALIGN_LEFT 1;
#define ALIGN_RIGHT 1;
#define ALIGN_CENTER 2;

typedef struct {

	char type;
	int alpha;
	int x,y;

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
	int x,y,w,h;
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
	int x,y,w,h;
	int layer;
	char *font;
	E_Font_Style *font_style;
	int font_size;
	Etox_Bit **bit_list;
	int num_bits;
	Evas *evas;

} Etox;

#ifdef __cplusplus
extern "C"
{
#endif

	E_Font_Style *E_load_font_style(char *path);
	void E_Font_Style_free(E_Font_Style *style);
	char etox_set_font_style(Etox *e, E_Font_Style *font_style);
	char etox_clip_rect_new(Etox *e, int x, int y, int w,int h);
	char *etox_get_text(Etox *e);
	char etox_show(Etox *e);
	char etox_hide(Etox *e);
	void etox_refresh(Etox *e);
	char etox_set_text(Etox *e, char *new_text);
	char etox_set_layer(Etox *e, int layer);
	int etox_get_layer(Etox *e);
	Etox_Bit *Etox_Bit_new(void);
	Etox *Etox_new(char *name);
	void etox_free(Etox *e);
	char etox_render(Etox *e);
	void etox_clean(Etox *e);

#ifdef __cplusplus
}
#endif

#endif
