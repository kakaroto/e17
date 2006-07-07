#include <Ewl.h>
#include "Ewl_Engine_Evas_Fb.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/*
 * In general all of the X event handlers should find their matching window
 * with ewl_window_window_find, and not ewl_embed_evas_window_find. If the
 * embed function is used, then we get duplicate events for apps that setup
 * their own handlers and embed EWL. The exception to this is selection events
 * such as copy/paste and DND. These events need to be handled for embedded
 * EWL, but have no equivalent in the Evas callback namespace.
 */

static int ewl_ev_fb_key_down(void *data, int type, void *_ev);
static int ewl_ev_fb_key_up(void *data, int type, void *_ev);
static int ewl_ev_fb_mouse_down(void *data, int type, void *_ev);
static int ewl_ev_fb_mouse_up(void *data, int type, void *_ev);
static int ewl_ev_fb_mouse_move(void *data, int type, void *_ev);

static Ecore_Event_Handler *ee_key_down_handler = NULL;
static Ecore_Event_Handler *ee_key_up_handler = NULL;
static Ecore_Event_Handler *ee_mouse_move_handler = NULL;
static Ecore_Event_Handler *ee_mouse_down_handler = NULL;
static Ecore_Event_Handler *ee_mouse_up_handler = NULL;

static void ee_canvas_setup(Ewl_Window *win, int debug);
static int ee_init(Ewl_Engine *engine);
static void ee_shutdown(Ewl_Engine *engine);

static Ewl_Engine_Info engine_funcs = {
	{
                ee_init,
                ee_shutdown,
                NULL, NULL, NULL, NULL,
                NULL, NULL, NULL, NULL,
                NULL, NULL, NULL, NULL,
                NULL, NULL, NULL, NULL,
                NULL, NULL, NULL, NULL,
                NULL, NULL, NULL,
                ee_canvas_setup
	}
};

Ecore_List *
ewl_engine_dependancies(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

Ewl_Engine *
ewl_engine_create(void)
{
        Ewl_Engine_Evas_Fb *engine;

        DENTER_FUNCTION(DLEVEL_STABLE);

        engine = NEW(Ewl_Engine_Evas_Fb, 1);
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
ee_init(Ewl_Engine *engine)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("engine", engine, TRUE);

	if (ee_key_down_handler)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	if (!ecore_fb_init(NULL))
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	ee_key_down_handler = ecore_event_handler_add(
					ECORE_FB_EVENT_KEY_DOWN,
					ewl_ev_fb_key_down, engine);
	ee_key_up_handler = ecore_event_handler_add(
					ECORE_FB_EVENT_KEY_UP, 
					ewl_ev_fb_key_up, engine);
	ee_mouse_down_handler = ecore_event_handler_add(
					ECORE_FB_EVENT_MOUSE_BUTTON_DOWN,
					ewl_ev_fb_mouse_down, engine);
	ee_mouse_up_handler = ecore_event_handler_add(
					ECORE_FB_EVENT_MOUSE_BUTTON_UP,
					ewl_ev_fb_mouse_up, engine);
	ee_mouse_move_handler = ecore_event_handler_add(
					ECORE_FB_EVENT_MOUSE_MOVE,
					ewl_ev_fb_mouse_move, engine);

	if (!ee_key_down_handler || !ee_key_up_handler
			|| !ee_mouse_down_handler || !ee_mouse_up_handler
			|| !ee_mouse_move_handler)
	{
		ee_shutdown(engine);
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

        engine->name = strdup("evas_software_x11");
        engine->functions = &engine_funcs;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ee_shutdown(Ewl_Engine *engine)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("engine", engine);

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

	ecore_fb_shutdown();
	engine->functions = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_canvas_setup(Ewl_Window *win, int debug)
{
	Evas *evas;
	Evas_Engine_Info *info = NULL;
	Evas_Engine_Info_FB *fbinfo;
	Ewl_Object *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	evas = evas_new();
	evas_output_method_set(evas, evas_render_method_lookup("gl_x11"));

	info = evas_engine_info_get(evas);
	if (!info) 
	{
		fprintf(stderr, "Unable to use gl_x11 engine for rendering, ");
		exit(-1);
	}  

	win->window = fbinfo = (Evas_Engine_Info_FB *)info;

	fbinfo->info.virtual_terminal = 0;
	fbinfo->info.device_number = 0;
	fbinfo->info.refresh = 0;
	fbinfo->info.rotation = 0;
	evas_engine_info_set(evas, (Evas_Engine_Info *)fbinfo);
	ewl_object_geometry_request(EWL_OBJECT(win), 0, 0, 240, 320);

	o = EWL_OBJECT(win);
	evas_engine_info_set(evas, info);
	evas_output_size_set(evas, ewl_object_current_w_get(o),
					ewl_object_current_h_get(o));
	evas_output_viewport_set(evas, ewl_object_current_x_get(o),
					ewl_object_current_y_get(o),
					ewl_object_current_w_get(o),
					ewl_object_current_h_get(o));
        ewl_embed_evas_set(EWL_EMBED(win), evas, win->window);

	EWL_ENGINE_EVAS_FB(EWL_EMBED(win)->engine)->window = win;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_ev_fb_key_down(void *data, int type __UNUSED__, void *e)
{
        Ewl_Embed *embed;
	Ewl_Engine_Evas_Fb *engine;
        Ecore_Fb_Event_Key_Down *ev;

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
ewl_ev_fb_key_up(void *data, int type __UNUSED__, void *e)
{
        Ewl_Embed *embed;
	Ewl_Engine_Evas_Fb *engine;
        Ecore_Fb_Event_Key_Up *ev;

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
ewl_ev_fb_mouse_down(void *data, int type __UNUSED__, void *e)
{
        int clicks = 1;
        Ewl_Embed *embed;
	Ewl_Engine_Evas_Fb *engine;
        Ecore_Fb_Event_Mouse_Button_Down *ev;
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
ewl_ev_fb_mouse_up(void *data, int type __UNUSED__, void *e)
{
        Ewl_Embed *embed;
	Ewl_Engine_Evas_Fb *engine;
        Ecore_Fb_Event_Mouse_Button_Up *ev;
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
ewl_ev_fb_mouse_move(void *data, int type __UNUSED__, void *e)
{
        Ewl_Embed *embed;
	Ewl_Engine_Evas_Fb *engine;
        Ecore_Fb_Event_Mouse_Move *ev;

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

