
#ifndef ETOX_H
#define ETOX_H 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

	char *name;
	char *text;

	E_Clip_Rect **rect_list;
	int num_rects;
	char rendered;


} Etox;

#ifdef __cplusplus
extern "C"
{
#endif

	E_Font_Style *E_load_font_style(char *path);
	char etox_set_font_style(E_Font_Style *font_style);
	char etox_clip_rect_new(Etox *e, int x, int y, int w,int h);
	char *etox_get_text(Etox *e);
	char etox_set_text(Etox *e, char *new_text);
	Etox *etox_new(char *name);

#ifdef __cplusplus
}
#endif

#endif
