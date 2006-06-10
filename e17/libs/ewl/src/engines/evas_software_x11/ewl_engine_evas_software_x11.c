#include "Ewl_Engine_Evas_Software_X11.h"
#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

static void ee_canvas_setup(Ewl_Window *win, int debug);
static int ee_init(Ewl_Engine *engine);

static Ewl_Engine_Info engine_funcs = {
		ee_init,
		NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL, NULL,
		ee_canvas_setup	
	};

Ecore_List *
ewl_engine_dependancies(void)
{
	Ecore_List *d;

	DENTER_FUNCTION(DLEVEL_STABLE);

	d = ecore_list_new();
	ecore_list_append(d, strdup("x11"));

	DRETURN_PTR(d, DLEVEL_STABLE);
}

Ewl_Engine *
ewl_engine_create(void)
{
	Ewl_Engine_Evas_Software_X11 *engine;

	DENTER_FUNCTION(DLEVEL_STABLE);

	engine = NEW(Ewl_Engine_Evas_Software_X11, 1);
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
	DCHECK_PARAM_PTR_RET("engine", engine, FALSE);

	engine->name = strdup("evas_software_x11");
	engine->functions = &engine_funcs;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ee_canvas_setup(Ewl_Window *win, int debug)
{
	Evas *evas;
	Ewl_Object *o;
	Evas_Engine_Info *info = NULL;
	Evas_Engine_Info_Software_X11 *sinfo;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	o = EWL_OBJECT(win);

	evas = evas_new();
	evas_output_method_set(evas, 
			evas_render_method_lookup("software_x11"));

	info = evas_engine_info_get(evas);
	if (!info) 
	{
		fprintf(stderr, "Unable to use software_x11 engine "
				"for rendering, ");
		exit(-1);
	}  

	sinfo = (Evas_Engine_Info_Software_X11 *)info;

	sinfo->info.display = ecore_x_display_get();
	sinfo->info.visual = DefaultVisual(sinfo->info.display,
				DefaultScreen(sinfo->info.display));
	sinfo->info.colormap = DefaultColormap(sinfo->info.display,
				DefaultScreen(sinfo->info.display));
	sinfo->info.drawable = (Ecore_X_Window)win->window;
	sinfo->info.depth = DefaultDepth(sinfo->info.display,
				DefaultScreen(sinfo->info.display));
	sinfo->info.rotation = 0;
	sinfo->info.debug = debug;

	evas_engine_info_set(evas, info);

	evas_output_size_set(evas, ewl_object_current_w_get(o),
					ewl_object_current_h_get(o));
	evas_output_viewport_set(evas, ewl_object_current_x_get(o),
					ewl_object_current_y_get(o),
					ewl_object_current_w_get(o),
					ewl_object_current_h_get(o));
        ewl_embed_evas_set(EWL_EMBED(win), evas, win->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



