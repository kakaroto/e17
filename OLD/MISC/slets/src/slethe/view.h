#ifndef __SLETS_VIEW_H__
#define __SLETS_VIEW_H__


extern int win_w, win_h;
extern int total_prevs;
extern int total_slots;
extern int total_menus;
extern int menu_end;   
extern int menu_w;
extern int menu_h;

extern char win_title[255];
extern char bg_files[28][255];
extern char icon_files[10][255];
extern char font_path[512];

struct _view_color
{
 int r;
 int g; 
 int b;
 int a;
};

typedef struct _view_color VIEW_COLOR;

struct _view_coords
{
 int x;
 int y;
};

typedef struct _view_coords VIEW_COORDS;

struct _view_prev
{
 VIEW_COORDS coords;
 VIEW_COLOR  color;
 CORE_ROW *r;
 Evas_Object o[3];
};

typedef struct _view_prev VIEW_PREV;

struct _view_slot
{
 VIEW_COORDS coords;
 VIEW_COLOR  color;
 CORE_SLOT *s;
 Evas_Object o[10];
};

typedef struct _view_slot VIEW_SLOT;

struct _view_font
{
 char name[255];
 int size;  
};

typedef struct _view_font VIEW_FONT;

struct _view_menu
{
 VIEW_COLOR color;
 VIEW_FONT  font;
 int x;
 char text[255];
};

typedef struct _view_menu  VIEW_MENU;

struct _view_points
{
 int exists;
 VIEW_COORDS icon;
 VIEW_COORDS text;
};

typedef struct _view_points VIEW_POINTS;

struct _view_text
{
 VIEW_COORDS coords;
 VIEW_FONT   font;
};

typedef struct _view_text VIEW_TEXT;

struct _view_sounds
{
 char startup[255];
 char menu[255];
 char slot[255];
};

typedef struct _view_sounds VIEW_SOUNDS;


extern VIEW_COLOR  text_color;
extern VIEW_COLOR  menu_color;
extern VIEW_COLOR  points_color;
extern VIEW_PREV   **prev;            
extern VIEW_SLOT   **slot;
extern VIEW_FONT   points_font;
extern VIEW_MENU   **menu;
extern VIEW_POINTS points[10];
extern VIEW_TEXT   text;
extern VIEW_SOUNDS snd;


void view_exit(int ret);
int  view_init(void);
void view_main(void);


#endif
