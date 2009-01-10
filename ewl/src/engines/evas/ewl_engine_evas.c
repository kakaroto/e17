/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Engine_Evas.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

#include <Evas.h>

#include <Edje.h>

static int ee_init(Ewl_Engine *engine);
static void ee_shutdown(Ewl_Engine *engine);

static void ee_canvas_output_set(Ewl_Embed *embed, int x, int y, int width,
                int height);
static void ee_canvas_render(Ewl_Embed *embed);
static void ee_canvas_freeze(Ewl_Embed *embed);
static void ee_canvas_thaw(Ewl_Embed *embed);
static void ee_canvas_damage_add(Ewl_Embed *embed, int x, int y, int w, int h);

static void *canvas_funcs[EWL_ENGINE_CANVAS_MAX] =
        {
                NULL,
                ee_canvas_output_set,
                ee_canvas_render,
                ee_canvas_freeze,
                ee_canvas_thaw,
                ee_canvas_damage_add
        };

static void *theme_funcs[EWL_ENGINE_THEME_MAX] =
        {
                /* FIXME: disable these calls for now. They stop ejde reloading
                 *  the theme object correctly after it was obscured, in some
                 *  cases. If these problems are solved, please activate them
                 *  again. For more information see bug #3, #116 and #456*/
                NULL, //edje_freeze,
                NULL, //edje_thaw,
                edje_file_data_get,
                NULL,
                NULL,
                evas_object_del,
                evas_object_move,
                evas_object_resize,
                evas_object_show,
                evas_object_hide,
                evas_object_clip_set,
                evas_object_clip_unset,
                NULL, NULL,
                NULL, NULL,
                NULL, NULL, NULL,
                NULL,
                evas_object_del,
                evas_object_clipees_get,
                evas_object_color_set,
                NULL,
                NULL,
        };

Ecore_DList *
ewl_engine_dependancies(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

Ewl_Engine *
ewl_engine_create(int *argc __UNUSED__, char ** argv __UNUSED__)
{
        Ewl_Engine_Evas *engine;

        DENTER_FUNCTION(DLEVEL_STABLE);

        engine = NEW(Ewl_Engine_Evas, 1);
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
        DCHECK_PARAM_PTR_RET(engine, FALSE);

        info = NEW(Ewl_Engine_Info, 1);
        info->shutdown = ee_shutdown;
        info->hooks.canvas = canvas_funcs;
        info->hooks.theme = theme_funcs;

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

void
ee_canvas_output_set(Ewl_Embed *embed, int x, int y, int width, int height)
{
        evas_output_size_set(embed->canvas, width, height);
        evas_output_viewport_set(embed->canvas, x, y, width, height);
}

static void
ee_canvas_render(Ewl_Embed *embed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);

        if (embed->canvas)
                evas_render(embed->canvas);

        DRETURN(DLEVEL_STABLE);
}

static void
ee_canvas_freeze(Ewl_Embed *embed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);

        if (embed->canvas && evas_event_freeze_get(embed->canvas) < 1) {
                evas_event_freeze(embed->canvas);
        }

        DRETURN(DLEVEL_STABLE);
}

static void
ee_canvas_thaw(Ewl_Embed *embed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);

        if (embed->canvas && evas_event_freeze_get(embed->canvas) > 0)
                evas_event_thaw(embed->canvas);

        DRETURN(DLEVEL_STABLE);
}

static void
ee_canvas_damage_add(Ewl_Embed *embed, int x, int y, int w, int h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);

        if (embed->canvas)
                evas_damage_rectangle_add(embed->canvas, x, y, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

