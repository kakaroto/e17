#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <signal.h>
#include <math.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <Ecore.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ecore_Job.h>
#include <Ecore_X.h>
#include "config.h"

#define DEBUG1 0

#define NPAR 16
#define DATADIR PACKAGE_DATA_DIR"/"

#ifdef DEBUG
#define DPRINT(stuff) fprintf stuff
#else
#define DPRINT(stuff)
#endif

int TERM_EVENT_TITLE_CHANGE;

enum Term_Key_Modifiers {
     TERM_KEY_MODIFIER_SHIFT = 0x1,
     TERM_KEY_MODIFIER_CTRL = 0x2,
     TERM_KEY_MODIFIER_ALT = 0x4,
     TERM_KEY_MODIFIER_MOD = 0x8,
     TERM_KEY_MODIFIER_WIN = 0x10,
};
typedef enum Term_Key_Modifiers Term_Key_Modifiers;

struct _Term_Event_Title_Change {
   char *title;
};
typedef struct _Term_Event_Title_Change Term_Event_Title_Change;

/* are those still global vars? if so, remove them */
//char *ptydev, *ttydev;
pid_t pid;

struct _Term_Fd {
   int               sys;
   Ecore_Fd_Handler *ecore;
};

/* Use UTF-8 internally :) */
struct _Term_Char {
   int nbchar;
   char car[6];
};
typedef struct _Term_Char Term_Char;

typedef struct _Term_Fd Term_Fd;

struct _Term_TGlyph {
   /*char c;*/
   char *uc;/* NEW */
   int  nbc;/* NEW */
   int  bg;
   int  fg;
   int  changed;
};
typedef struct _Term_TGlyph Term_TGlyph;

struct _Term_TCanvas {
   int           canvas_id;
   /*
   int           rows;
   int           cols;
   */
   int           cur_fg;
   int           cur_bg;
   int          *changed_rows;
   int           app_keypad_mode;
   int           size;
   int           pos;
   Term_TGlyph **grid;
};
typedef struct _Term_TCanvas Term_TCanvas;

struct _Term_Font {
   char  path[PATH_MAX];
   char  face[PATH_MAX];
   int   size;
   int   width;
   int   height;
};
typedef struct _Term_Font Term_Font;

struct _Term_EGlyph {
   Evas_Object *text;
#if 0
   Evas_Object *bg;
#endif
};
typedef struct _Term_EGlyph Term_EGlyph;

struct _Term_Cursor {
   Evas_Object *shape;
   double       last_reset;
};
typedef struct _Term_Cursor Term_Cursor;

struct _Term {
   int            term_id;
   pid_t          pid;
   Ecore_Evas    *ee;
   Evas          *evas;
   Evas_Object   *bg;
   Term_TCanvas  *tcanvas;
   Term_Cursor    cursor;
   Term_Font      font;
   Term_EGlyph  **grid;
   int		  debug;/* Temp */
   Term_Fd        cmd_fd;
   Term_Fd        slave;
   char           data[512];
   int            data_ptr;
   int            data_len;  

   char          *title;

   int            cols;
   int            rows;
   int            cur_col;
   int            cur_row;
   int            w;
   int            h;

   int            scroll_in_region;
   int            scroll_region_start;
   int            scroll_region_end;

   int            saved_cursor_x;
   int            saved_cursor_y;
};
typedef struct _Term Term;

Evas_Object    *term_new(Evas *evas);
Term           *term_init(Evas_Object *o);
Term_TCanvas   *term_tcanvas_new(Term *term);
int             term_tcanvas_data(void *data, Ecore_Fd_Handler *fd_handler);
void            term_tcanvas_glyph_push(Term *term, Term_Char *c);
void            term_tcanvas_fg_color_set(Term *term, int c);
void            term_tcanvas_bg_color_set(Term *term, int c);
Term_Char      *term_tcanvas_data_pop(Term *term);
int             term_font_get_width(Term *term);
int             term_font_get_height(Term *term);

void            term_handler_xterm_seq(int op, Term *term);
int             term_handler_escape_seq(Term *term);

void            term_cb_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
void            term_cb_resize(Ecore_Evas *ee);

struct winsize *get_font_dim(Term *term);
int             get_pty(Term *term);
int             get_tty(Term *term);
void            sigchld_handler(int a);
struct passwd  *find_user(void);
int             execute_command(Term *term);//, int argc, const char **argv);

void            term_window_init(Ecore_Evas *ee, Evas *evas);
void            term_term_bg_set(Term *term, char *img);
int             term_redraw(void *data);

int             term_cursor_move_up(Term *term, int n);
int             term_cursor_move_down(Term *term, int n);
int             term_cursor_move_left(Term *term, int n);
int             term_cursor_move_right(Term *term, int n);
int             term_cursor_move_col(Term *term, int n);
int             term_cursor_move_row(Term *term, int n);
void            term_cursor_goto(Term *term, int x, int y);
void            term_cursor_rego(Term *term);
void            term_delete_rows(Term *term, int start, int n);
void            term_add_rows(Term *term, int pos, int n);
void            term_tcanvas_save(Term *term);
void            term_tcanvas_restore(Term *term);
void            term_clear_area(Term *term, int x1, int y1, int x2, int y2);
void            term_delete_lines(Term *term, int lines);
void            term_scroll_up(Term *term, int rows);
void            term_scroll_down(Term *term, int rows);
   
void            term_smart_add(Evas_Object *o);
void            term_smart_del(Evas_Object *o);
void            term_smart_layer_set(Evas_Object *o, int l);
void            term_smart_raise(Evas_Object *o);
void            term_smart_lower(Evas_Object *o);
void            term_smart_stack_above(Evas_Object *o, Evas_Object *above);
void            term_smart_stack_below(Evas_Object *o, Evas_Object *below);
void            term_smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
void            term_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
void            term_smart_show(Evas_Object *o);
void            term_smart_hide(Evas_Object *o);
void            term_smart_color_set(Evas_Object *o, int r, int g, int b, int a);
void            term_smart_clip_set(Evas_Object *o, Evas_Object *clip);
void            term_smart_clip_unset(Evas_Object *o);
Evas_Smart     *term_smart_get();

void            term_event_title_change_free(void *data, void *ev);
   
int             term_cursor_anim(Term *term);
