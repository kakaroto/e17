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

#include "libclouseau.h"
#include "ui/obj_information.h"

static Elm_Genlist_Item_Class itc, itc_ee;
static Eina_Bool _lib_init = EINA_FALSE;
static Eina_List *tree = NULL;
static Eina_Bool list_show_clippers = EINA_TRUE, list_show_hidden = EINA_TRUE;

static void libclouseau_highlight(Evas_Object *addr);
static Eina_Bool libclouseau_highlight_fade(void *rv);

static void
_item_tree_item_free(Tree_Item *parent)
{
   Tree_Item *treeit;

   EINA_LIST_FREE(parent->children, treeit)
     {
        _item_tree_item_free(treeit);
     }

   eina_stringshare_del(parent->name);
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
        char buf[PATH_MAX];

        if (treeit->is_clipper && !treeit->is_visible)
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
        else if (treeit->is_clipper)
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
        else if (!treeit->is_visible)
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
   return strdup(treeit->name);
}

static char *
item_ee_label_get(void *data, Evas_Object *obj __UNUSED__,
      const char *part __UNUSED__)
{
   Tree_Item *treeit = data;
   char buf[256];
   snprintf(buf, sizeof(buf), "%p %s", treeit->ptr, treeit->name);
   return strdup(buf);
}

static void
_gl_selected(void *data __UNUSED__, Evas_Object *pobj __UNUSED__,
      void *event_info)
{
   clouseau_obj_information_list_clear();
   /* If not an object, return. */
   if (!elm_genlist_item_parent_get(event_info))
      return;

   Tree_Item *treeit = elm_genlist_item_data_get(event_info);

   Evas_Object *obj = treeit->ptr;
   libclouseau_highlight(obj);

   clouseau_obj_information_list_populate(treeit);
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
        if ((!list_show_hidden && !treeit->is_visible) ||
              (!list_show_clippers && treeit->is_clipper))
           continue;

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
   char buf[1024];

   treeit = calloc(1, sizeof(*treeit));
   treeit->ptr = o;
   treeit->is_obj = EINA_TRUE;
   if (elm_widget_is(o))
     {
        snprintf(buf, sizeof(buf), "%p %s: %s", o, evas_object_type_get(o),
              elm_widget_type_get(o));
     }
   else
     {
        snprintf(buf, sizeof(buf), "%p %s", o, evas_object_type_get(o));
     }
   treeit->name = eina_stringshare_add(buf);
   treeit->is_clipper = !!evas_object_clipees_get(o);
   treeit->is_visible = evas_object_visible_get(o);

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

   clouseau_obj_information_list_clear();
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
        treeit->name = eina_stringshare_add(ecore_evas_title_get(ee));
        treeit->ptr = ee;

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
_show_clippers_check_changed(void *data, Evas_Object *obj,
      void *event_info __UNUSED__)
{
   list_show_clippers = elm_check_state_get(obj);
   _load_list(data);
}

static void
_show_hidden_check_changed(void *data, Evas_Object *obj,
      void *event_info __UNUSED__)
{
   list_show_hidden = elm_check_state_get(obj);
   _load_list(data);
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
   Evas_Object *win, *bg, *panes, *bx, *bt, *show_hidden_check,
               *show_clippers_check;

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

   /* Control buttons */
     {
        Evas_Object *hbx;

        hbx = elm_box_add(bx);
        evas_object_size_hint_align_set(hbx, 0.0, 0.5);
        elm_box_horizontal_set(hbx, EINA_TRUE);
        elm_box_pack_end(bx, hbx);
        elm_box_padding_set(hbx, 10, 0);
        evas_object_show(hbx);

        bt = elm_button_add(hbx);
        evas_object_size_hint_align_set(bt, 0.0, 0.5);
        elm_object_text_set(bt, "Load");
        elm_box_pack_end(hbx, bt);
        evas_object_show(bt);

        show_hidden_check = elm_check_add(hbx);
        elm_object_text_set(show_hidden_check, "Show Hidden");
        elm_check_state_set(show_hidden_check, list_show_hidden);
        elm_box_pack_end(hbx, show_hidden_check);
        evas_object_show(show_hidden_check);

        show_clippers_check = elm_check_add(hbx);
        elm_object_text_set(show_clippers_check, "Show Clippers");
        elm_check_state_set(show_clippers_check, list_show_clippers);
        elm_box_pack_end(hbx, show_clippers_check);
        evas_object_show(show_clippers_check);
     }

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
        evas_object_smart_callback_add(show_hidden_check, "changed",
              _show_hidden_check_changed, gl);
        evas_object_smart_callback_add(show_clippers_check, "changed",
              _show_clippers_check_changed, gl);

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
        Evas_Object *prop_list = NULL;
        prop_list = clouseau_obj_information_list_add(panes);
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
