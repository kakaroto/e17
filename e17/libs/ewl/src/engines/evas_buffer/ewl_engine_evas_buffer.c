#include <Ewl.h>
#include "Evas_Engine_Buffer.h"
#include "Ewl_Engine_Evas_Buffer.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ee_canvas_setup(Ewl_Window *win, int debug);
static void ee_canvas_output_set(Ewl_Embed *embed, int x, int y, int width,
		int height);
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
		ee_canvas_setup,
		ee_canvas_output_set
	}
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
ewl_engine_create(void)
{
	Ewl_Engine_Evas_Buffer *engine;

	DENTER_FUNCTION(DLEVEL_STABLE);

	engine = NEW(Ewl_Engine_Evas_Buffer, 1);
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

	engine->name = strdup("evas_buffer");
	engine->functions = &engine_funcs;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ee_shutdown(Ewl_Engine *engine)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("engine", engine);

	engine->functions = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_canvas_setup(Ewl_Window *win, int debug)
{
	Evas *evas;
	Evas_Engine_Info *info = NULL;
	Evas_Engine_Info_Buffer *bufinfo;
	Ewl_Object *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	evas = evas_new();
	evas_output_method_set(evas, evas_render_method_lookup("buffer"));

	info = evas_engine_info_get(evas);
	if (!info) 
	{
		fprintf(stderr, "Unable to use buffer engine for rendering, ");
		exit(-1);
	}  

	win->window = bufinfo = (Evas_Engine_Info_Buffer *)info;

	bufinfo->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_ARGB32;
	bufinfo->info.dest_buffer = NULL;
	bufinfo->info.use_color_key = 0;
	bufinfo->info.alpha_threshold = 0;
	bufinfo->info.func.new_update_region = NULL;
	bufinfo->info.func.free_update_region = NULL;
	evas_engine_info_set(evas, (Evas_Engine_Info *)bufinfo);

	o = EWL_OBJECT(win);
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

static void
ee_canvas_output_set(Ewl_Embed *emb, int x, int y, int width, int height)
{
	Evas *evas;
	Evas_Engine_Info *info = NULL;
	Evas_Engine_Info_Buffer *bufinfo;
	Ewl_Object *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("emb", emb);
	DCHECK_TYPE("emb", emb, EWL_EMBED_TYPE);

	evas = emb->evas;
	evas_output_method_set(evas, evas_render_method_lookup("buffer"));

	info = evas_engine_info_get(evas);
	if (!info) 
	{
		fprintf(stderr, "Unable to use buffer engine for rendering, ");
		exit(-1);
	}  

	o = EWL_OBJECT(emb);
	bufinfo = (Evas_Engine_Info_Buffer *)info;
	bufinfo->info.dest_buffer = realloc(bufinfo->info.dest_buffer,
			width * height * 4 * sizeof(char));

	evas_engine_info_set(evas, info);
	evas_output_size_set(evas, width, height);
	evas_output_viewport_set(evas, x, y, width, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
