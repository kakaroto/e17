#define _GNU_SOURCE 1
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>

#include <Eina.h>
#include <Ecore.h>
#include <Edje.h>
#include <Evas.h>
#include <Elementary.h>

#define ELM_INTERNAL_API_ARGESFSDFEFC
#include <elm_widget.h>

#include "config.h"

#ifdef HAVE___ATTRIBUTE__
# define __UNUSED__ __attribute__((unused))
#else
# define __UNUSED__
#endif

static Elm_Genlist_Item_Class itc, itc_ee;
static Eina_Bool _lib_init = EINA_FALSE;
static Eina_List *tree = NULL;
static Evas_Object *prop_list;

static void libclouseau_highlight(Evas_Object *addr);
static Eina_Bool libclouseau_highlight_fade(void *rv);

typedef struct _Tree_Item Tree_Item;
struct _Tree_Item
{
   Tree_Item *parent;
   Eina_List *children;
   union {
        Ecore_Evas *ee;
        Evas_Object *obj;
   } data;
};

static void
_item_tree_item_free(Tree_Item *parent)
{
   Tree_Item *treeit;

   EINA_LIST_FREE(parent->children, treeit)
     {
        _item_tree_item_free(treeit);
     }

   free(parent);
}

static void
_item_tree_free(void)
{
   Tree_Item *treeit;

   EINA_LIST_FREE(tree, treeit)
     {
        _item_tree_item_free(treeit);
     }
}

static Evas_Object *
item_icon_get(void *data, Evas_Object *parent, const char *part)
{
   Tree_Item *treeit = data;
   if (!strcmp(part, "elm.swallow.icon"))
     {
        Evas_Object *obj;
        char buf[PATH_MAX];

        obj = treeit->data.obj;
        if (evas_object_clipees_get(obj) && !evas_object_visible_get(obj))
          {
             Evas_Object *ic;
             Evas_Object *bx = elm_box_add(parent);
             evas_object_size_hint_aspect_set(bx, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);

             ic = elm_icon_add(bx);
             snprintf(buf, sizeof(buf), "%s/images/clipper.png",
                   PACKAGE_DATA_DIR);
             elm_icon_file_set(ic, buf, NULL);
             evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);
             evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND,
                   EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(ic, EVAS_HINT_FILL,
                   EVAS_HINT_FILL);
             elm_box_pack_end(bx, ic);

             ic = elm_icon_add(bx);
             snprintf(buf, sizeof(buf), "%s/images/hidden.png",
                   PACKAGE_DATA_DIR);
             elm_icon_file_set(ic, buf, NULL);
             evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);
             evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND,
                   EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(ic, EVAS_HINT_FILL,
                   EVAS_HINT_FILL);
             elm_box_pack_end(bx, ic);

             return bx;

          }
        else if (evas_object_clipees_get(obj))
          {
             Evas_Object *ic;
             ic = elm_icon_add(parent);
             snprintf(buf, sizeof(buf), "%s/images/clipper.png",
                   PACKAGE_DATA_DIR);
             elm_icon_file_set(ic, buf, NULL);
             evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);
             return ic;
          }
        else if (!evas_object_visible_get(obj))
          {
             Evas_Object *ic;
             ic = elm_icon_add(parent);
             snprintf(buf, sizeof(buf), "%s/images/hidden.png",
                   PACKAGE_DATA_DIR);
             elm_icon_file_set(ic, buf, NULL);
             evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL,
                   1, 1);
             return ic;
          }
     }

   return NULL;
}

static char *
item_label_get(void *data, Evas_Object *obj __UNUSED__,
      const char *part __UNUSED__)
{
   Tree_Item *treeit = data;
   char buf[256];
   if (elm_widget_is(treeit->data.obj))
     {
        snprintf(buf, sizeof(buf), "%p %s: %s", treeit->data.obj,
              evas_object_type_get(treeit->data.obj),
              elm_widget_type_get(treeit->data.obj));
     }
   else
     {
        snprintf(buf, sizeof(buf), "%p %s", treeit->data.obj,
              evas_object_type_get(treeit->data.obj));
     }
   return strdup(buf);
}

static char *
item_ee_label_get(void *data, Evas_Object *obj __UNUSED__,
      const char *part __UNUSED__)
{
   Tree_Item *treeit = data;
   char buf[256];
   snprintf(buf, sizeof(buf), "%p %s", treeit->data.ee,
         ecore_evas_title_get(treeit->data.ee));
   return strdup(buf);
}

static void
_gl_selected(void *data __UNUSED__, Evas_Object *pobj __UNUSED__,
      void *event_info)
{
   elm_list_clear(prop_list);

   /* If not an object, return. */
   if (!elm_genlist_item_parent_get(event_info))
      return;

   Tree_Item *treeit = elm_genlist_item_data_get(event_info);
   Evas_Object *obj = treeit->data.obj;
   libclouseau_highlight(obj);
   /* Populate properties list */
     {
        char buf[1024];
        Eina_Bool visibility;
        Evas_Coord x, y, w, h;
        double dx, dy;

        visibility = evas_object_visible_get(obj);
        snprintf(buf, sizeof(buf), "Visibility: %d", (int) visibility);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        snprintf(buf, sizeof(buf), "Layer: %hd",
              evas_object_layer_get(obj));
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_geometry_get(obj, &x, &y, &w, &h);
        snprintf(buf, sizeof(buf), "Position: %d %d", x, y);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
        snprintf(buf, sizeof(buf), "Size: %d %d", w, h);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

#if 0
        if (evas_object_clip_get(obj))
          {
             evas_object_geometry_get(
                   evas_object_clip_get(obj), &x, &y, &w, &h);
             snprintf(buf, sizeof(buf), "Clipper position: %d %d", x, y);
             elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
             snprintf(buf, sizeof(buf), "Clipper size: %d %d", w, h);
             elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
          }
#endif

        evas_object_size_hint_min_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Min size: %d %d", w, h);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_size_hint_max_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Max size: %d %d", w, h);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_size_hint_request_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Request size: %d %d", w, h);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_size_hint_align_get(obj, &dx, &dy);
        snprintf(buf, sizeof(buf), "Align: %.6lg %.6lg", dx, dy);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        evas_object_size_hint_weight_get(obj, &dx, &dy);
        snprintf(buf, sizeof(buf), "Weight: %.6lg %.6lg", dx, dy);
        elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);

        /* Handle color */
          {
             int r, g, b, a;
             evas_object_color_get(obj, &r, &g, &b, &a);
             snprintf(buf, sizeof(buf), "Color: %d %d %d %d", r, g, b, a);
             elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
          }

        if (evas_object_clipees_get(obj))
          {
             snprintf(buf, sizeof(buf), "Has clipees");
             elm_list_item_append(prop_list, buf, NULL, NULL, NULL, NULL);
          }

        elm_list_go(prop_list);
     }
}

static void
gl_exp(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   Evas_Object *gl = elm_genlist_item_genlist_get(it);
   Tree_Item *parent = elm_genlist_item_data_get(it);
   Tree_Item *treeit;
   Eina_List *itr;

   EINA_LIST_FOREACH(parent->children, itr, treeit)
     {
        Elm_Genlist_Item_Flags iflag = (treeit->children) ?
           ELM_GENLIST_ITEM_SUBITEMS : ELM_GENLIST_ITEM_NONE;
        elm_genlist_item_append(gl, &itc, treeit, it, iflag,
              _gl_selected, NULL);
     }
}

static void
gl_con(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_subitems_clear(it);
}

static void
gl_exp_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_expanded_set(it, EINA_TRUE);
}

static void
gl_con_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_expanded_set(it, EINA_FALSE);
}

static void
libclouseau_item_add(Evas_Object *o, Evas_Object *gl, Tree_Item *parent)
{
   Eina_List *children, *tmp;
   Evas_Object *child;
   Tree_Item *treeit;

   treeit = calloc(1, sizeof(*treeit));
   treeit->data.obj = o;
   treeit->parent = parent;
   parent->children = eina_list_append(parent->children, treeit);

   children = evas_object_smart_members_get(o);
   EINA_LIST_FOREACH(children, tmp, child){
        libclouseau_item_add(child, gl, treeit);
   }
}

static void
_load_list(Evas_Object *gl)
{
   Eina_List *ees, *eeitr;
   Ecore_Evas *ee, *this_ee;

   elm_list_clear(prop_list);
   elm_genlist_clear(gl);
   _item_tree_free();

   ees = ecore_evas_ecore_evas_list_get();

   this_ee = ecore_evas_ecore_evas_get(
         evas_object_evas_get(elm_object_top_widget_get(gl)));

   EINA_LIST_FOREACH(ees, eeitr, ee)
     {
        Eina_List *objs, *objitr;
        Evas_Object *obj;
        Tree_Item *treeit;

        Evas *e;
        int w, h;

        if (this_ee == ee)
           continue;

        e = ecore_evas_get(ee);
        evas_output_size_get(e, &w, &h);

        treeit = calloc(1, sizeof(*treeit));
        treeit->data.ee = ee;

        tree = eina_list_append(tree, treeit);

        objs = evas_objects_in_rectangle_get(e, SHRT_MIN, SHRT_MIN,
              USHRT_MAX, USHRT_MAX, EINA_TRUE, EINA_TRUE);
        EINA_LIST_FOREACH(objs, objitr, obj)
          {
             libclouseau_item_add(obj, gl, treeit);
          }

        /* Insert the base ee items */
          {
             Elm_Genlist_Item_Flags glflag = (treeit->children) ?
                ELM_GENLIST_ITEM_SUBITEMS : ELM_GENLIST_ITEM_NONE;
             elm_genlist_item_append(gl, &itc_ee, treeit, NULL,
                   glflag, NULL, NULL);
          }
     }
}

static void
_bt_clicked(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   elm_object_text_set(obj, "Reload");
   _load_list(data);
}

void
libclouseau_init(void)
{
   Evas_Object *win, *bg, *panes, *bx, *bt;

   win = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
   elm_win_autodel_set(win, EINA_TRUE);
   elm_win_title_set(win, PACKAGE_NAME);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);

   bt = elm_button_add(win);
   evas_object_size_hint_align_set(bt, 0.0, 0.5);
   elm_object_text_set(bt, "Load");
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   panes = elm_panes_add(win);
   evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(panes, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, panes);
   evas_object_show(panes);

   /* The main list */
     {
        Evas_Object *gl;

        gl = elm_genlist_add(panes);
        evas_object_size_hint_align_set(gl, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_panes_content_left_set(panes, gl);
        evas_object_show(gl);

        evas_object_smart_callback_add(bt, "clicked", _bt_clicked, gl);

        itc_ee.item_style = "default";
        itc_ee.func.label_get = item_ee_label_get;
        itc_ee.func.icon_get = NULL;
        itc_ee.func.state_get = NULL;
        itc_ee.func.del = NULL;

        itc.item_style = "default";
        itc.func.label_get = item_label_get;
        itc.func.icon_get = item_icon_get;
        itc.func.state_get = NULL;
        itc.func.del = NULL;

        evas_object_smart_callback_add(gl, "expand,request", gl_exp_req, gl);
        evas_object_smart_callback_add(gl, "contract,request", gl_con_req, gl);
        evas_object_smart_callback_add(gl, "expanded", gl_exp, gl);
        evas_object_smart_callback_add(gl, "contracted", gl_con, gl);
        evas_object_smart_callback_add(gl, "selected", _gl_selected, NULL);
     }

   /* Properties list */
     {
        prop_list = elm_list_add(panes);
        evas_object_size_hint_align_set(prop_list, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(prop_list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

        elm_panes_content_right_set(panes, prop_list);
        evas_object_show(prop_list);
     }

   evas_object_resize(win, 500, 500);
   evas_object_show(win);
}

/* Hook on the main loop
 * We only do something here if we didn't already go into elm_init,
 * which probably means we are not using elm. */
void
ecore_main_loop_begin(void)
{
   Eina_Bool _is_init = _lib_init;
   void (*_ecore_main_loop_begin)(void) =
      dlsym(RTLD_NEXT, "ecore_main_loop_begin");

   if (!_is_init)
     {
        /* Make sure we init elementary, wouldn't be needed once we
         * take away the ui to another proc. */
        elm_init(0, NULL);

        libclouseau_init();
     }

   _lib_init = EINA_TRUE;
   _ecore_main_loop_begin();

   return;
}


/* HIGHLIGHT code. */

/* The color of the highlight */
enum {
	HIGHLIGHT_R = 255,
	HIGHLIGHT_G = 128,
	HIGHLIGHT_B = 128,
	HIGHLIGHT_A = 255,

	/* How much padding around the highlight box.
         * Currently we don't want any. */
	PADDING = 0,
};

static void
libclouseau_highlight(Evas_Object *obj)
{
   Evas *e;
   Evas_Object *r;
   int x, y, w, h;

   e = evas_object_evas_get(obj);
   if (!e) return;

   evas_object_geometry_get(obj, &x, &y, &w, &h);

   r = evas_object_rectangle_add(e);
   evas_object_move(r, x - PADDING, y - PADDING);
   evas_object_resize(r, w + (2 * PADDING), h + (2 * PADDING));
   evas_object_color_set(r, HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B,
         HIGHLIGHT_A);
   evas_object_show(r);
   ecore_timer_add(0.1, libclouseau_highlight_fade, r);
}

static Eina_Bool
libclouseau_highlight_fade(void *_rect)
{
   Evas_Object *rect = _rect;
   int r, g, b, a;
   double na;

   evas_object_color_get(rect, &r, &g, &b, &a);
   if (a < 20)
     {
        evas_object_del(rect);
        return EINA_FALSE;
     }

   na = a - 20;
   r = na / a * r;
   g = na / a * g;
   b = na / a * b;
   evas_object_color_set(rect, r, g, b, na);

   return EINA_TRUE;
}
