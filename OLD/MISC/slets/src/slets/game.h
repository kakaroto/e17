#ifndef __SLETS_GAME_H__
#define __SLETS_GAME_H__


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

struct _game_color
{
 int r;
 int g; 
 int b;
 int a;
};

typedef struct _game_color GAME_COLOR;

struct _game_coords
{
 int x;
 int y;
};

typedef struct _game_coords GAME_COORDS;

struct _game_prev
{
 GAME_COORDS coords;
 GAME_COLOR  color;
 CORE_ROW *r;
 Evas_Object o[3];
};

typedef struct _game_prev GAME_PREV;

struct _game_slot
{
 GAME_COORDS coords;
 GAME_COLOR  color;
 CORE_SLOT *s;
 Evas_Object o[10];
};

typedef struct _game_slot GAME_SLOT;

struct _game_font
{
 char name[255];
 int size;  
};

typedef struct _game_font GAME_FONT;

struct _game_menu
{
 GAME_COLOR color;
 GAME_FONT  font;
 int x;
 char text[255];
};

typedef struct _game_menu  GAME_MENU;

struct _game_points
{
 int exists;
 GAME_COORDS icon;
 GAME_COORDS text;
};

typedef struct _game_points GAME_POINTS;

struct _game_text
{
 GAME_COORDS coords;
 GAME_FONT   font;
};

typedef struct _game_text GAME_TEXT;

struct _game_sounds
{
 char startup[255];
 char menu[255];
 char slot[255];
};

typedef struct _game_sounds GAME_SOUNDS;


extern GAME_COLOR  text_color;
extern GAME_COLOR  menu_color;
extern GAME_COLOR  points_color;
extern GAME_PREV   **prev;            
extern GAME_SLOT   **slot;
extern GAME_FONT   points_font;
extern GAME_MENU   **menu;
extern GAME_POINTS points[10];
extern GAME_TEXT   text;
extern GAME_SOUNDS snd;


void game_exit(int ret);
void game_put_score(void);
int  game_put_in_slot(int wich);
void game_rehash(void);
int  game_init(void);
void game_main(void);


#endif
