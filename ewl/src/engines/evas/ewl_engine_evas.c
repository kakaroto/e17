/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Engine_Evas.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

#include <Evas.h>

#include <Edje.h>

static Evas_Smart *widget_smart = NULL;

static int ee_init(Ewl_Engine *engine);
static void ee_shutdown(Ewl_Engine *engine);

static void ee_canvas_output_set(Ewl_Embed *embed, int x, int y, int width,
                int height);
static void ee_canvas_render(Ewl_Embed *embed);
static void ee_canvas_freeze(Ewl_Embed *embed);
static void ee_canvas_thaw(Ewl_Embed *embed);
static void ee_canvas_damage_add(Ewl_Embed *embed, int x, int y, int w, int h);
static void *ee_canvas_smart_new(Ewl_Embed *embed);
static void *ee_canvas_clip_add(Ewl_Embed *embed);
static void ee_canvas_stack_add(Ewl_Widget *w);
static void ee_canvas_layer_update(Ewl_Widget *w);

static Evas_Object *ewl_widget_layer_neighbor_find_above(Ewl_Widget *w);
static Evas_Object *ewl_widget_layer_neighbor_find_below(Ewl_Widget *w);

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
                ee_canvas_smart_new,
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
                ee_canvas_clip_add,
                evas_object_del,
                evas_object_clipees_get,
                evas_object_color_set,
                ee_canvas_stack_add,
                ee_canvas_layer_update,
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

static void *
ee_canvas_smart_new(Ewl_Embed *embed)
{
        Evas_Object *smart_object;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);

        if (!widget_smart) {
                widget_smart = evas_smart_new("Ewl Widget Smart Object",
                                NULL, NULL, NULL, NULL,
                                NULL, NULL, NULL, NULL,
                                NULL, NULL, NULL, NULL,
                                NULL, NULL, NULL);
        }

        smart_object = evas_object_smart_add(embed->canvas, widget_smart);
        DRETURN_PTR(smart_object, DLEVEL_STABLE);
}

static void *
ee_canvas_clip_add(Ewl_Embed *embed)
{
        Evas_Object *obj;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);

        obj = evas_object_rectangle_add(embed->canvas);
        if (obj)
                evas_object_pass_events_set(obj, TRUE);

        DRETURN_PTR(obj, DLEVEL_STABLE);
}

static void
ee_canvas_stack_add(Ewl_Widget *w)
{
        Evas_Object *smart_parent;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (w->parent && !REVEALED(w->parent))
                DRETURN(DLEVEL_STABLE);


        if (w->parent && !TOPLAYERED(w))
                smart_parent = w->parent->smart_object;
        else {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(w);
                smart_parent = emb->smart;
        }

        evas_object_smart_member_add(w->smart_object, smart_parent);

        if (w->theme_object)
                evas_object_smart_member_add(w->theme_object, w->smart_object);

        if (w->fx_clip_box)
                evas_object_smart_member_add(w->fx_clip_box, w->smart_object);

        if (w->theme_object && w->fx_clip_box)
                evas_object_stack_below(w->theme_object, w->fx_clip_box);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_canvas_layer_update(Ewl_Widget *w)
{
        Ewl_Widget *p;
        int layer;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!(p = w->parent))
                DRETURN(DLEVEL_STABLE);

        /* check first if the widget should be on the top */
        if (TOPLAYERED(w))
        {
                evas_object_raise(w->smart_object);
                DRETURN(DLEVEL_STABLE);
        }

        layer = ewl_widget_layer_priority_get(w);
        if (layer == 0)
                evas_object_stack_above(w->smart_object, p->fx_clip_box);
        else if (layer > 0) {
                Evas_Object *above;

                if (!(above = ewl_widget_layer_neighbor_find_above(w)))
                        DWARNING("No object above.");

                evas_object_stack_above(w->smart_object, above);
        }
        else {
                Evas_Object *below;

                if (!(below = ewl_widget_layer_neighbor_find_below(w)))
                        DWARNING("No object below.");

                evas_object_stack_below(w->smart_object, below);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Evas_Object *
ewl_widget_layer_neighbor_find_above(Ewl_Widget *w)
{
        Evas_Object *o, *ol;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        if (!w->parent)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        o = ol = w->parent->fx_clip_box;

        while ((o = evas_object_above_get(o)))
        {
                Ewl_Widget *found;

                found = evas_object_data_get(o, "EWL");
                /*
                 * Perhaps it is a cached object so no reason to stop iterating
                 */
                if (found) {
                        /* ignore the widget itself */
                        if (w == found)
                                continue;
                        if (ewl_widget_layer_priority_get(w) <=
                                        ewl_widget_layer_priority_get(found))
                                break;
                        ol = o;
                }
        }

        DRETURN_PTR(ol, DLEVEL_STABLE);
}

static Evas_Object *
ewl_widget_layer_neighbor_find_below(Ewl_Widget *w)
{
        Evas_Object *o, *ol;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        if (!w->parent)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        o = ol = w->parent->fx_clip_box;

        while ((o = evas_object_below_get(o)))
        {
                Ewl_Widget *found;

                found = evas_object_data_get(o, "EWL");
                /*
                 * Perhaps it is a cached object so no reason to stop iterating
                 */
                if (found) {
                        /* ignore the widget itself */
                        if (w == found)
                                continue;
                        if (ewl_widget_layer_priority_get(w) >=
                                        ewl_widget_layer_priority_get(found))
                                break;
                        ol = o;
                }
        }

        DRETURN_PTR(ol, DLEVEL_STABLE);
}
