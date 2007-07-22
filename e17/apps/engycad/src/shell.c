/*
 * Copyright (C) 2002, Yury Hudobin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "engy.h"
#include <stdlib.h>

/* vars */
Shell              *shell = NULL;
Drawing            *drawing = NULL;
char               *app_name;
int                 my_int = 0;

int                 render_method = -1;

int                 fpsflag = 0;

int                 drag_x, drag_y, drag;
double              old_x, old_y, old_scale;

/* prototypes */
static int         ecore_idle(void *);
//static void         ecore_window_expose(Ecore_Event *);
//static void         ecore_window_configure(Ecore_Event *);
//static void         ecore_mouse_move(Ecore_Event *);
//static void         ecore_mouse_down(Ecore_Event *);
//static void         ecore_mouse_up(Ecore_Event *);

static void         ecore_key_down(Ecore_Event *);
void                _shell_bg_create(void);
void                create_view(void);
void                print_help(char *);
void 		    check_rc_files(void);


static void 
bg_mouse_move(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void 
bg_mouse_down(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void
bg_mouse_up(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void
bg_key_down(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void
bg_key_up(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void
bg_mouse_wheel(void *data, Evas *_e, Evas_Object *_o, void *event_info);


void                shell_drag_start(int, int);
void                shell_drag(int, int);
void                shell_drag_stop(int, int);
void                shell_zoom(int, int);
void                shell_unzoom(int, int);

void 		    shell_shutdown(void);

/**********  main()  ***************/

	int
handler_signal_exit(void *data, int ev_type, void *ev)
{
	Ecore_Event_Signal_Exit *e;
	
	e = ev;
	if (e->interrupt) printf("exit: interrupt\n");
	if (e->quit)      printf("exit: quit\n");
	if (e->terminate) printf("exit: terminate\n");
	
	ecore_main_loop_quit();
	return 1;
}

	static void
engy_pre_rend(Ecore_Evas *ee)
{
//	edje_thaw();
}

	static void
engy_post_rend(Ecore_Evas *ee)
{
//	edje_freeze();
}

	static void
engy_delete_request(Ecore_Evas *ee)
{
	ecore_main_loop_quit();
}

	static void
engy_resize(Ecore_Evas * ee)
{
	Evas_Coord w, h;
	
	if (!shell->evas)
		return;
	
	evas_output_viewport_get(shell->evas, NULL, NULL, &w, &h);

	shell->w = (int)w;
	shell->h = (int)h;
	evas_object_resize(shell->o_bg, shell->w, shell->h);

        cl_configure(w,h);
        log_configure();
        info_configure();
        panel_configure();
        gui_put_string(DUP("_zoom_in|_zoom_out"));
}


int
//main(int argc, char *argv[], char *envp[]) GLS
main(int argc, char *argv[], char *envp[])
{
    int                 i, res;
    int                 k[20];
    char               *mode = "s";
    int                 flag = 0;

    app_name = argv[0];
    setlocale(LC_ALL, "");

    shell = (Shell *) malloc(sizeof(Shell));
    ENGY_ASSERT(shell);
    memset(shell, 0, sizeof(Shell));
    shell->title = TITLE;

    check_rc_files();

    {
        char                buf[4096];
        char               *home;

        home = getenv("HOME");
        if(!home || (strlen(home)==0)){
	    fprintf(stderr,"check out $HOME\n");
	    exit(-1);
	}
	
	if(strlen(home)>1000) {
	    fprintf(stderr,"check out $HOME\n");
	    exit(-1);
	}

	sprintf(buf, "%s/engycad", home);
	shell->home = DUP(buf);
    }

    {
        struct timeval      tv;

        gettimeofday(&tv, NULL);
        srand(tv.tv_usec);
    }

    for (i = 0; i < 20; i++)
        k[i] = 1;
    k[0] = 0;

    for (i = 1; i < argc; i++)
      {
          if (!strcmp(argv[i], "--fps"))
            {
                fpsflag = 1;
            }
// GLS
/*          if ((!strcmp(argv[i], "-m")) || (!strcmp(argv[i], "--mode")))
            {
                k[i] = 0;
                k[i + 1] = 0;
                if (!strcmp(argv[i + 1], "software"))
                  {
                      render_method = RENDER_METHOD_ALPHA_SOFTWARE;
                      mode = "s";
                  }
                if (!strcmp(argv[i + 1], "x11"))
                  {
                      render_method = RENDER_METHOD_BASIC_HARDWARE;
                      mode = "x11";
                  }
                if (!strcmp(argv[i + 1], "3d"))
                  {
                      render_method = RENDER_METHOD_3D_HARDWARE;
                      mode = "3d";
                  }
            }
*/
          if (!strcmp(argv[i], "--rcfile") || !strcmp(argv[i], "-f"))
            {
		IF_FREE(shell->rcfile);
                shell->rcfile = DUP(argv[i + 1]);		
                k[i] = 0;
                k[i + 1] = 0;
            }

          if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--serv"))
            {
                my_run("caddserv &");
                exit(0);
            }

          if (!strcmp(argv[i], "-h"))
            {
                print_help(argv[0]);
                exit(0);
            }
          if (!strcmp(argv[i], "--help"))
            {
                print_help(argv[0]);
                exit(0);
            }

      }

    for (i = argc; i < 20; i++)
        k[i] = 0;

    /* starting multiple instances */
    for (i = 0; i < 20; i++)
        if (k[i])
            flag++;
    if (flag > 1)
      {
          char                buf[4096];

          flag = 0;
          for (i = 1; i < 20; i++)
            {
                if (k[i])
                  {
                      snprintf(buf, 4000,"%s -f %s -m %s %s &",
                              argv[0], shell->rcfile, mode, argv[i]);
                      my_run(buf);
                  }
            }
          exit(0);
      }

    /* single mode runing */
    for (i = 0; i < 20; i++)
        if (k[i])
            shell->drawingfile = argv[i];

    {
        char                buf[4096];
        char               *s;

        E_DB_STR_GET(shell->rcfile, "/home", s, res);
        if(res){
	    IF_FREE(shell->home);
	    shell->home = s;
	} else {
	    IF_FREE(shell->home);
	    shell->home = DUP(PACKAGE_DATA_DIR);
	}
	

        E_DB_STR_GET(shell->rcfile, "/menufile", s, res);
        ENGY_ASSERT(res);
        if (s[0] != '/')
          {
              snprintf(buf, 4000,"%s/%s", shell->home, s);
              shell->menu_file = DUP(buf);
              FREE(s);
          }
        else
          {
              shell->menu_file = s;
          }

        E_DB_STR_GET(shell->rcfile, "/iconsfile", s, res);
        ENGY_ASSERT(res);
        if (s[0] != '/')
          {
              snprintf(buf, 4000, "%s/%s", shell->home, s);
              shell->icons_file = DUP(buf);
              FREE(s);
          }
        else
          {
              shell->icons_file = s;
          }

        E_DB_STR_GET(shell->rcfile, "/dimstylesfile", s, res);
        ENGY_ASSERT(res);
        if (s[0] != '/')
          {
              snprintf(buf, 4000, "%s/%s", shell->home, s);
              shell->dim_styles_file = DUP(buf);
              FREE(s);
          }
        else
          {
              shell->dim_styles_file = s;
          }

        E_DB_STR_GET(shell->rcfile, "/textstylesfile", s, res);
        ENGY_ASSERT(res);
        if (s[0] != '/')
          {
              snprintf(buf, 4000, "%s/%s", shell->home, s);
              shell->text_styles_file = DUP(buf);
              FREE(s);
          }
        else
          {
              shell->text_styles_file = s;
          }

        E_DB_STR_GET(shell->rcfile, "/linestylesfile", s, res);
        ENGY_ASSERT(res);
        if (s[0] != '/')
          {
              snprintf(buf, 4000, "%s/%s", shell->home, s);
              shell->line_styles_file = DUP(buf);
              FREE(s);
          }
        else
          {
              shell->line_styles_file = s;
          }

        E_DB_STR_GET(shell->rcfile, "/pointstylesfile", s, res);
        ENGY_ASSERT(res);
        if (s[0] != '/')
          {
              snprintf(buf, 4000, "%s/%s", shell->home, s);
              shell->point_styles_file = DUP(buf);
              FREE(s);
          }
        else
          {
              shell->point_styles_file = s;
          }

        E_DB_STR_GET(shell->rcfile, "/hatchstylesfile", s, res);
        ENGY_ASSERT(res);
        if (s[0] != '/')
          {
              snprintf(buf, 4000, "%s/%s", shell->home, s);
              shell->hatch_styles_file = DUP(buf);
              FREE(s);
          }
        else
          {
              shell->hatch_styles_file = s;
          }

    }
   
    // GLS
    /*
    {
	char                buf[4096];
	char               *td;
	
	E_DB_STR_GET(shell->rcfile, "/textdomain", td, res);
	if(!res){
	    td = DUP(PACKAGE_LOCALE_DIR);
	}
	if(td[0]!='/'){
	    snprintf(buf, 4000, "%s/%s", shell->home, td);
	    bindtextdomain("engycad", buf);
	    FREE(td);
	} else {
	    bindtextdomain("engycad", td);
	    FREE(td);
	}
    }   
    */

    if (!ecore_init()) return -1;
    ecore_app_args_set(argc, (const char **)argv);
    
    ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT,
		    handler_signal_exit, NULL);
    
    shell_init();
    ecore_main_loop_begin();
    shell_shutdown();
    ecore_evas_shutdown();
    return 0;
}

void
shell_init(void)
{

/*    ecore_display_init(NULL);
    ecore_event_signal_init();
    ecore_event_filter_init();
    ecore_event_x_init();

    ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_EXPOSE,
                                   ecore_window_expose);
    ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_MOVE, ecore_mouse_move);
    ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_CONFIGURE,
                                   ecore_window_configure);
    ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_DOWN, ecore_mouse_down);
    ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_UP, ecore_mouse_up);
    ecore_event_filter_handler_add(ECORE_EVENT_KEY_DOWN, ecore_key_down);
*/
    ecore_idle_enterer_add(ecore_idle, NULL);

    create_view();
    drawing_load();
}

/* beans */

static int 
ecore_idle(void *data)
{
    static double       last = 0;
    static int          cnt = 0;


    cnt += fpsflag;
    if (cnt == 100)
      {
          cnt = 0;
          printf("%f\n", 1 / ((ecore_time_get() - last) / 100));
          last = ecore_time_get();
      }
    gui_apply();
    gra_apply();
    return 1;
}

void
_get_title_dcd(void)
{
    char               *fr, *to;
    int                 res;

    E_DB_STR_GET(shell->rcfile, "/main_win/from", fr, res);
    ENGY_ASSERT(res);
    E_DB_STR_GET(shell->rcfile, "/main_win/to", to, res);
    ENGY_ASSERT(res);

    shell->title_dcd = iconv_open(to, fr);
    ENGY_ASSERTS(((long)shell->title_dcd != -1L), "iconv_open");
    FREE(fr);
    FREE(to);

    E_DB_STR_GET(shell->rcfile, "/main_win/cl/from", fr, res);
    ENGY_ASSERT(res);
    E_DB_STR_GET(shell->rcfile, "/main_win/cl/to", to, res);
    ENGY_ASSERT(res);

    shell->dcd = iconv_open(to, fr);
    ENGY_ASSERTS(((long)shell->dcd != -1L), "iconv_open");
    FREE(fr);
    FREE(to);

    E_DB_INT_GET(shell->rcfile, "/main_win/cl/charmap", shell->charmap, res);
    ENGY_ASSERT(res);
}

void
create_view(void)
{
//    Window              win, ewin;
    Evas               *evas;
    Ecore_Evas * ee;
    int                 x, y, w, h, res;
    int                 maxcol;
    int                 engine;
    char               *fontdir;
    int                 font_cache, image_cache;
    char buf[4096];
    char *s;

    E_DB_INT_GET(shell->rcfile, "/main_win/win_x", x, res);
    ENGY_ASSERT(res);

    E_DB_INT_GET(shell->rcfile, "/main_win/win_y", y, res);
    ENGY_ASSERT(res);

    E_DB_INT_GET(shell->rcfile, "/main_win/win_w", w, res);
    ENGY_ASSERT(res);

    E_DB_INT_GET(shell->rcfile, "/main_win/win_h", h, res);
    ENGY_ASSERT(res);

    E_DB_STR_GET(shell->rcfile, "/aliases", (shell->aliases), res);
    ENGY_ASSERT(res);

    E_DB_INT_GET(shell->rcfile, "/maxcolors", maxcol, res);
    ENGY_ASSERT(res);

    E_DB_INT_GET(shell->rcfile, "/rendermethod", engine, res);
    ENGY_ASSERT(res);
    if (render_method != -1)
        engine = render_method;

    E_DB_INT_GET(shell->rcfile, "/maximagecache", image_cache, res);
    ENGY_ASSERT(res);

    E_DB_INT_GET(shell->rcfile, "/maxfontcache", font_cache, res);
    ENGY_ASSERT(res);
    
    E_DB_STR_GET(shell->rcfile, "/fontdir", s, res);
    ENGY_ASSERT(res);
    if (s[0] != '/')
    {
	snprintf(buf, 4000,"%s/%s", shell->home, s);
	imlib_add_path_to_font_path(buf);
	fontdir = DUP(buf);
    }
    else
    {
	imlib_add_path_to_font_path(s);
	fontdir = DUP(s);
    }
    FREE(s);
    
    
/*    win = ecore_window_new(0, x, y, w, h);
    ecore_window_set_events(win, XEV_CONFIGURE | XEV_KEY);
    
    evas = evas_new_all(ecore_display_get(),
                        win,
                        0,
                        0,
                        w, h, engine, maxcol, font_cache, image_cache, fontdir);
    ENGY_ASSERTS(evas, "evas_new");
    FREE(fontdir);
    evas_set_output_viewport(evas, 0, 0, w, h);
    ewin = evas_get_window(evas);
    ecore_window_set_events(ewin, XEV_EXPOSE | XEV_BUTTON | XEV_MOUSE_MOVE);
*/

    if (!ecore_x_init(NULL))
	    exit(-1);
	    //LOG_AND_RETURN (ERR_EFL);
    
    if (!ecore_evas_init())
	    exit(-1);
	    //LOG_AND_RETURN (ERR_EFL);
    
    ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 640, 480);
    
    if (!ee)
	    exit(-1);
	    //LOG_AND_RETURN (ERR_EFL);
    
    evas = ecore_evas_get(ee);
    evas_font_path_prepend(evas, fontdir);
    FREE(fontdir);
    ecore_evas_callback_delete_request_set(ee, engy_delete_request);
    ecore_evas_callback_pre_render_set(ee, engy_pre_rend);
    ecore_evas_callback_post_render_set(ee, engy_post_rend);
    ecore_evas_callback_resize_set(ee, engy_resize);
    ecore_evas_name_class_set(ee, "engy", "main");
    ecore_evas_show(ee);
    
    _get_title_dcd();

    shell->title = my_iconv(shell->title_dcd, TITLE);
    ecore_evas_title_set(ee, shell->title);

    shell->evas = evas;
    shell->win = ecore_evas_software_x11_window_get(ee);
    shell->ee = ee;
    shell->w = w;
    shell->h = h;

    // GLS
//    ecore_set_blank_pointer(win);
//    ecore_window_set_title(win, shell->title);
//  my_evas_init();
    _shell_bg_create();
    menu_init();
    // GLS
    engy_cl_init();
    cl_configure(w, h);
    log_init();
    info_init();
    info_sync();
    panel_init();
    pointer_init();
    serv_init();
    logo_init();
    alias_init();
//    evas_render(shell->evas);
}

void
_shell_bg_create(void)
{
    Evas               *e;
    Evas_Object        *o;
    int                 r, g, b, a, res;
    char *s;
    
    e = shell->evas;
    
    
    E_DB_STR_GET (shell->rcfile, "/bgimage", s, res);
    
    if (!res)
    {
	E_DB_INT_GET (shell->rcfile, "/main_win/bgcolor/r", r, res);
	ENGY_ASSERT (res);
	E_DB_INT_GET (shell->rcfile, "/main_win/bgcolor/g", g, res);
	ENGY_ASSERT (res);
	E_DB_INT_GET (shell->rcfile, "/main_win/bgcolor/b", b, res);
	ENGY_ASSERT (res);
	E_DB_INT_GET (shell->rcfile, "/main_win/bgcolor/a", a, res);
	ENGY_ASSERT (res);
	
	o = evas_object_rectangle_add(e);
	evas_object_color_set (o, r*a/255, g*a/255, b*a/255, a);
    }
    else
    {
	char buf[4096];
	
	if (s[0] != '/')
	{
	    snprintf (buf, 4000, "%s/%s", shell->home, s);
	    FREE (s);
	    s = DUP (buf);
	}
	o = evas_image_load(s);
	FREE (s);
    }    
    
    evas_object_move(o, 0, 0);
    evas_object_resize(o, shell->w, shell->h);
    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, bg_mouse_move, NULL);
    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, bg_mouse_down, NULL);
    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, bg_mouse_up, NULL);
    evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, bg_key_down, NULL);
    evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_UP, bg_key_up, NULL);
    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_WHEEL, bg_mouse_wheel, NULL);
    evas_object_layer_set(o, 0);
    evas_object_show(o);
    evas_object_focus_set(o, 1);

    shell->o_bg = o;
    shell->context.color.red = 0;
    shell->context.color.green = 0;
    shell->context.color.blue = 0;
    shell->context.color.alpha = 255;
    shell->context.line_style = DUP("dashed2");
    shell->context.thickness = 0.5;
    shell->context.show_thickness = 1;
    shell->context.line_scale = 1.0;
    shell->context.textstyle = DUP("gost");
    shell->context.text_height = 5.0;
    shell->context.point_style = DUP("cross");
    shell->context.dim_style = DUP("gost-5-3");
    shell->context.hatch_style = DUP("glass");
    shell->context.gravity = GRAVITY_C | GRAVITY_M;
    shell->prec = 10.0;
    shell->segnum = 8;
    shell->arcnum = 20;
    shell->monitor = 96;
    shell->paper = 100;
    shell->float_prec = 3;
    shell->snap = 0.005;
}
/*
static void
ecore_window_expose(Ecore_Event * ev)
{
    Ecore_Event_Window_Expose *ee;

    ee = (Ecore_Event_Window_Expose *) ev->event;
    evas_update_rect(shell->evas, ee->x, ee->y, ee->w, ee->h);
}

static void
ecore_window_configure(Ecore_Event * ev)
{
    Ecore_Event_Window_Configure *ee;
    static              old_w, old_h;

    ee = (Ecore_Event_Window_Configure *) ev->event;
    if (ee->win == shell->app_win)
      {
          shell->w = ee->w;
          shell->h = ee->h;
          ecore_window_resize(shell->evas_win, shell->w, shell->h);
          evas_set_output_size(shell->evas, shell->w, shell->h);
          evas_set_output_viewport(shell->evas, 0, 0, shell->w, shell->h);
          evas_resize(shell->evas, shell->o_bg, shell->w, shell->h);

          if ((old_w != ee->w) || (old_h != ee->h))
            {
                cl_configure();
                log_configure();
                info_configure();
                panel_configure();
                gui_put_string(DUP("_zoom_in|_zoom_out"));
                old_w = ee->w;
                old_h = ee->h;
            }
      }
}

static void
ecore_key_down(Ecore_Event * ev)
{
    Ecore_Event_Key_Down *ee;

    ee = (Ecore_Event_Key_Down *) ev->event;
    cl_handle_key(ee->key, ee->mods, ee->compose);
}

static void
ecore_mouse_down(Ecore_Event * ev)
{
    Ecore_Event_Mouse_Down *ee;

    ee = (Ecore_Event_Mouse_Down *) ev->event;
    if (ee->win != shell->evas_win)
        return;
    evas_event_button_down(shell->evas, ee->x, ee->y, ee->button);
    if (ee->button == 2)
        shell_drag_start(ee->x, ee->y);
    if ((ee->mods == 2) && (ee->button == 1))
        shell_zoom(ee->x, ee->y);
    if ((ee->mods == 2) && (ee->button == 3))
        shell_unzoom(ee->x, ee->y);

}

static void
ecore_mouse_up(Ecore_Event * ev)
{
    Ecore_Event_Mouse_Up *ee;

    ee = (Ecore_Event_Mouse_Up *) ev->event;
    if (ee->win != shell->evas_win)
        return;
    evas_event_button_up(shell->evas, ee->x, ee->y, ee->button);
    if (ee->button == 2)
        shell_drag_stop(ee->x, ee->y);
}

static void
ecore_mouse_move(Ecore_Event * ev)
{
    Ecore_Event_Mouse_Move *ee;

    ee = (Ecore_Event_Mouse_Move *) ev->event;
    if (ee->win != shell->evas_win)
        return;
    evas_event_move(shell->evas, ee->x, ee->y);
    pointer_move(ee->x, ee->y);
    shell_drag(ee->x, ee->y);

}
*/
/* bg callbacks */

static void
bg_key_down(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
	Evas_Event_Key_Down *ee = (Evas_Event_Key_Down*) event_info;
	
	if(!strcmp(ee->keyname, "Shift_L") 
			|| !strcmp(ee->keyname, "Shift_R"))
		shell->is_shifted = 1;

	cl_handle_key(_o, event_info);
}

static void
bg_key_up(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
	Evas_Event_Key_Up *ee = (Evas_Event_Key_Up*) event_info;
	
	if(!strcmp(ee->keyname, "Shift_L") 
			|| !strcmp(ee->keyname, "Shift_R"))
		shell->is_shifted = 0;
}

static void
bg_mouse_wheel(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
	Evas_Event_Mouse_Wheel *ee = (Evas_Event_Mouse_Wheel*) event_info;
	
	if(ee->z > 0) 
		shell_unzoom(ee->output.x, ee->output.y);
	else if(ee->z < 0)
		shell_zoom(ee->output.x, ee->output.y);
}

static void
bg_mouse_move(void *data, Evas *_e, Evas_Object *_o, void * event_info)
{
    Drawing            *d;
    Evas_Coord              x, y;
    Evas_Coord              ex, ey, ew, eh;
    Evas_Coord		_x, _y;

    Evas_Event_Mouse_Move *em = (Evas_Event_Mouse_Move*) event_info;

    _x = em->cur.canvas.x;
    _y = em->cur.canvas.y;
    magnet_mouse_move(_x, _y);
    info_sync_mouse();

    evas_output_viewport_get(shell->evas, &ex, &ey, &ew, &eh);
    d = drawing;
    if (!d)
        return;
    x = _x / d->scale - d->x;
    y = -(_y - eh - ey) / d->scale - d->y;
    shell_drag(em->cur.output.x, em->cur.output.y);
    gra_pre_draw_mouse_move(x, y);
}

static void
bg_mouse_down(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Drawing            *d;
    double              x, y, _x, _y;
    Evas_Coord          ex, ey, ew, eh;
    int                 shift_state, ctrl_state;
    int 		_b;
    Evas_Event_Mouse_Down * em = (Evas_Event_Mouse_Down*) event_info;

    if (!shell)
        return;
    if (!shell->evas)
        return;

    _b = em->button;
    shift_state = evas_key_modifier_is_set(em->modifiers, "Shift"); 
    ctrl_state = evas_key_modifier_is_set(em->modifiers, "Control");

    evas_output_viewport_get(shell->evas, &ex, &ey, &ew, &eh);
    d = drawing;
    if (!d)
        return;
    _x = em->canvas.x;
    _y = em->canvas.y;

    if (!ctrl_state && (_b == 1))
      {
          x = _x / d->scale - d->x;
          y = -(_y - eh - ey) / d->scale - d->y;
          gra_pre_draw_mouse_click(x, y);
      }

    if (!shift_state && !ctrl_state && (_b == 3))
      {
          menu_box_show(DUP("context.menu"), (double)_x + 1, (double)_y + 1, 1);
      }

    if (_b == 2)
        shell_drag_start(_x, _y);
    if (ctrl_state && (_b == 1))
        shell_zoom(_x, _y);
    if (ctrl_state && (_b == 3))
        shell_unzoom(_x, _y);     
}

static void
bg_mouse_up(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
	Evas_Event_Mouse_Up *em = (Evas_Event_Mouse_Up*)event_info;
	if (em->button == 2)
		shell_drag_stop(em->output.x, em->output.y);
}

/* view moving/zooming  */

void
shell_drag_start(int x, int y)
{

    if (!drawing)
        return;
    pointer_push_and_set(0x1500 + POINTER_HAND);
    drag = 1;
    drag_x = x;
    drag_y = y;
    lock_data();
    drawing->old.x = drawing->x;
    drawing->old.y = drawing->y;
    unlock_data();
}

void
shell_drag(int x, int y)
{

    if (!drag)
        return;
    if (!drawing)
        return;

    lock_data();
    drawing->x += ((float)(x - drag_x)) / drawing->scale;
    drawing->y -= ((float)(y - drag_y)) / drawing->scale;
    unlock_data();
    drag_x = x;
    drag_y = y;

    drawing_sync();
}

void
shell_drag_stop(int x, int y)
{

    if (!drawing)
        return;
    lock_data();

    drawing->x += ((float)(x - drag_x)) / drawing->scale;
    drawing->y -= ((float)(y - drag_y)) / drawing->scale;

    append_undo_double((void *)(&(drawing->x)),
                       drawing->old.x, drawing->x, 1, OBJ_DRAWING, NULL);
    append_undo_double((void *)(&(drawing->y)),
                       drawing->old.y, drawing->y, 0, 0, NULL);
    unlock_data();
    drag_x = x;
    drag_y = y;

    drawing_sync();
    drag = 0;
    pointer_pop(0x1500 + POINTER_HAND);
}

void
shell_zoom(int x, int y)
{
    Evas_Coord              ex, ey, ew, eh;

    if (!drawing)
        return;
    evas_output_viewport_get(shell->evas, &ex, &ey, &ew, &eh);

    lock_data();
    y = eh + ey - y;

    drawing->old.x = drawing->x;
    drawing->old.y = drawing->y;
    drawing->old.scale = drawing->scale;

    drawing->x -= x / drawing->scale;
    drawing->y -= y / drawing->scale;
    drawing->scale *= 1.25;
    drawing->x += x / drawing->scale;
    drawing->y += y / drawing->scale;

    append_undo_double((void *)(&(drawing->x)), drawing->old.x,
                       drawing->x, 1, OBJ_DRAWING, NULL);
    append_undo_double((void *)(&(drawing->y)), drawing->old.y,
                       drawing->y, 0, 0, NULL);
    append_undo_double((void *)(&(drawing->scale)), drawing->old.scale,
                       drawing->scale, 0, 0, NULL);

    unlock_data();
    drawing_sync();
}

void
shell_unzoom(int x, int y)
{
    Evas_Coord              ex, ey, ew, eh;

    if (!drawing)
        return;
    evas_output_viewport_get(shell->evas, &ex, &ey, &ew, &eh);

    lock_data();
    y = eh + ey - y;

    drawing->old.x = drawing->x;
    drawing->old.y = drawing->y;
    drawing->old.scale = drawing->scale;

    drawing->x -= x / drawing->scale;
    drawing->y -= y / drawing->scale;
    drawing->scale /= 1.25;
    drawing->x += x / drawing->scale;
    drawing->y += y / drawing->scale;

    append_undo_double((void *)(&(drawing->x)), drawing->old.x,
                       drawing->x, 1, OBJ_DRAWING, NULL);
    append_undo_double((void *)(&(drawing->y)), drawing->old.y,
                       drawing->y, 0, 0, NULL);
    append_undo_double((void *)(&(drawing->scale)), drawing->old.scale,
                       drawing->scale, 0, 0, NULL);

    unlock_data();
    drawing_sync();
}

void
print_help(char *s)
{

    setlocale(LC_ALL, "");    
/*    {     
	char                buf[4096];
	char               *td;
	int res;
	
	E_DB_STR_GET(shell->rcfile, "/textdomain", td, res);
	if(!res){
	    td = DUP(PACKAGE_LOCALE_DIR);
	}
	if(td[0]!='/'){
	    snprintf(buf, 4000, "%s/%s", shell->home, td);
	    bindtextdomain("engycad", buf);
	    FREE(td);
	} else {
	    bindtextdomain("engycad", td);
	    FREE(td);
	} 
    }
    */
    printf(_("usage: %s [OPTIONS] FILES\n"
             "Options are:\n"
             "    -h, --help        Print this message\n"
             "    -s, --serv        Start TCP/IP clipboard\n"
             "    -m, --mode mode   where mode is 'software', 'x11', or '3d' \n"
             "                      -- to run program on software, XWindows, \n"
             "                      or OpenGL (Mesa) engine respectively\n"
             "Engy 0.3.0 -- Simple eye-candy CAD environment\n"
             "(c) 2002 Yuri Hudobin\n"), s);
}


void check_rc_files(void){
    char                buf[4096];
    char               *home;
    FILE *test;
    int res;
    
    home = getenv("HOME");
    if(!home || (strlen(home)==0)){
	fprintf(stderr,"check out $HOME\n");
	exit(-1);
    }
    
    if(strlen(home)>1000) {
	fprintf(stderr,"$HOME is too long\n");
	exit(-1);
    }
    
    
    sprintf(buf, "%s/engycad/.engycad.db", home);
    test = fopen(buf,"r");
    if(!test){
	snprintf(buf,4000,"%s/engycad", home);
	res = mkdir(buf,0755);
	if(res==-1){
	    fprintf(stderr,"Cannot create $HOME/engycad dir!\n");
	}
	
	snprintf(buf, 4000, 
			"cp %s/resources/tools/.engycad.db %s/engycad/",
			PACKAGE_DATA_DIR, home);
	printf("%s\n", buf);
	system(buf);

	sprintf(buf, "%s/engycad/.engycad.db",home);
	test = fopen(buf,"r");
	if(!test){
	   fprintf(stderr,
		"Cannot create rcfile!\n");
	   exit(-1);
	}
    } else {
	fclose(test);
    }

    shell->rcfile = DUP(buf);
}               








void
shell_shutdown(void)
{
	E_DB_FLUSH;

	serv_put_string(DUP("__exit"));
	usleep(10000);
	gui_apply();
	alias_shutdown();
	cl_shutdown();
	menu_shutdown();
	panel_shutdown();
	drawing_free();

	if((long)shell->title_dcd != -1L)
		iconv_close(shell->title_dcd);

	if((long)shell->dcd != -1L)
		iconv_close(shell->dcd);


	ecore_evas_free(shell->ee);

	IF_FREE(shell->title);
	IF_FREE(shell->rcfile);
	IF_FREE(shell->menu_file);
	IF_FREE(shell->icons_file);
	IF_FREE(shell->dim_styles_file);
	IF_FREE(shell->text_styles_file);
	IF_FREE(shell->line_styles_file);
	IF_FREE(shell->point_styles_file);
	IF_FREE(shell->hatch_styles_file);
	IF_FREE(shell->home);
	IF_FREE(shell->aliases);

}

