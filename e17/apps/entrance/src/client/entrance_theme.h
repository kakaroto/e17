#ifndef _ENTRANCE_THEME_H
#define _ENTRANCE_THEME_H

#include <Edb.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

/* Data Structures */
struct _E_Font {
	char *name;
	int size;
	char *style;
};

struct _E_Color {
	int r, g, b, a;
};

struct _E_Pos {
	float x, y;
};

struct _E_Offset {
	int x, y;
};

struct _E_Size {
	int w, h;
};

typedef struct _E_Font E_Font;
typedef struct _E_Color E_Color;
typedef struct _E_Pos E_Pos;
typedef struct _E_Offset E_Offset;
typedef struct _E_Size E_Size;

struct _Entrance_Theme {
	char *name, *path;
	char *bg, *pointer;
	
	struct {
		E_Font font;
		E_Color color;
		E_Pos pos;
		E_Offset offset;
	} welcome;
	
	struct {
		E_Font font;
		E_Color color;
	} password;

	struct {
		E_Font font;
		E_Color color;
		E_Pos pos;
		E_Offset offset;
		struct {
			E_Pos pos;
			E_Offset offset;
			E_Size size;
			E_Color color;
		} box;
	} entry;

	struct {
		struct {
			E_Font font;
			E_Color color, hicolor;
			E_Pos pos;
			E_Offset offset;
		} text;

		struct {
			E_Pos pos;
			E_Offset offset;
			E_Size size;
		} icon;
	} selected_session;

	struct {
		E_Font font, sel_font;
		E_Color color, sel_color, box_color;
		E_Pos pos;
		E_Offset offset;
		E_Size size;
	} session_list;

	struct {
		E_Font font;
		E_Color color;
		E_Pos pos;
		E_Offset offset;
	} hostname, date, time;

	struct {
		E_Pos pos;
		E_Offset offset;
		E_Size size;
		int border;
		E_Color color;
	} face;
};

typedef struct _Entrance_Theme *Entrance_Theme;

/* Functions */
Entrance_Theme entrance_theme_parse(char *name, char *path);

/* Macros */

#define THEME_SETCOLOR(obj, colr) evas_object_color_set(obj, \
                                    (colr).r, (colr).g, (colr).b, (colr).a)
#define THEME_SETFONT(obj, attr) evas_object_text_font_set(obj, (attr).font.name, \
                                    (attr).font.size)
#define THEME_MOVE(obj, attr, g) evas_object_move(obj, \
                                    ((attr).pos.x * (double) (g).w) \
                                    + (double) (attr).offset.x, \
                                    ((attr).pos.y * (double) (g).h) \
                                    + (double) attr.offset.y)
#define THEME_RESIZE(obj, attr) evas_object_resize(obj, \
                                (double) (attr).size.w, (double) (attr).size.h)
#define THEME_IMAGE_RESIZE(obj, attr) THEME_RESIZE(obj, attr); \
                                      evas_object_image_fill_set(obj, 0.0, 0.0, \
                                         (double) (attr).size.w, \
                                         (double) (attr).size.h )
                          
#endif
