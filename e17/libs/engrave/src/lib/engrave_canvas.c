#include "engrave_private.h"
#include "Engrave.h"
#include "engrave_macros.h"
#include <limits.h>

typedef struct Engrave_Canvas Engrave_Canvas;
struct Engrave_Canvas
{
    Evas_Object *clip;
    Evas_Coord x, y, w, h;

    Engrave_File *engrave_file;
    Engrave_Group *current_group;
};

static Evas_Smart *engrave_canvas_object_smart_get(void);
static Evas_Object *engrave_canvas_object_new(Evas *evas);
static void engrave_canvas_object_add(Evas_Object *o);
static void engrave_canvas_object_del(Evas_Object *o);
static void engrave_canvas_object_layer_set(Evas_Object *o, int l);
static void engrave_canvas_object_raise(Evas_Object *o);
static void engrave_canvas_object_lower(Evas_Object *o);
static void engrave_canvas_object_stack_above(Evas_Object *o, Evas_Object *above);
static void engrave_canvas_object_stack_below(Evas_Object *o, Evas_Object *below);
static void engrave_canvas_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void engrave_canvas_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void engrave_canvas_object_show(Evas_Object *o);
static void engrave_canvas_object_hide(Evas_Object *o);
static void engrave_canvas_object_color_set(Evas_Object *o, int r, int g, int b, int a);
static void engrave_canvas_object_clip_set(Evas_Object *o, Evas_Object *clip);
static void engrave_canvas_object_clip_unset(Evas_Object *o);

static void engrave_canvas_redraw(Evas_Object *o, Engrave_Canvas *ec);
static void engrave_canvas_part_redraw(Engrave_Part *ep, void *data);
static void engrave_canvas_part_state_redraw(Engrave_Part_State *eps,
                                            Engrave_Part *ep, void *data);
static void engrave_canvas_part_state_recalc(Engrave_Canvas *ec, 
                                    Engrave_Part_State *eps, 
                                    Engrave_Part_State *rel1_to_x, 
                                    Engrave_Part_State *rel1_to_y, 
                                    Engrave_Part_State *rel2_to_x,
                                    Engrave_Part_State *rel2_to_y,
                                    Engrave_Part_State *confine);

static Evas_Object *engrave_canvas_part_state_image_setup(Evas *evas, Engrave_File *ef,
                                    Engrave_Part_State *eps);
static Evas_Object *engrave_canvas_part_state_text_setup(Evas *evas,
                                    Engrave_Part_State *eps);
static Evas_Object *engrave_canvas_part_state_rect_setup(Evas *evas,
                                    Engrave_Part_State *eps);

/**
 * engrave_canvas_new - create a new cavnas
 * @param e: The Evas to create the canvas in
 * 
 * @return Returns the Evas_Object that is the canvas or NULL on error.
 */
Evas_Object *
engrave_canvas_new(Evas *e)
{
    Evas_Object *res = NULL;
    Engrave_Canvas *data = NULL;

    if ((res = engrave_canvas_object_new(e))) {
        if ((data = evas_object_smart_data_get(res)))
            return res;
        else
            evas_object_del(res);
    }
    return NULL;
}

/**
 * engrave_canvas_fil_set - set the file to display in this canvas
 * @param o: The evas object that is the canvas
 * @param ef: The Engrave_File to display in the canvas
 *
 * @return Returns no value.
 */
void
engrave_canvas_file_set(Evas_Object *o, Engrave_File *ef)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o))) {
        Engrave_Group *eg;
        Evas *evas;

        ec->engrave_file = ef;

        /* XXX this isn't right */
        eg = engrave_file_group_last_get(ec->engrave_file);
        engrave_canvas_current_group_set(o, eg);

        evas = evas_object_evas_get(o);
        if (engrave_file_font_dir_get(ef))
            evas_font_path_append(evas, engrave_file_font_dir_get(ef));

        engrave_canvas_redraw(o, ec);
    }
}

/**
 * engrave_canvas_current_group_set - set the group to display in the canvas
 * @param o: The canvas object to display the group in
 * @param eg; The Engrave_Group to display
 * 
 * @return Returns no value.
 */
void
engrave_canvas_current_group_set(Evas_Object *o, Engrave_Group *eg)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o))) {
        ec->current_group = eg;
        engrave_canvas_redraw(o, ec);
    }
}

static void
engrave_canvas_redraw(Evas_Object *o, Engrave_Canvas *ec)
{
    engrave_group_parts_foreach(ec->current_group, engrave_canvas_part_redraw, o);
}

static void
engrave_canvas_part_redraw(Engrave_Part *ep, void *data)
{
    Engrave_Part_State *eps;

    /* 
     * only want to draw one state for a part. The default one for now.
     * XXX need to fix this tho to draw the selected visible state ...
    */
    eps = engrave_part_state_by_name_value_find(ep, "default", 0.0);
    engrave_canvas_part_state_redraw(eps, ep, data);
}

static void
engrave_canvas_part_state_redraw(Engrave_Part_State *eps, 
                                    Engrave_Part *ep, void *data)
{
    Engrave_Group *eg;
    Evas_Object *o, *eps_object;
    Engrave_Canvas *ec;
    Engrave_Part *rel1_to_x = NULL, *rel1_to_y = NULL;
    Engrave_Part *rel2_to_x = NULL, *rel2_to_y = NULL, *confine = NULL;

    if (!eps) return;

    o = data;
    ec = evas_object_smart_data_get(o);
    if (!ec) return;

    if (!(eps_object = engrave_part_state_evas_object_get(eps))) {
        Evas *evas;

        evas = evas_object_evas_get(o);
        switch(engrave_part_type_get(ep)) {
            case ENGRAVE_PART_TYPE_IMAGE:
                eps_object = engrave_canvas_part_state_image_setup(evas,
                                                    ec->engrave_file, eps);
                break;

            case ENGRAVE_PART_TYPE_TEXT:
                eps_object = engrave_canvas_part_state_text_setup(evas, eps);
                break;

            case ENGRAVE_PART_TYPE_RECT:
                eps_object = engrave_canvas_part_state_rect_setup(evas, eps);
                break;

            case ENGRAVE_PART_TYPE_SWALLOW:
                /* XXX ?? */
                break;

            default:
                break;
        }
        engrave_part_state_evas_object_set(eps, eps_object);
        evas_object_clip_set(eps_object, ec->clip);
    }

    eg = engrave_part_parent_get(ep);
    if (engrave_part_state_rel1_to_x_get(eps)) {
        rel1_to_x = engrave_group_part_by_name_find(eg, 
                        engrave_part_state_rel1_to_x_get(eps));
        engrave_canvas_part_state_redraw(engrave_part_current_state_get(rel1_to_x),
                                                             rel1_to_x, data);
    }

    if (engrave_part_state_rel1_to_y_get(eps)) {
        rel1_to_y = engrave_group_part_by_name_find(eg, 
                        engrave_part_state_rel1_to_y_get(eps));
        engrave_canvas_part_state_redraw(engrave_part_current_state_get(rel1_to_y),
                                                             rel1_to_y, data);
    }

    if (engrave_part_state_rel2_to_x_get(eps)) {
        rel2_to_x = engrave_group_part_by_name_find(eg, 
                        engrave_part_state_rel2_to_x_get(eps));
        engrave_canvas_part_state_redraw(engrave_part_current_state_get(rel2_to_x),
                                                             rel2_to_x, data);
    }

    if (engrave_part_state_rel2_to_y_get(eps)) {
        rel2_to_y = engrave_group_part_by_name_find(eg, 
                        engrave_part_state_rel2_to_y_get(eps));
        engrave_canvas_part_state_redraw(engrave_part_current_state_get(rel2_to_y),
                                                             rel2_to_y, data);
    }

    if (engrave_part_dragable_confine_get(ep)) {
        confine = engrave_group_part_by_name_find(eg, 
                        engrave_part_dragable_confine_get(ep));
        engrave_canvas_part_state_redraw(engrave_part_current_state_get(confine),
                                                             confine, data);
    }

    engrave_canvas_part_state_recalc(ec, eps, 
                                    engrave_part_current_state_get(rel1_to_x),
                                    engrave_part_current_state_get(rel1_to_y), 
                                    engrave_part_current_state_get(rel2_to_x),
                                    engrave_part_current_state_get(rel2_to_y),
                                    engrave_part_current_state_get(confine));

    if (engrave_part_type_get(ep) == ENGRAVE_PART_TYPE_RECT) {
        int r, g, b, a;

        evas_object_move(eps_object, ec->x + eps->pos.x, 
                ec->y + eps->pos.y);
        evas_object_resize(eps_object, eps->pos.w, eps->pos.h);

        engrave_part_state_color_get(eps, &r, &g, &b, &a);
        evas_object_color_set(eps_object, r, g, b, a);

    } else if (engrave_part_type_get(ep) == ENGRAVE_PART_TYPE_TEXT) {
        int r, g, b, a;
        int offset_x = 0, offset_y = 0;

        /* XXX this needs a lot of work ... */
        evas_object_move(eps_object, ec->x + eps->pos.x + offset_x,
                                    ec->y + eps->pos.y + offset_y);
        engrave_part_state_color_get(eps, &r, &g, &b, &a);
        evas_object_color_set(eps_object, r, g, b, a);

    } else if (engrave_part_type_get(ep) == ENGRAVE_PART_TYPE_IMAGE) {
        double pos_rel_x, pos_rel_y, rel_x, rel_y;
        int pos_abs_x, pos_abs_y, abs_x, abs_y;
        double fill_x, fill_y, fill_w, fill_h;
        int l, r, t, b;

        engrave_part_state_fill_origin_relative_get(eps, 
                &pos_rel_x, &pos_rel_y);
        engrave_part_state_fill_size_relative_get(eps, &rel_x, &rel_y);
        engrave_part_state_fill_origin_offset_get(eps, 
                &pos_abs_x, &pos_abs_y);
        engrave_part_state_fill_size_offset_get(eps, &abs_x, &abs_y);

        fill_x = pos_abs_x + (eps->pos.w * pos_rel_x);
        fill_y = pos_abs_y + (eps->pos.w * pos_rel_y);
        fill_w = abs_x + (eps->pos.w * rel_x);
        fill_h = abs_y + (eps->pos.h * rel_y);

        evas_object_move(eps_object, ec->x + eps->pos.x, 
                ec->y + eps->pos.y);
        evas_object_resize(eps_object, eps->pos.w, eps->pos.h);
        evas_object_image_fill_set(eps_object, fill_x, fill_y, fill_w, fill_h);
        evas_object_image_smooth_scale_set(eps_object,
                engrave_part_state_fill_smooth_get(eps));

        engrave_part_state_image_border_get(eps, &l, &r, &t, &b);
        evas_object_image_border_set(eps_object, l, r, t, b);

        engrave_part_state_color_get(eps, &l, &r, &t, &b);
        evas_object_color_set(eps_object, l, r, t, b);

    } else if (engrave_part_type_get(ep) == ENGRAVE_PART_TYPE_SWALLOW) {
        evas_object_move(eps_object, ec->x + eps->pos.x, ec->y + eps->pos.y);
        evas_object_resize(eps_object, eps->pos.w, eps->pos.h);
    }

    if (engrave_part_state_visible_get(eps))
        evas_object_show(eps_object);
    else
        evas_object_hide(eps_object);
}

static void
engrave_canvas_part_state_recalc(Engrave_Canvas *ec, Engrave_Part_State *eps, 
                                    Engrave_Part_State *rel1_to_x, 
                                    Engrave_Part_State *rel1_to_y, 
                                    Engrave_Part_State *rel2_to_x,
                                    Engrave_Part_State *rel2_to_y,
                                    Engrave_Part_State *confine)
{
    int minw = 0, minh = 0, maxw = 0, maxh = 0;
    double align_x, align_y;
    Engrave_Part *ep = engrave_part_state_parent_get(eps);

    if (rel1_to_x)
        eps->pos.x = eps->rel1.offset.x + rel1_to_x->pos.x 
                    + (eps->rel1.relative.x * rel1_to_x->pos.w);
    else
        eps->pos.x = eps->rel1.offset.x + (eps->rel1.relative.x * ec->w);

    if (rel2_to_x)
        eps->pos.w = eps->rel2.offset.x + rel2_to_x->pos.x 
                    + (eps->rel2.relative.x * rel2_to_x->pos.y) 
                    - eps->pos.x + 1;
    else
        eps->pos.w = eps->rel2.offset.x + (eps->rel2.relative.x * ec->w) 
                    - eps->pos.x + 1;

    if (rel1_to_y)
        eps->pos.y = eps->rel1.offset.y + rel1_to_y->pos.y 
                    + (eps->rel1.relative.y * rel1_to_y->pos.h);
    else
        eps->pos.y = eps->rel1.offset.y + (eps->rel1.relative.y * ec->h);

    if (rel2_to_y)
        eps->pos.h = eps->rel2.offset.y + rel2_to_y->pos.y 
                    + (eps->rel2.relative.y * rel2_to_y->pos.h)
                    - eps->pos.y + 1;
    else
        eps->pos.h = eps->rel2.offset.y + (eps->rel2.relative.y * ec->h)
                    - eps->pos.y + 1;

    /* aspect */
    engrave_part_state_align_get(eps, &align_x, &align_y);
    if (eps->pos.h > 0) {
        double aspect;
        double aspect_max, aspect_min;
        double new_w, new_h, want_x, want_y, want_w, want_h;
        Engrave_Aspect_Preference prefer;

        want_x = eps->pos.x;
        want_w = new_w = eps->pos.w;

        want_y = eps->pos.y;
        want_h = new_h = eps->pos.h;

        aspect = (double)eps->pos.w / (double)eps->pos.h;
        engrave_part_state_aspect_get(eps, &aspect_min, &aspect_max);
        prefer = engrave_part_state_aspect_preference_get(eps);

        if (prefer == ENGRAVE_ASPECT_PREFERENCE_NONE) {
            if ((aspect_max > 0.0) && (aspect > aspect_max)) {
                new_h = (eps->pos.w / aspect_max);
                new_w = (eps->pos.h * aspect_max);
            }

            if ((aspect_min > 0.0) && (aspect < aspect_min)) {
                new_h = (eps->pos.w / aspect_min);
                new_w = (eps->pos.h * aspect_min);
            }

        } else if (prefer == ENGRAVE_ASPECT_PREFERENCE_VERTICAL) {
            if ((aspect_max > 0.0) && (aspect > aspect_max))
                new_w = (eps->pos.h * aspect_max);

            if ((aspect_min > 0.0) && (aspect < aspect_min))
                new_w = (eps->pos.h * aspect_min);

        } else if (prefer == ENGRAVE_ASPECT_PREFERENCE_HORIZONTAL) {
            if ((aspect_max > 0.0) && (aspect > aspect_max))
                new_h = (eps->pos.w / aspect_max);
        
            if ((aspect_min > 0.0) && (aspect < aspect_min))
                new_h = (eps->pos.w / aspect_min);

        } else if (prefer == ENGRAVE_ASPECT_PREFERENCE_BOTH) {
            if ((aspect_max > 0.0) && (aspect > aspect_max)) {
                new_w = (eps->pos.h * aspect_max);
                new_h = (eps->pos.w / aspect_max);
            }

            if ((aspect_min > 0.0) && (aspect < aspect_min)) {
                new_w = (eps->pos.h * aspect_min);
                new_h = (eps->pos.w / aspect_min);
            }
        }
        
        if (prefer == ENGRAVE_ASPECT_PREFERENCE_BOTH) {
            if (new_w > eps->pos.w)
                eps->pos.w = new_w;
            else
                eps->pos.h = new_h;

        } else {
            if ((eps->pos.h - new_h) > (eps->pos.w - new_w)) {
                if (eps->pos.h != new_h)
                    eps->pos.h = new_h;

                if (prefer == ENGRAVE_ASPECT_PREFERENCE_VERTICAL)
                    eps->pos.w = new_w;

            } else {
                if (eps->pos.w != new_w)
                    eps->pos.w = new_w;

                if (prefer == ENGRAVE_ASPECT_PREFERENCE_HORIZONTAL)
                    eps->pos.h = new_h;
            }
        }

        eps->pos.x = want_x + ((want_w - eps->pos.w) * align_x);
        eps->pos.y = want_y + ((want_h - eps->pos.h) * align_y);
    }

    /* size step */
    {
        double step_x, step_y;
        engrave_part_state_step_get(eps, &step_x, &step_y);

        if (step_x > 0) {
            int steps;
            int new_w;

            steps = eps->pos.w / step_x;
            new_w = step_x * steps;
            if (eps->pos.w > new_w) {
                eps->pos.x = eps->pos.x + ((eps->pos.w - new_w) * align_x);
                eps->pos.w = new_w;
            }
        }

        if (step_y > 0) {
            int steps;
            int new_h;

            steps = eps->pos.h / step_y;
            new_h = step_y * steps;
            if (eps->pos.h > new_h) {
                eps->pos.y = eps->pos.y + ((eps->pos.h - new_h) * align_y);
                eps->pos.h = new_h;
            }
        }
    }

    /* XXX swallow stuff??? */
    engrave_part_state_min_size_get(eps, &minw, &minh);
    engrave_part_state_max_size_get(eps, &maxw, &maxh);

    /* if we have text wanting to make the min size the text size ... */
    if (engrave_part_type_get(ep) == ENGRAVE_PART_TYPE_TEXT) {
        /* XXX FIXME */
    }

    /* adjust for min size */
    if (minw >= 0) {
        if (eps->pos.w < minw) {
            eps->pos.x += ((eps->pos.w - minw) * align_x);
            eps->pos.w = minw;
        }
    }

    if (minh >= 0) {
        if (eps->pos.h < minh) {
            eps->pos.y += ((eps->pos.h - minh) * align_y);
            eps->pos.h = minh;
        }
    }

    /* adjust for max size */
    if (maxw >= 0) {
        if (eps->pos.w > maxw) {
            eps->pos.x += ((eps->pos.w - maxw) * align_x);
            eps->pos.w = maxw;
        }
    }

    if (maxh >= 0) {
        if (eps->pos.h > maxh) {
            eps->pos.y += ((eps->pos.h - maxh) * align_y);
            eps->pos.h = maxh;
        }
    }

    if (confine) {
        /* XXX ? */
    }
}

static Evas_Object *
engrave_canvas_part_state_image_setup(Evas *evas, Engrave_File *ef,
                                    Engrave_Part_State *eps)
{
    Evas_Object *o;
    char path[PATH_MAX];
    Engrave_Image *ei;

    ei = engrave_part_state_image_normal_get(eps);
    snprintf(path, PATH_MAX, "%s/%s",
                engrave_file_image_dir_get(ef),
                engrave_image_name_get(ei));

    o = evas_object_image_add(evas);
    evas_object_image_file_set(o, path, NULL);
    return o;
}

static Evas_Object *
engrave_canvas_part_state_text_setup(Evas *evas, Engrave_Part_State *eps)
{
    Evas_Object *o;
    const char *font_key;
    const char *font_name;
    Engrave_Part *ep = (eps ? eps->parent : NULL);
    Engrave_Group *eg = (ep ? ep->parent : NULL);
    Engrave_File *ef = (eg ? eg->parent : NULL);

    if (ef) {
        Engrave_Font * efont;

        font_key = engrave_part_state_text_font_get(eps);
        efont = engrave_file_font_by_name_find(ef, font_key);
        font_name = engrave_font_path_get(efont);

    } else 
        font_name = "Vera.ttf";

    o = evas_object_text_add(evas);
    evas_object_text_text_set(o, engrave_part_state_text_text_get(eps));
    evas_object_text_font_set(o, font_name, 
                            engrave_part_state_text_size_get(eps));
    return o;
}

static Evas_Object *
engrave_canvas_part_state_rect_setup(Evas *evas, Engrave_Part_State *eps)
{
    Evas_Object *o;
    o = evas_object_rectangle_add(evas);
    return o;
}

static Evas_Object *
engrave_canvas_object_new(Evas *evas)
{
    Evas_Object *ec_object;
    ec_object = evas_object_smart_add(evas,
                    engrave_canvas_object_smart_get());
    return ec_object;
}

static Evas_Smart *
engrave_canvas_object_smart_get(void)
{
    static Evas_Smart *smrt = NULL;
    if (smrt) return smrt;

    smrt = evas_smart_new("engrave_canvas_object",
                            engrave_canvas_object_add,
                            engrave_canvas_object_del,
                            engrave_canvas_object_layer_set,
                            engrave_canvas_object_raise,
                            engrave_canvas_object_lower,
                            engrave_canvas_object_stack_above,
                            engrave_canvas_object_stack_below,
                            engrave_canvas_object_move,
                            engrave_canvas_object_resize,
                            engrave_canvas_object_show,
                            engrave_canvas_object_hide,
                            engrave_canvas_object_color_set,
                            engrave_canvas_object_clip_set,
                            engrave_canvas_object_clip_unset,
                            NULL
                          );
    return smrt;
}

static void 
engrave_canvas_object_add(Evas_Object *o)
{
    Engrave_Canvas *ec;
    Evas *evas;

    evas = evas_object_evas_get(o);

    ec = NEW(Engrave_Canvas, 1);
    ec->clip = evas_object_rectangle_add(evas);

    evas_object_smart_data_set(o, ec);
}

static void
engrave_canvas_object_del(Evas_Object *o)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o))) {
        evas_object_del(ec->clip);

        FREE(ec);
    }
}

static void
engrave_canvas_object_layer_set(Evas_Object *o, int l)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o))) 
        evas_object_layer_set(ec->clip, l);
}

static void
engrave_canvas_object_raise(Evas_Object *o)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o)))
        evas_object_raise(ec->clip);
}

static void
engrave_canvas_object_lower(Evas_Object *o)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o)))
        evas_object_lower(ec->clip);
}

static void
engrave_canvas_object_stack_above(Evas_Object *o, Evas_Object *above)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o)))
        evas_object_stack_above(ec->clip, above);
}

static void
engrave_canvas_object_stack_below(Evas_Object *o, Evas_Object *below)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o)))
        evas_object_stack_below(ec->clip, below);
}

static void
engrave_canvas_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o))) {
        evas_object_move(ec->clip, x, y);
        ec->x = x;
        ec->y = y;

        engrave_canvas_redraw(o, ec);
    }
}

static void
engrave_canvas_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o))) {
        evas_object_resize(ec->clip, w, h);
        ec->w = w;
        ec->h = h;

        engrave_canvas_redraw(o, ec);
    }
}

static void
engrave_canvas_object_show(Evas_Object *o)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o)))
        evas_object_show(ec->clip);
}

static void
engrave_canvas_object_hide(Evas_Object *o)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o)))
        evas_object_hide(ec->clip);
}

static void
engrave_canvas_object_color_set(Evas_Object *o, int r, int g, int b, int a)
{
    /* don't care ... */
}

static void
engrave_canvas_object_clip_set(Evas_Object *o, Evas_Object *clip)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o)))
        evas_object_clip_set(ec->clip, clip);
}

static void
engrave_canvas_object_clip_unset(Evas_Object *o)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o)))
        evas_object_clip_unset(ec->clip);
}


