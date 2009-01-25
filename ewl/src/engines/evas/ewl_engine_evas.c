/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Engine_Evas.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

#include <Evas.h>
#include <Edje.h>

#define EWL_WIDGET_SMART_NAME "Ewl Widget Smart"

static int ee_init(Ewl_Engine *engine);
static void ee_shutdown(Ewl_Engine *engine);

static Evas_Smart *ee_smart = NULL;

/* canvas functions */
static void ee_canvas_output_set(Ewl_Embed *embed, int x, int y, int width,
                int height);
static void ee_canvas_render(Ewl_Embed *embed);
static void ee_canvas_freeze(Ewl_Embed *embed);
static void ee_canvas_thaw(Ewl_Embed *embed);
static void ee_canvas_damage_add(Ewl_Embed *embed, int x, int y, int w, int h);

/* theme functions */
static Evas_Object *ee_smart_new(void *canvas);
static Evas_Object *ee_smart_clip_get(Evas_Object *smart);
static void ee_theme_layer_stack_add(Ewl_Widget *w);
static void ee_theme_layer_stack_del(Ewl_Widget *w);
static void ee_theme_layer_update(Ewl_Widget *w);
static void ee_theme_object_color_set(Evas_Object *obj, Ewl_Color_Set *color);
static Evas_Object *ee_theme_object_group_add(Ewl_Embed *embed);
static Evas_Object *ee_theme_element_add(Ewl_Embed *embed);
static Evas_Object *ee_theme_layer_neighbor_find_below(Ewl_Widget *w, Evas_Object *clip);
static Evas_Object *ee_theme_layer_neighbor_find_above(Ewl_Widget *w, Evas_Object *clip);
static unsigned int ee_theme_element_swallow(Evas_Object *obj1, Evas_Object *obj2);
static void ee_theme_element_state_set(Evas_Object *obj, const char *signal);


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
                /* the general functions */
                /* FIXME: disable these calls for now. They stop ejde reloading
                 *  the theme object correctly after it was obscured, in some
                 *  cases. If these problems are solved, please activate them
                 *  again. For more information see bug #3, #116 and #456*/
                NULL, //edje_freeze,
                NULL, //edje_thaw,
                ee_theme_layer_stack_add,
                ee_theme_layer_stack_del,
                ee_theme_layer_update,
                /* the object functions */
                evas_object_del,
                ee_theme_object_color_set,
                evas_object_show,
                evas_object_move,
                evas_object_resize,
                /* group function */
                ee_theme_object_group_add,
                /* the theme functions */
                ee_theme_element_add,
                edje_object_file_set,
                edje_object_load_error_get,
                ee_theme_element_state_set,
                edje_object_part_text_set,
                edje_object_size_min_get,
                edje_object_size_min_calc,
                edje_object_size_max_get,
                edje_object_data_get,
                ee_theme_element_swallow,
                edje_object_part_unswallow,
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

/*
 * The theme functions
 */
static Evas_Object *
ee_theme_object_group_add(Ewl_Embed *embed)
{
        Evas_Object *obj;

        DENTER_FUNCTION(DLEVEL_STABLE);

        obj = ee_smart_new(embed->canvas);

        DRETURN_PTR(obj, DLEVEL_STABLE);
}

static Evas_Object *
ee_theme_element_add(Ewl_Embed *embed)
{
        Evas_Object *obj;

        DENTER_FUNCTION(DLEVEL_STABLE);

        obj = edje_object_add(embed->canvas);

        DRETURN_PTR(obj, DLEVEL_STABLE);
}

static void
ee_theme_object_color_set(Evas_Object *obj, Ewl_Color_Set *color)
{
        int a;

        DENTER_FUNCTION(DLEVEL_STABLE);

        a = color->a;
        evas_object_color_set(obj, color->r * a / 0xff, color->g * a / 0xff,
                        color->b * a / 0xff, a);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static unsigned int
ee_theme_element_swallow(Evas_Object *obj1, Evas_Object *obj2)
{
        unsigned int ret;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((ret = edje_object_part_exists(obj1, "swallow")))
                edje_object_part_swallow(obj1, "swallow", obj2);

        DRETURN_INT(ret, DLEVEL_STABLE);        
}

static void
ee_theme_element_state_set(Evas_Object *obj, const char *signal)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        edje_object_signal_emit(obj, signal, "EWL");

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_theme_layer_stack_add(Ewl_Widget *w)
{
        Evas_Object *smart_parent;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (w->parent && !TOPLAYERED(w))
                smart_parent = w->parent->smart_object;
        else
        {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(w);
                smart_parent = emb->smart;
        }

        if (!smart_parent)
                DRETURN(DLEVEL_STABLE);

        if (w->theme_object)
        {
                evas_object_smart_member_add(w->theme_object, smart_parent);
                evas_object_data_set(w->theme_object, "EWL", w);
                if (w->smart_object && !w->swallowed)
                {
                        evas_object_smart_member_add(w->smart_object,
                                        smart_parent);
                        evas_object_stack_above(w->smart_object,
                                        w->theme_object);
                        evas_object_data_set(w->smart_object, "EWL", w);
                }
        }
        else if (w->smart_object)
        {
                evas_object_smart_member_add(w->smart_object, smart_parent);
                evas_object_data_set(w->smart_object, "EWL", w);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_theme_layer_stack_del(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (w->theme_object)
                evas_object_data_del(w->theme_object, "EWL");
        if (w->smart_object)
                evas_object_data_del(w->smart_object, "EWL");

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_theme_layer_update(Ewl_Widget *w)
{
        Evas_Object *anchor;
        Ewl_Widget *p;
        int layer;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!(p = w->parent))
                DRETURN(DLEVEL_STABLE);

        if (w->theme_object)
                anchor = w->theme_object;
        else if (w->smart_object)
                anchor = w->smart_object;
        else
                DRETURN(DLEVEL_STABLE);

        /* check first if the widget should be on the top */
        if (TOPLAYERED(w))
                evas_object_raise(anchor);
        else
        {
                Evas_Object *clip;

                clip = ee_smart_clip_get(p->smart_object);
                layer = ewl_widget_layer_priority_get(w);
                if (layer == 0)
                        evas_object_stack_above(anchor, clip);
                else if (layer > 0) {
                        Evas_Object *above;

                        if (!(above = ee_theme_layer_neighbor_find_above(w,
                                                        clip)))
                        {
                                DWARNING("No object to stack above.");
                                DRETURN(DLEVEL_STABLE);
                        }
                        evas_object_stack_above(anchor, above);
                }
                else {
                        Evas_Object *below;

                        if (!(below = ee_theme_layer_neighbor_find_below(w,
                                                        clip)))
                        {
                                DWARNING("No object to stack below.");
                                DRETURN(DLEVEL_STABLE);
                        }
                        evas_object_stack_below(anchor, below);
                }
        }
        if (!w->swallowed && w->smart_object && w->theme_object)
               evas_object_stack_above(w->smart_object, w->theme_object);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Evas_Object *
ee_theme_layer_neighbor_find_above(Ewl_Widget *w, Evas_Object *clip)
{
        Evas_Object *o, *ol;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_PARAM_PTR_RET(clip, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        o = ol = clip;

        while ((o = evas_object_above_get(o)))
        {
                Ewl_Widget *found;

                found = evas_object_data_get(o, "EWL");
                /*
                 * Perhaps it is a cached object so no reason to stop iterating
                 */
                if (found) {
                        /* ignore the widget itself */
                        if (w == found) continue;
                        if (ewl_widget_layer_priority_get(w) <=
                                        ewl_widget_layer_priority_get(found))
                                break;
                        ol = o;
                }
        }

        DRETURN_PTR(ol, DLEVEL_STABLE);
}

static Evas_Object *
ee_theme_layer_neighbor_find_below(Ewl_Widget *w, Evas_Object *clip)
{
        Evas_Object *o, *ol;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_PARAM_PTR_RET(clip, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        o = ol = clip;

        while ((o = evas_object_below_get(o)))
        {
                Ewl_Widget *found;

                found = evas_object_data_get(o, "EWL");
                /*
                 * Perhaps it is a cached object so no reason to stop iterating
                 */
                if (found) {
                        /* ignore the widget itself */
                        if (w == found) continue;
                        if (ewl_widget_layer_priority_get(w) >=
                                        ewl_widget_layer_priority_get(found))
                                break;
                        ol = o;
                }
        }

        DRETURN_PTR(ol, DLEVEL_STABLE);
}

/*
 * The widget smart object
 */
static Evas_Object *
ee_smart_clip_get(Evas_Object *smart)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(smart, NULL);

        DRETURN_PTR(evas_object_smart_data_get(smart), DLEVEL_STABLE);
}

static void
ee_smart_del(Evas_Object *obj)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj)))
                evas_object_del(clip);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj)))
                evas_object_move(clip, x, y);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj)))
                evas_object_resize(clip, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_smart_show(Evas_Object *obj)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj))
                        && evas_object_clipees_get(clip))
                evas_object_show(clip);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_smart_hide(Evas_Object *obj)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj)))
                evas_object_hide(clip);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj)))
                evas_object_color_set(clip, r, g, b, a);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_smart_clip_set(Evas_Object *obj, Evas_Object *clip_to)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj)))
                evas_object_clip_set(clip, clip_to);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_smart_clip_unset(Evas_Object *obj)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj)))
                evas_object_clip_unset(clip);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ee_smart_member_add(Evas_Object *obj, Evas_Object *member)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj)))
        {
                evas_object_clip_set(member, clip);
                if (evas_object_visible_get(obj))
                        evas_object_show(clip);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_smart_member_del(Evas_Object *obj, Evas_Object *member)
{
        Evas_Object *clip;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if ((clip = evas_object_smart_data_get(obj)))
        {
                evas_object_clip_unset(member);
                if (!evas_object_clipees_get(clip))
                        evas_object_hide(clip);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Evas_Object *
ee_smart_new(void *canvas)
{
        Evas_Object *obj;
        Evas_Object *clip;
        
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(canvas, NULL);

        if (!ee_smart)
        {
                static const Evas_Smart_Class sc = {
                        EWL_WIDGET_SMART_NAME,
                        EVAS_SMART_CLASS_VERSION,
                        NULL, /* add */
                        ee_smart_del,
                        ee_smart_move,
                        ee_smart_resize,
                        ee_smart_show,
                        ee_smart_hide,
                        ee_smart_color_set,
                        ee_smart_clip_set,
                        ee_smart_clip_unset,
                        NULL, /* calculate */
                        ee_smart_member_add,
                        ee_smart_member_del,
                        NULL
                };
                ee_smart = evas_smart_class_new(&sc);
        }
        obj = evas_object_smart_add(canvas, ee_smart);

        /* create the clip object */
        clip = evas_object_rectangle_add(canvas);
        evas_object_smart_data_set(obj, clip);
        evas_object_smart_member_add(clip, obj);

        DRETURN_PTR(obj, DLEVEL_STABLE);
}

