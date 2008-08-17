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
static void engrave_canvas_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void engrave_canvas_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void engrave_canvas_object_show(Evas_Object *o);
static void engrave_canvas_object_hide(Evas_Object *o);
static void engrave_canvas_object_color_set(Evas_Object *o, int r, int g, int b, int a);
static void engrave_canvas_object_clip_set(Evas_Object *o, Evas_Object *clip);
static void engrave_canvas_object_clip_unset(Evas_Object *o);

static void engrave_canvas_redraw(Evas_Object *o, Engrave_Canvas *ec);
static void engrave_canvas_part_redraw(Engrave_Part *ep, void *data);

static void engrave_canvas_part_recalc(Engrave_Canvas *ec, 
                                    Engrave_Part *ep, 
                                    Engrave_Part_State *rel1_to_x, 
                                    Engrave_Part_State *rel1_to_y, 
                                    Engrave_Part_State *rel2_to_x,
                                    Engrave_Part_State *rel2_to_y,
                                    Engrave_Part_State *confine);

static void engrave_canvas_part_state_text_setup(Engrave_Part_State *eps);
static void engrave_canvas_part_state_rect_setup(Engrave_Part_State *eps);
static void engrave_canvas_part_state_image_setup(Engrave_Part_State *eps);

static void engrave_canvas_part_hide(Engrave_Part *ep, void *data);

/**
 * engrave_canvas_new - create a new cavnas
 * @param e: The Evas to create the canvas in
 * 
 * @return Returns the Evas_Object that is the canvas or NULL on error.
 */
EAPI Evas_Object *
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
EAPI void
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
EAPI void
engrave_canvas_current_group_set(Evas_Object *o, Engrave_Group *eg)
{
    Engrave_Canvas *ec;

    if ((ec = evas_object_smart_data_get(o))) {
	if (ec->current_group)
	   engrave_group_parts_foreach(ec->current_group,engrave_canvas_part_hide,NULL);
        ec->current_group = eg;
        engrave_canvas_redraw(o, ec);
    }
}

static void
engrave_canvas_part_hide(Engrave_Part *ep, void *data)
{
   evas_object_hide(ep->object);
}
  
static void
engrave_canvas_redraw(Evas_Object *o, Engrave_Canvas *ec)
{
    engrave_group_parts_foreach(ec->current_group, engrave_canvas_part_redraw, o);
}

static void
engrave_canvas_part_redraw(Engrave_Part *ep, void *data)
{
   Engrave_Part_State *eps = NULL;
   Evas_Object *o, *ep_object;
   Engrave_Group *eg;
   Engrave_Canvas *ec;
   Engrave_Part *rel1_to_x = NULL, *rel1_to_y = NULL;
   Engrave_Part *rel2_to_x = NULL, *rel2_to_y = NULL, *confine = NULL;

   
   if (!ep) return;
   
   o = data;
   ec = evas_object_smart_data_get(o);
   if (!ec) return;
      
   eps = engrave_part_current_state_get(ep);
   
   if (!eps) 
      eps = engrave_part_state_by_name_value_find(ep,"default", 0.0);
   
   if (!(ep_object = engrave_part_evas_object_get(ep))) {
        Evas *evas = evas_object_evas_get(o);

        if (engrave_part_type_get(ep) == ENGRAVE_PART_TYPE_IMAGE)
           ep_object = evas_object_image_add(evas);
        if (engrave_part_type_get(ep) == ENGRAVE_PART_TYPE_TEXT)
           ep_object = evas_object_text_add(evas);
        if (engrave_part_type_get(ep) == ENGRAVE_PART_TYPE_RECT)
           ep_object = evas_object_rectangle_add(evas);
        /*if (engrave_part_type_get(ep) == ENGRAVE_PART_TYPE_SWALLOW)
           ep_object = XXX ?? */
        
        engrave_part_evas_object_set(ep, ep_object);
        evas_object_clip_set(ep_object, ec->clip);
    }
 
    eg = engrave_part_parent_get(ep);
    if (engrave_part_state_rel1_to_x_get(eps)) {
        rel1_to_x = engrave_group_part_by_name_find(eg, 
                        engrave_part_state_rel1_to_x_get(eps));
        engrave_canvas_part_redraw(rel1_to_x, data);
    }

    if (engrave_part_state_rel1_to_y_get(eps)) {
        rel1_to_y = engrave_group_part_by_name_find(eg, 
                        engrave_part_state_rel1_to_y_get(eps));
        engrave_canvas_part_redraw(rel1_to_y, data);
    }

    if (engrave_part_state_rel2_to_x_get(eps)) {
        rel2_to_x = engrave_group_part_by_name_find(eg, 
                        engrave_part_state_rel2_to_x_get(eps));
        engrave_canvas_part_redraw(rel2_to_x, data);
    }

    if (engrave_part_state_rel2_to_y_get(eps)) {
        rel2_to_y = engrave_group_part_by_name_find(eg, 
                        engrave_part_state_rel2_to_y_get(eps));
        engrave_canvas_part_redraw(rel2_to_y, data);
    }

    if (engrave_part_dragable_confine_get(ep)) {
        confine = engrave_group_part_by_name_find(eg, 
                        engrave_part_dragable_confine_get(ep));
        engrave_canvas_part_redraw(confine, data);
    }

    engrave_canvas_part_recalc(ec, ep, 
                                    engrave_part_current_state_get(rel1_to_x),
                                    engrave_part_current_state_get(rel1_to_y), 
                                    engrave_part_current_state_get(rel2_to_x),
                                    engrave_part_current_state_get(rel2_to_y),
                                    engrave_part_current_state_get(confine));
    
   switch (engrave_part_type_get(ep))
     {
      case ENGRAVE_PART_TYPE_RECT:
        engrave_canvas_part_state_rect_setup(eps);
        evas_object_move(ep_object, ec->x + ep->pos.x, ec->y + ep->pos.y);
        evas_object_resize(ep_object, ep->pos.w, ep->pos.h);
        break;
	
      case ENGRAVE_PART_TYPE_TEXT:
        engrave_canvas_part_state_text_setup(eps);
        evas_object_move(ep_object, ec->x + ep->pos.x,
			 ec->y + ep->pos.y);
        break;
	
      case ENGRAVE_PART_TYPE_IMAGE:
        engrave_canvas_part_state_image_setup(eps);
        evas_object_move(ep_object, ec->x + ep->pos.x, ec->y + ep->pos.y);
        evas_object_resize(ep_object, ep->pos.w, ep->pos.h);
        break;
	
      case ENGRAVE_PART_TYPE_SWALLOW:
	evas_object_move(ep_object, ec->x + ep->pos.x, ec->y + ep->pos.y);
	evas_object_resize(ep_object, ep->pos.w, ep->pos.h);
	break;

      default:
	break;
     }

   if (engrave_part_state_visible_get(eps))
      evas_object_show(ep_object);
   else
      evas_object_hide(ep_object);
   evas_object_raise(ep_object);
}

static void
engrave_canvas_part_recalc(Engrave_Canvas *ec, Engrave_Part *ep, 
                                    Engrave_Part_State *rel1_to_x, 
                                    Engrave_Part_State *rel1_to_y, 
                                    Engrave_Part_State *rel2_to_x,
                                    Engrave_Part_State *rel2_to_y,
                                    Engrave_Part_State *confine)
{
    int minw = 0, minh = 0, maxw = 0, maxh = 0;
    double align_x, align_y;
    Engrave_Part_State *eps = engrave_part_current_state_get(ep);

    if (rel1_to_x)
        ep->pos.x = eps->rel1.offset.x + ((Engrave_Part*)(rel1_to_x->parent))->pos.x 
                    + (eps->rel1.relative.x * ((Engrave_Part*)(rel1_to_x->parent))->pos.w);
    else
        ep->pos.x = eps->rel1.offset.x + (eps->rel1.relative.x * ec->w);

    if (rel2_to_x)
        ep->pos.w = eps->rel2.offset.x + ((Engrave_Part*)(rel2_to_x->parent))->pos.x 
                    + (eps->rel2.relative.x * ((Engrave_Part*)(rel2_to_x->parent))->pos.w) 
                    - ep->pos.x + 1;
    else
        ep->pos.w = eps->rel2.offset.x + (eps->rel2.relative.x * ec->w) 
                    - ep->pos.x + 1;

    if (rel1_to_y)
        ep->pos.y = eps->rel1.offset.y + ((Engrave_Part*)(rel1_to_y->parent))->pos.y 
                    + (eps->rel1.relative.y * ((Engrave_Part*)(rel1_to_y->parent))->pos.h);
    else
        ep->pos.y = eps->rel1.offset.y + (eps->rel1.relative.y * ec->h);

    if (rel2_to_y)
        ep->pos.h = eps->rel2.offset.y + ((Engrave_Part*)(rel2_to_y->parent))->pos.y 
                    + (eps->rel2.relative.y * ((Engrave_Part*)(rel2_to_y->parent))->pos.h)
                    - ep->pos.y + 1;
    else
        ep->pos.h = eps->rel2.offset.y + (eps->rel2.relative.y * ec->h)
                    - ep->pos.y + 1;

    /* aspect */
    engrave_part_state_align_get(eps, &align_x, &align_y);
    if (ep->pos.h > 0) {
        double aspect;
        double aspect_max, aspect_min;
        double new_w, new_h, want_x, want_y, want_w, want_h;
        Engrave_Aspect_Preference prefer;

        want_x = ep->pos.x;
        want_w = new_w = ep->pos.w;

        want_y = ep->pos.y;
        want_h = new_h = ep->pos.h;

        aspect = (double)ep->pos.w / (double)ep->pos.h;
        engrave_part_state_aspect_get(eps, &aspect_min, &aspect_max);
        prefer = engrave_part_state_aspect_preference_get(eps);

        if (prefer == ENGRAVE_ASPECT_PREFERENCE_NONE) {
            if ((aspect_max > 0.0) && (aspect > aspect_max)) {
                new_h = (ep->pos.w / aspect_max);
                new_w = (ep->pos.h * aspect_max);
            }

            if ((aspect_min > 0.0) && (aspect < aspect_min)) {
                new_h = (ep->pos.w / aspect_min);
                new_w = (ep->pos.h * aspect_min);
            }

        } else if (prefer == ENGRAVE_ASPECT_PREFERENCE_VERTICAL) {
            if ((aspect_max > 0.0) && (aspect > aspect_max))
                new_w = (ep->pos.h * aspect_max);

            if ((aspect_min > 0.0) && (aspect < aspect_min))
                new_w = (ep->pos.h * aspect_min);

        } else if (prefer == ENGRAVE_ASPECT_PREFERENCE_HORIZONTAL) {
            if ((aspect_max > 0.0) && (aspect > aspect_max))
                new_h = (ep->pos.w / aspect_max);
        
            if ((aspect_min > 0.0) && (aspect < aspect_min))
                new_h = (ep->pos.w / aspect_min);

        } else if (prefer == ENGRAVE_ASPECT_PREFERENCE_BOTH) {
            if ((aspect_max > 0.0) && (aspect > aspect_max)) {
                new_w = (ep->pos.h * aspect_max);
                new_h = (ep->pos.w / aspect_max);
            }

            if ((aspect_min > 0.0) && (aspect < aspect_min)) {
                new_w = (ep->pos.h * aspect_min);
                new_h = (ep->pos.w / aspect_min);
            }
        }
        
        if (prefer == ENGRAVE_ASPECT_PREFERENCE_BOTH) {
            if (new_w > ep->pos.w)
                ep->pos.w = new_w;
            else
                ep->pos.h = new_h;

        } else {
            if ((ep->pos.h - new_h) > (ep->pos.w - new_w)) {
                if (ep->pos.h != new_h)
                    ep->pos.h = new_h;

                if (prefer == ENGRAVE_ASPECT_PREFERENCE_VERTICAL)
                    ep->pos.w = new_w;

            } else {
                if (ep->pos.w != new_w)
                    ep->pos.w = new_w;

                if (prefer == ENGRAVE_ASPECT_PREFERENCE_HORIZONTAL)
                    ep->pos.h = new_h;
            }
        }

        ep->pos.x = want_x + ((want_w - ep->pos.w) * align_x);
        ep->pos.y = want_y + ((want_h - ep->pos.h) * align_y);
    }

    /* size step */
    {
        double step_x, step_y;
        engrave_part_state_step_get(eps, &step_x, &step_y);

        if (step_x > 0) {
            int steps;
            int new_w;

            steps = ep->pos.w / step_x;
            new_w = step_x * steps;
            if (ep->pos.w > new_w) {
                ep->pos.x = ep->pos.x + ((ep->pos.w - new_w) * align_x);
                ep->pos.w = new_w;
            }
        }

        if (step_y > 0) {
            int steps;
            int new_h;

            steps = ep->pos.h / step_y;
            new_h = step_y * steps;
            if (ep->pos.h > new_h) {
                ep->pos.y = ep->pos.y + ((ep->pos.h - new_h) * align_y);
                ep->pos.h = new_h;
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
        if (ep->pos.w < minw) {
            ep->pos.x += ((ep->pos.w - minw) * align_x);
            ep->pos.w = minw;
        }
    }

    if (minh >= 0) {
        if (ep->pos.h < minh) {
            ep->pos.y += ((ep->pos.h - minh) * align_y);
            ep->pos.h = minh;
        }
    }

    /* adjust for max size */
    if (maxw >= 0) {
        if (ep->pos.w > maxw) {
            ep->pos.x += ((ep->pos.w - maxw) * align_x);
            ep->pos.w = maxw;
        }
    }

    if (maxh >= 0) {
        if (ep->pos.h > maxh) {
            ep->pos.y += ((ep->pos.h - maxh) * align_y);
            ep->pos.h = maxh;
        }
    }

    if (confine) {
        /* XXX ? */
    }
}

static void
engrave_canvas_part_state_text_setup(Engrave_Part_State *eps)
{
   int r, g, b, a;
   const char *font_key;
   char font_name[PATH_MAX];
   Engrave_Part *ep = (eps ? eps->parent : NULL);
   Engrave_Group *eg = (ep ? ep->parent : NULL);
   Engrave_File *ef = (eg ? eg->parent : NULL);

   if (ef) {
      Engrave_Font *efont = NULL;

      font_key = engrave_part_state_text_font_get(eps);
      efont = engrave_file_font_by_name_find(ef, font_key);
      snprintf(font_name, PATH_MAX, "%s/%s", engrave_file_font_dir_get(ef),
                                       engrave_font_path_get(efont));
   } else {
      strcpy(font_name, "Vera.ttf");
   }

   evas_object_text_text_set(ep->object, 
                             engrave_part_state_text_text_get(eps));
   evas_object_text_font_set(ep->object, font_name, 
                             engrave_part_state_text_size_get(eps));
   
   engrave_part_state_color_get(eps, &r, &g, &b, &a);
   evas_color_argb_premul(a,&r,&g,&b);
   evas_object_color_set(ep->object, r, g, b, a);
   
   engrave_part_state_color2_get(eps, &r, &g, &b, &a);
   evas_color_argb_premul(a,&r,&g,&b);
   evas_object_text_shadow_color_set(ep->object, r, g, b, a);
   evas_object_text_glow_color_set(ep->object, r, g, b, a);
   
   engrave_part_state_color3_get(eps, &r, &g, &b, &a);
   evas_color_argb_premul(a,&r,&g,&b);
   evas_object_text_outline_color_set(ep->object, r, g, b, a);
   evas_object_text_glow2_color_set(ep->object, r, g, b, a);

   switch (engrave_part_effect_get(ep))
   {
   case ENGRAVE_TEXT_EFFECT_OUTLINE: 
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_OUTLINE); 
      break;
   case ENGRAVE_TEXT_EFFECT_SOFT_OUTLINE: 
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_SOFT_OUTLINE);
      break;
   case ENGRAVE_TEXT_EFFECT_SHADOW:
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_SHADOW);
      break;
   case ENGRAVE_TEXT_EFFECT_SOFT_SHADOW: 
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_SOFT_SHADOW);
      break;
   case ENGRAVE_TEXT_EFFECT_OUTLINE_SHADOW:
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_OUTLINE_SHADOW);
      break;
   case ENGRAVE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW:
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW);
      break;
   case ENGRAVE_TEXT_EFFECT_FAR_SHADOW:
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_FAR_SHADOW);
      break;
   case ENGRAVE_TEXT_EFFECT_FAR_SOFT_SHADOW:
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_FAR_SOFT_SHADOW);
      break;
   case ENGRAVE_TEXT_EFFECT_GLOW:
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_GLOW);
      break;
   default: 
      evas_object_text_style_set(ep->object, EVAS_TEXT_STYLE_PLAIN);
      break;
   }
}

static void
engrave_canvas_part_state_rect_setup(Engrave_Part_State *eps)
{
   int r, g, b, a;
   Engrave_Part *ep = (eps ? eps->parent : NULL);
   
   if (!ep) return;
    
   engrave_part_state_color_get(eps, &r, &g, &b, &a);
   evas_color_argb_premul(a,&r,&g,&b);
   evas_object_color_set(ep->object, r, g, b, a);
}

static void
engrave_canvas_part_state_image_setup(Engrave_Part_State *eps)
{
   double pos_rel_x, pos_rel_y, rel_x, rel_y;
   int pos_abs_x, pos_abs_y, abs_x, abs_y;
   double fill_x, fill_y, fill_w, fill_h;
   int l, r, b, t;
   char path[PATH_MAX];
   Engrave_Image *ei;
   Engrave_Part *ep = (eps ? eps->parent : NULL);
   Engrave_Group *eg = (ep ? ep->parent : NULL);

   if (!ep || !eg) return;
   
   if ((ei = engrave_part_state_image_normal_get(eps)))
   {
      snprintf(path, PATH_MAX, "%s/%s",
         engrave_file_image_dir_get(engrave_group_parent_get(eg)),
         engrave_image_name_get(ei));
      evas_object_image_file_set(ep->object, path, NULL);
   }

   engrave_part_state_fill_origin_relative_get(eps, &pos_rel_x, &pos_rel_y);
   engrave_part_state_fill_size_relative_get(eps, &rel_x, &rel_y);
   engrave_part_state_fill_origin_offset_get(eps, &pos_abs_x, &pos_abs_y);
   engrave_part_state_fill_size_offset_get(eps, &abs_x, &abs_y);

   fill_x = pos_abs_x + (ep->pos.w * pos_rel_x);
   fill_y = pos_abs_y + (ep->pos.w * pos_rel_y);
   fill_w = abs_x + (ep->pos.w * rel_x);
   fill_h = abs_y + (ep->pos.h * rel_y);

   evas_object_image_fill_set(ep->object, fill_x, fill_y, fill_w, fill_h);
   evas_object_image_smooth_scale_set(ep->object,
      engrave_part_state_fill_smooth_get(eps));

   engrave_part_state_image_border_get(eps, &l, &r, &t, &b);
   evas_object_image_border_set(ep->object, l, r, t, b);

   engrave_part_state_color_get(eps, &l, &r, &t, &b);
   evas_object_color_set(ep->object, l, r, t, b);
   
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

   static const Evas_Smart_Class sc =
   {  
      "engrave_canvas_object",
      EVAS_SMART_CLASS_VERSION,
      engrave_canvas_object_add,
      engrave_canvas_object_del,
      engrave_canvas_object_move,
      engrave_canvas_object_resize,
      engrave_canvas_object_show,
      engrave_canvas_object_hide,
      engrave_canvas_object_color_set,
      engrave_canvas_object_clip_set,
      engrave_canvas_object_clip_unset,
      NULL
   };
   smrt = evas_smart_class_new(&sc); 
    
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


