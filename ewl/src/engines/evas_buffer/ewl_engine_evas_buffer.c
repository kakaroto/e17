/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Engine_Evas_Buffer.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ee_canvas_setup(Ewl_Window *win, int debug);
static void ee_canvas_output_set(Ewl_Embed *embed, int x, int y, int width,
                int height);
static void ee_canvas_render(Ewl_Embed *emb);
static int ee_init(Ewl_Engine *engine);
static void ee_shutdown(Ewl_Engine *engine);

static void *canvas_funcs[EWL_ENGINE_CANVAS_MAX] =
        {
                ee_canvas_setup,
                ee_canvas_output_set,
                ee_canvas_render, NULL, NULL
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
ewl_engine_create(int *argv __UNUSED__, char ** argc __UNUSED__)
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
        Ewl_Engine_Info *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(engine, TRUE);

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

        IF_FREE(engine->functions);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_canvas_setup(Ewl_Window *win, int debug __UNUSED__)
{
        Evas *evas;
        Evas_Engine_Info *info = NULL;
        Evas_Engine_Info_Buffer *bufinfo;
        Ewl_Object *o;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        evas = evas_new();
        evas_output_method_set(evas, evas_render_method_lookup("buffer"));

        info = evas_engine_info_get(evas);
        if (!info)
        {
                fprintf(stderr, "Unable to use buffer engine for rendering.\n");
                exit(-1);
        }

        win->window = bufinfo = (Evas_Engine_Info_Buffer *)info;

        bufinfo->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_ARGB32;
        bufinfo->info.dest_buffer = NULL;
        bufinfo->info.dest_buffer_row_bytes = 0;
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
        ewl_embed_canvas_set(EWL_EMBED(win), evas, win->window);

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
        DCHECK_PARAM_PTR(emb);
        DCHECK_TYPE(emb, EWL_EMBED_TYPE);

        evas = emb->canvas;

        evas_output_size_set(evas, width, height);
        evas_output_viewport_set(evas, x, y, width, height);
        evas_damage_rectangle_add(evas, 0, 0, width, height);

        info = evas_engine_info_get(evas);
        if (!info)
        {
                fprintf(stderr, "Unable to use buffer engine for rendering.\n");
                exit(-1);
        }

        o = EWL_OBJECT(emb);
        bufinfo = (Evas_Engine_Info_Buffer *)info;
        bufinfo->info.dest_buffer_row_bytes = sizeof(int) * width;
        bufinfo->info.dest_buffer = realloc(bufinfo->info.dest_buffer,
                        bufinfo->info.dest_buffer_row_bytes * height);

        evas_engine_info_set(evas, info);

        emb->canvas_window = bufinfo->info.dest_buffer;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_canvas_render(Ewl_Embed *embed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);

        if (embed->canvas) {
                Eina_List *updates;

                updates = evas_render_updates(embed->canvas);
                if (updates) {
                        ewl_callback_call(EWL_WIDGET(embed),
                                        EWL_CALLBACK_VALUE_CHANGED);
                        evas_render_updates_free(updates);
                }
        }

        DRETURN(DLEVEL_STABLE);
}

