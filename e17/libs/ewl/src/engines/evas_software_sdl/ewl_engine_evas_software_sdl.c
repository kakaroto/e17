/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Engine_Evas_Software_SDL.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static int ewl_ev_sdl_key_down(void *data, int type, void *_ev);
static int ewl_ev_sdl_key_up(void *data, int type, void *_ev);
static int ewl_ev_sdl_mouse_down(void *data, int type, void *_ev);
static int ewl_ev_sdl_mouse_up(void *data, int type, void *_ev);
static int ewl_ev_sdl_mouse_move(void *data, int type, void *_ev);

static Ecore_Event_Handler *ee_key_down_handler = NULL;
static Ecore_Event_Handler *ee_key_up_handler = NULL;
static Ecore_Event_Handler *ee_mouse_move_handler = NULL;
static Ecore_Event_Handler *ee_mouse_down_handler = NULL;
static Ecore_Event_Handler *ee_mouse_up_handler = NULL;

static Ecore_Idler *ewl_event = NULL;

static void ee_canvas_setup(Ewl_Window *win, int debug);
static int ee_init(Ewl_Engine *engine);
static void ee_shutdown(Ewl_Engine *engine);

static void *canvas_funcs[EWL_ENGINE_CANVAS_MAX] =
        {
                ee_canvas_setup,
                NULL, NULL, NULL, NULL
        };

Ecore_DList *
ewl_engine_dependancies(void)
{
        Ecore_DList *d;

        DENTER_FUNCTION(DLEVEL_STABLE);

        d = ecore_dlist_new();
        ecore_dlist_append(d, strdup("evas"));

        DRETURN_PTR(d, DLEVEL_STABLE);
}

Ewl_Engine *
ewl_engine_create(int *argc __UNUSED__, char ** argv __UNUSED__)
{
        Ewl_Engine_Evas_Software_SDL *engine;

        DENTER_FUNCTION(DLEVEL_STABLE);

        engine = NEW(Ewl_Engine_Evas_Software_SDL, 1);
        if (!engine)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ee_init(EWL_ENGINE(engine)))
        {
                FREE(engine);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(EWL_ENGINE(engine), DLEVEL_STABLE);
}

static int
ewl_sdl_event(void *data)
{
        ecore_sdl_feed_events();
        return 1;
}


static int
ewl_idler_event_setup(void *data __UNUSED__)
{
        ewl_event = ecore_timer_add(0.008, ewl_sdl_event, NULL);
        return 0;
}

static int
ee_init(Ewl_Engine *engine)
{
        Ewl_Engine_Info *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(engine, TRUE);

        if (ee_key_down_handler)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        if (!ecore_sdl_init(NULL))
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        ee_key_down_handler = ecore_event_handler_add(
                                        ECORE_SDL_EVENT_KEY_DOWN,
                                        ewl_ev_sdl_key_down, engine);
        ee_key_up_handler = ecore_event_handler_add(
                                        ECORE_SDL_EVENT_KEY_UP,
                                        ewl_ev_sdl_key_up, engine);
        ee_mouse_down_handler = ecore_event_handler_add(
                                        ECORE_SDL_EVENT_MOUSE_BUTTON_DOWN,
                                        ewl_ev_sdl_mouse_down, engine);
        ee_mouse_up_handler = ecore_event_handler_add(
                                        ECORE_SDL_EVENT_MOUSE_BUTTON_UP,
                                        ewl_ev_sdl_mouse_up, engine);
        ee_mouse_move_handler = ecore_event_handler_add(
                                        ECORE_SDL_EVENT_MOUSE_MOVE,
                                        ewl_ev_sdl_mouse_move, engine);

        if (!ee_key_down_handler || !ee_key_up_handler
                        || !ee_mouse_down_handler || !ee_mouse_up_handler
                        || !ee_mouse_move_handler)
        {
                ee_shutdown(engine);
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        info = NEW(Ewl_Engine_Info, 1);
        info->shutdown = ee_shutdown;
        info->hooks.canvas = canvas_funcs;

        engine->functions = info;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ee_shutdown(Ewl_Engine *engine)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(engine);

        if (ee_key_down_handler)
                ecore_event_handler_del(ee_key_down_handler);
        ee_key_down_handler = NULL;

        if (ee_key_up_handler)
                ecore_event_handler_del(ee_key_up_handler);
        ee_key_up_handler = NULL;

        if (ee_mouse_down_handler)
                ecore_event_handler_del(ee_mouse_down_handler);
        ee_mouse_down_handler = NULL;

        if (ee_mouse_up_handler)
                ecore_event_handler_del(ee_mouse_up_handler);
        ee_mouse_up_handler = NULL;

        if (ee_mouse_move_handler)
                ecore_event_handler_del(ee_mouse_move_handler);
        ee_mouse_move_handler = NULL;

        ecore_timer_del(ewl_event);
        ecore_sdl_shutdown();

        IF_FREE(engine->functions);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_canvas_setup(Ewl_Window *win, int debug __UNUSED__)
{
        Evas *evas;
        Evas_Engine_Info *info = NULL;
        Evas_Engine_Info_SDL *sdlinfo;
        Ewl_Object *o;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        evas = evas_new();
        evas_output_method_set(evas, evas_render_method_lookup("software_sdl"));

        info = evas_engine_info_get(evas);
        if (!info)
        {
                fprintf(stderr, "Unable to use evas_software_sdl engine for rendering, ");
                exit(-1);
        }

        win->window = sdlinfo = (Evas_Engine_Info_SDL *)info;

        sdlinfo->info.fullscreen = 0;
        sdlinfo->info.hwsurface = 1;
        sdlinfo->info.noframe = 0;
        sdlinfo->info.alpha = 1;
        evas_engine_info_set(evas, (Evas_Engine_Info *)sdlinfo);

        o = EWL_OBJECT(win);
        evas_engine_info_set(evas, info);
        evas_output_size_set(evas, ewl_object_current_w_get(o),
                                        ewl_object_current_h_get(o));
        evas_output_viewport_set(evas, ewl_object_current_x_get(o),
                                        ewl_object_current_y_get(o),
                                        ewl_object_current_w_get(o),
                                        ewl_object_current_h_get(o));

        ecore_idler_add(ewl_idler_event_setup, NULL);

        ewl_embed_canvas_set(EWL_EMBED(win), evas, win->window);

        EWL_ENGINE_EVAS_SOFTWARE_SDL(EWL_EMBED(win)->engine)->window = win;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_ev_sdl_key_down(void *data, int type __UNUSED__, void *e)
{
        Ewl_Embed *embed;
        Ewl_Engine_Evas_Software_SDL *engine;
        Ecore_Sdl_Event_Key_Down *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;
        engine = data;

        embed = EWL_EMBED(engine->window);
        if (!embed)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        ewl_embed_key_down_feed(embed, ev->keyname, ewl_ev_modifiers_get());

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_sdl_key_up(void *data, int type __UNUSED__, void *e)
{
        Ewl_Embed *embed;
        Ewl_Engine_Evas_Software_SDL *engine;
        Ecore_Sdl_Event_Key_Up *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;
        engine = data;

        embed = EWL_EMBED(engine->window);
        if (!embed)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        ewl_embed_key_down_feed(embed, ev->keyname, ewl_ev_modifiers_get());

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_sdl_mouse_down(void *data, int type __UNUSED__, void *e)
{
        int clicks = 1;
        Ewl_Embed *embed;
        Ewl_Engine_Evas_Software_SDL *engine;
        Ecore_Sdl_Event_Mouse_Button_Down *ev;
        unsigned int key_modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;
        engine = data;

        embed = EWL_EMBED(engine->window);
        if (!embed)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        if (ev->double_click)
                clicks = 2;
        if (ev->triple_click)
                clicks = 3;

        key_modifiers = ewl_ev_modifiers_get();

        /* Feed a mouse move, since they don't occur prior to mouse down on a
         * touchscreen */
        ewl_embed_mouse_move_feed(embed, ev->x, ev->y, key_modifiers);

        ewl_embed_mouse_down_feed(embed, ev->button, clicks, ev->x, ev->y,
                                  key_modifiers);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_sdl_mouse_up(void *data, int type __UNUSED__, void *e)
{
        Ewl_Embed *embed;
        Ewl_Engine_Evas_Software_SDL *engine;
        Ecore_Sdl_Event_Mouse_Button_Up *ev;
        unsigned int key_modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;
        engine = data;

        embed = EWL_EMBED(engine->window);
        if (!embed)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        key_modifiers = ewl_ev_modifiers_get();
        ewl_embed_mouse_move_feed(embed, ev->x, ev->y, key_modifiers);
        ewl_embed_mouse_up_feed(embed, ev->button, ev->x, ev->y, key_modifiers);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_sdl_mouse_move(void *data, int type __UNUSED__, void *e)
{
        Ewl_Embed *embed;
        Ewl_Engine_Evas_Software_SDL *engine;
        Ecore_Sdl_Event_Mouse_Move *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;
        engine = data;

        embed = EWL_EMBED(engine->window);
        if (!embed)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        ewl_embed_mouse_move_feed(embed, ev->x, ev->y,
                                        ewl_ev_modifiers_get());

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

