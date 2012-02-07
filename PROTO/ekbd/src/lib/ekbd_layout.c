#include <ctype.h>
#include <Eina.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Edje.h>
#include "Ekbd.h"
#include "ekbd_private.h"
#include "ekbd_layout.h"
#include "ekbd_send.h"

#define HOLD_TIMEOUT_DELAY 1.5
#define REPEAT_DELAY 1.0
#define MIN_REPEAT_DELAY 0.02

enum
{
   NORMAL = 0,
   SHIFT = (1 << 0),
   CAPSLOCK = (1 << 1),
   CTRL = (1 << 2),
   ALT = (1 << 3),
   ALTGR = (1 << 4)
};

static void _ekbd_layout_build(Smart_Data *sd);
static Ekbd_Int_Key_State *_ekbd_layout_key_state_get(Smart_Data *sd, Ekbd_Int_Key *ky);


static void _ekbd_layout_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _ekbd_layout_cb_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _ekbd_layout_cb_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event);

static void _ekbd_layout_tie_free(Ekbd_Int_Tie *tk);

static Ekbd_Int_Key *_ekbd_layout_at_coord_get(Eina_List *keys, Evas_Coord x, Evas_Coord y);
static void _ekbd_layout_tie_calc(Smart_Data *sd);

static void _ekbd_layout_key_press_handle(Smart_Data *sd, Ekbd_Int_Key *ky);
static void _ekbd_layout_state_update(Smart_Data *sd);

static Eina_Bool _ekbd_layout_cb_hold_timeout(void *data);
static Eina_Bool _ekbd_layout_cb_repeat(void *data);
static void _ekbd_layout_parse(Smart_Data *sd, const char *layout);


Evas_Object *
ekbd_layout_theme_obj_new(Evas *e, const char *custom_file, const char *group)
{
   Evas_Object *o;

   o = edje_object_add(e);
   if (custom_file)
     {
        if (!edje_object_file_set(o, custom_file, group))
          edje_object_file_set(o, PACKAGE_DATA_DIR"/themes/default.edj", group);
     }
   else
     edje_object_file_set(o, PACKAGE_DATA_DIR"/themes/default.edj", group);
   return o;
}

static void
_ekbd_layout_key_free(Ekbd_Int_Key *ky)
{
   Ekbd_Int_Key_State *st;

   EINA_LIST_FREE(ky->states, st)
     {
        if (st->label) eina_stringshare_del(st->label);
        if (st->icon) eina_stringshare_del(st->icon);
        if (st->out) eina_stringshare_del(st->out);
        if (st->lp_out) eina_stringshare_del(st->lp_out);
        if (st->tie) _ekbd_layout_tie_free(st->tie);
        if (st->lp_tie) _ekbd_layout_tie_free(st->lp_tie);
        free(st);
     }
   if (ky->obj) evas_object_del(ky->obj);
   if (ky->icon_obj) evas_object_del(ky->icon_obj);
   free(ky);
}

static void
_ekbd_layout_tie_free(Ekbd_Int_Tie *tk)
{
   Ekbd_Int_Key *ky;

   EINA_LIST_FREE(tk->keys, ky)
     {
        _ekbd_layout_key_free(ky);
     }
   eina_stringshare_del(tk->file);
   free(tk);
}

void
ekbd_layout_free(Smart_Data *sd)
{
   Ekbd_Int_Key *ky;

   if (sd->layout.directory) free(sd->layout.directory);
   if (sd->layout.file) eina_stringshare_del(sd->layout.file);
   sd->layout.directory = NULL;
   sd->layout.file = NULL;
   if (sd->down.tie)
     {
        evas_object_del(sd->cover_obj);
        evas_object_del(sd->down.tie->base_obj);
        sd->down.tie = NULL;
     }
   EINA_LIST_FREE(sd->layout.keys, ky)
     {
        if (ky->icon_obj)
          {
             evas_object_smart_member_del(ky->icon_obj);
          }
        _ekbd_layout_key_free(ky);
     }
   if (sd->event_obj) evas_object_del(sd->event_obj);
   sd->event_obj = NULL;
}



static void
_ekbd_layout_tie_parse(Smart_Data *sd, Ekbd_Int_Tie *tie, const char *path)
{
   FILE *f;
   char buf[PATH_MAX];
   Ekbd_Int_Key *ky = NULL;
   Ekbd_Int_Key_State *st = NULL;
   int isok = 0;

   if (!(f = fopen(path, "r"))) return;
   while (fgets(buf, sizeof(buf), f))
     {
        int len;
        char str[PATH_MAX];
        if (!isok)
          {
             if (!strcmp(buf, "##TIECONF-1.0\n")) isok = 1;
          }
        if (!isok) break;
        if (buf[0] == '#') continue;
        len = strlen(buf);
        if (len > 0)
          {
             if (buf[len -1] == '\n') buf[len - 1] = 0;
          }
        if (sscanf(buf, "%4000s", str) != 1) continue;
        if (!strcmp(str, "kbd"))
          {
             if (sscanf(buf, "%*s %i %i", &(tie->w), &(tie->h)) != 2)
               continue;
          }
        if (!strcmp(str, "fuzz"))
          {
             sscanf(buf, "%*s %i\n", &(tie->fuzz));
             continue;
          }
        if (!strcmp(str, "key"))
          {
             ky = calloc(1, sizeof(Ekbd_Int_Key));
             if (!ky) continue;
             if (sscanf(buf, "%*s %i %i %i %i\n",
                        &(ky->orig_x), &(ky->orig_y),
                        &(ky->orig_w), &(ky->orig_h)) != 4)
               {
                  free(ky);
                  ky = NULL;
                  continue;
               }
             tie->keys = eina_list_append(tie->keys, ky);
          }
        if (!ky) continue;
        if ((!strcmp(str, "normal")) || (!strcmp(str, "shift")) ||
            (!strcmp(str, "capslock")) || (!strcmp(str, "altgr")))
          {
             char *p;
             char label[PATH_MAX];

             if (sscanf(buf, "%*s %4000s", label) != 1) continue;
             st = calloc(1, sizeof(Ekbd_Int_Key_State));
             if (!st) continue;
             ky->states = eina_list_append(ky->states, st);
             if (!strcmp(str, "normal")) st->state = NORMAL;
             if (!strcmp(str, "shift")) st->state = SHIFT;
             if (!strcmp(str, "capslock")) st->state = CAPSLOCK;
             if (!strcmp(str, "altgr")) st->state = ALTGR;
             p = strrchr(label, '.');
             if ((p) && (!strcmp(p, ".png")))
               st->icon = eina_stringshare_add(label);
             else if ((p) && (!strcmp(p, ".edj")))
               st->icon = eina_stringshare_add(label);
             else
               st->label = eina_stringshare_add(label);
             if (sscanf(buf, "%*s %*s %4000s", str) != 1) continue;
             if (str[0] != '"')
               {
                  p = strrchr(str, '.');
                  if (p && (!strcmp(p, ".kbd")))
                    {
                       Ekbd_Layout *kl = NULL;
                       Eina_List *l;
                       snprintf(buf, sizeof(buf), "%s/%s",
                                sd->layout.directory, str);
                       EINA_LIST_FOREACH(sd->layouts, l, kl)
                         {
                            if (!strcmp(kl->path, buf))
                              {
                                 st->layout = kl;
                                 break;
                              }
                         }
                       if (!kl)
                         st->layout = ekbd_layout_add(sd, buf);
                    }
                  else
                    st->out = eina_stringshare_add(str);
               }
             else
               st->out = eina_stringshare_add(str);
          }
        if (!strcmp(str, "is_shift")) ky->is_shift = 1;
        if (!strcmp(str, "is_multi_shift")) ky->is_multi_shift = 1;
        if (!strcmp(str, "is_ctrl")) ky->is_ctrl = 1;
        if (!strcmp(str, "is_alt")) ky->is_alt = 1;
        if (!strcmp(str, "is_altgr")) ky->is_altgr = 1;
        if (!strcmp(str, "is_capslock")) ky->is_capslock = 1;
     }
   fclose(f);
}



static void
_ekbd_layout_parse(Smart_Data *sd, const char *layout)
{
   FILE *f;
   char buf[PATH_MAX];
   int isok = 0;
   Ekbd_Int_Key *ky = NULL;
   Ekbd_Int_Key_State *st = NULL;

   if (!(f = fopen(layout, "r"))) return;

   sd->layout.directory = ecore_file_dir_get(layout);
   sd->layout.file = eina_stringshare_add(layout);

   /* Make the default direction LTR */
   sd->layout.direction = EKBD_DIRECTION_LTR;

   while (fgets(buf, sizeof(buf), f))
     {
        int len;
        char str[PATH_MAX];

        if (!isok)
          {
             if (!strcmp(buf, "##KBDCONF-1.0\n")) isok = 1;
          }
        if (!isok) break;
        if (buf[0] == '#') continue;
        len = strlen(buf);
        if (len > 0)
          {
             if (buf[len - 1] == '\n') buf[len - 1] = 0;
          }
        if (sscanf(buf, "%4000s", str) != 1) continue;
        if (!strcmp(str, "kbd"))
          {
             if (sscanf(buf, "%*s %i %i\n", &(sd->layout.w), &(sd->layout.h)) != 2)
               continue;
          }
        if (!strcmp(str, "fuzz"))
          {
             sscanf(buf, "%*s %i\n", &(sd->layout.fuzz));
             continue;
          }
        if (!strcmp(str, "direction"))
          {
             char direction[4];

             sscanf(buf, "%*s %3s\n", direction);

             /* If rtl mark as rtl, otherwise make it ltr */
             if (!strcmp(direction, "rtl"))
               sd->layout.direction = EKBD_DIRECTION_RTL;
             else
               sd->layout.direction = EKBD_DIRECTION_LTR;
             continue;
          }
        if (!strcmp(str, "key"))
          {
             ky = calloc(1, sizeof(Ekbd_Int_Key));
             if (!ky) continue;
             if (sscanf(buf, "%*s %i %i %i %i\n",
                        &(ky->orig_x), &(ky->orig_y),
                        &(ky->orig_w), &(ky->orig_h)) != 4)
               {
                  free(ky);
                  ky = NULL;
                  continue;
               }
             sd->layout.keys = eina_list_append(sd->layout.keys, ky);
          }
        if (!ky) continue;
        if ((!strcmp(str, "normal")) || (!strcmp(str, "shift")) ||
            (!strcmp(str, "capslock")) || (!strcmp(str, "altgr")))
          {
             char *p;
             char label[PATH_MAX];

             if (sscanf(buf, "%*s %4000s", label) != 1) continue;
             st = calloc(1, sizeof(Ekbd_Int_Key_State));
             if (!st) continue;
             ky->states = eina_list_append(ky->states, st);
             if (!strcmp(str, "normal")) st->state = NORMAL;
             if (!strcmp(str, "shift")) st->state = SHIFT;
             if (!strcmp(str, "capslock")) st->state = CAPSLOCK;
             if (!strcmp(str, "altgr")) st->state = ALTGR;
             p = strrchr(label, '.');
             if ((p) && (!strcmp(p, ".png")))
               st->icon = eina_stringshare_add(label);
             else if ((p) && (!strcmp(p, ".edj")))
               st->icon = eina_stringshare_add(label);
             else
               st->label = eina_stringshare_add(label);
             if (sscanf(buf, "%*s %*s %4000s", str) != 1) continue;
             if (str[0] != '"')
               {
                  p = strrchr(str, '.');
                  if (p)
                    {
                       char tmppath[PATH_MAX];

                       if (!strcmp(p, ".tie"))
                         {
                            Ekbd_Int_Tie *kt;
                            kt = calloc(1, sizeof(Ekbd_Int_Tie));
                            snprintf(tmppath, sizeof(tmppath), "%s/%s",
                                     sd->layout.directory, str);
                            _ekbd_layout_tie_parse(sd, kt, tmppath);
                            st->tie = kt;
                            kt->key = ky;
                         }
                       else if (!strcmp(p, ".kbd"))
                         {
                            Ekbd_Layout *kl = NULL;
                            Eina_List *l;
                            snprintf(tmppath, sizeof(tmppath), "%s/%s",
                                     sd->layout.directory, str);
                            EINA_LIST_FOREACH(sd->layouts, l, kl)
                              {
                                 if (!strcmp(kl->path, tmppath))
                                   {
                                      st->layout = kl;
                                      break;
                                   }
                              }
                            if (!kl)
                              st->layout = ekbd_layout_add(sd, tmppath);
                         }
                    }
                  else
                    st->out = eina_stringshare_add(str);
               }
             else
               st->out = eina_stringshare_add(str);

             /* get the longpress action (if available) */
             if (sscanf(buf, "%*s %*s %*s %4000s", str) != 1) continue;
             if (str[0] != '"')
               {
                  p = strrchr(str, '.');
                  if (p)
                    {
                       if (!strcmp(p, ".tie"))
                         {
                            Ekbd_Int_Tie *lp_kt;
                            lp_kt = calloc(1, sizeof(Ekbd_Int_Tie));
                            snprintf(buf, sizeof(buf), "%s/%s",
                                     sd->layout.directory, str);
                            _ekbd_layout_tie_parse(sd, lp_kt, buf);
                            st->lp_tie = lp_kt;
                            lp_kt->key = ky;
                         }
                    }
                  else
                    st->lp_out = eina_stringshare_add(str);
               }
             else
                st->lp_out = eina_stringshare_add(str);
          }
        if (!strcmp(str, "is_shift")) ky->is_shift = 1;
        if (!strcmp(str, "is_multi_shift")) ky->is_multi_shift = 1;
        if (!strcmp(str, "is_ctrl")) ky->is_ctrl = 1;
        if (!strcmp(str, "is_alt")) ky->is_alt = 1;
        if (!strcmp(str, "is_altgr")) ky->is_altgr = 1;
        if (!strcmp(str, "is_capslock")) ky->is_capslock = 1;
     }
   fclose(f);
}

Ekbd_Layout *
ekbd_layout_add(Smart_Data *sd, const char *path)
{
   char buf[PATH_MAX], *p;
   Ekbd_Layout *kil;
   int isok = 0;
   kil = calloc(1, sizeof(Ekbd_Layout));
   if (kil)
     {
        char *s;
        FILE *f;
        kil->path = path;
        s = strdup(ecore_file_file_get(kil->path));
        if (s)
          {
             p = strchr(s, '.');
             if (p) *p = 0;
             kil->name = eina_stringshare_add(s);
             free(s);
          }
        s = ecore_file_dir_get(kil->path);
        if (s)
          {
             kil->dir = eina_stringshare_add(s);
             free(s);
          }
        f = fopen(kil->path, "r");
        if (f)
          {
             while(fgets(buf, sizeof(buf), f))
               {
                  int buflen;
                  char str[4096];

                  if (!isok)
                    {
                       if (!strcmp(buf, "##KBDCONF-1.0\n")) isok = 1;
                    }
                  if (!isok) break;
                  if (buf[0] == '#') continue;
                  buflen = strlen(buf);
                  if (buflen > 0)
                    {
                       if (buf[buflen - 1] == '\n') buf[buflen - 1] = 0;
                    }
                  if (sscanf(buf, "%4000s", str) != 1) continue;
                  if (!strcmp(str, "type"))
                    {
                       sscanf(buf, "%*s %4000s\n", str);
                       if (!strcmp(str, "ALPHA"))
                         kil->type = EKBD_TYPE_ALPHA;
                       else if (!strcmp(str, "NUMERIC"))
                         kil->type = EKBD_TYPE_NUMERIC;
                       else if (!strcmp(str, "PIN"))
                         kil->type = EKBD_TYPE_PIN;
                       else if (!strcmp(str, "PHONE_NUMBER"))
                         kil->type = EKBD_TYPE_PHONE_NUMBER;
                       else if (!strcmp(str, "HEX"))
                         kil->type = EKBD_TYPE_HEX;
                       else if (!strcmp(str, "TERMINAL"))
                         kil->type = EKBD_TYPE_TERMINAL;
                       else if (!strcmp(str, "PASSWORD"))
                         kil->type = EKBD_TYPE_PASSWORD;
                       else if (!strcmp(str, "IP"))
                         kil->type = EKBD_TYPE_IP;
                       else if (!strcmp(str, "HOST"))
                         kil->type = EKBD_TYPE_HOST;
                       else if (!strcmp(str, "FILE"))
                         kil->type = EKBD_TYPE_FILE;
                       else if (!strcmp(str, "URL"))
                         kil->type = EKBD_TYPE_URL;
                       else if (!strcmp(str, "KEYPAD"))
                         kil->type = EKBD_TYPE_KEYPAD;
                       else if (!strcmp(str, "J2ME"))
                         kil->type = EKBD_TYPE_J2ME;
                       continue;
                    }
                  if (!strcmp(str, "icon"))
                    {
                       sscanf(buf, "%*s %4000s\n", str);
                       snprintf(buf, sizeof(buf), "%s/%s", kil->dir, str);
                       kil->icon = eina_stringshare_add(buf);
                       continue;
                    }
               }
             fclose(f);
          }
        if (!isok)
          {
             free(kil);
             kil = NULL;
          }
        else
          sd->layouts = eina_list_append(sd->layouts, kil);
     }
   return kil;
}


void
ekbd_layouts_free(Smart_Data *sd)
{
   Ekbd_Layout *kil;
   EINA_LIST_FREE(sd->layouts, kil)
     {
        eina_stringshare_del(kil->path);
        eina_stringshare_del(kil->dir);
        eina_stringshare_del(kil->icon);
        eina_stringshare_del(kil->name);
        free(kil);
     }
}

void
ekbd_layout_select(Smart_Data *sd, Ekbd_Layout *kil)
{
   ekbd_layout_free(sd);
   _ekbd_layout_parse(sd, kil->path);
   _ekbd_layout_build(sd);
//   _ekbd_layout_buf_update_(ki);
   _ekbd_layout_state_update(sd);
}

static void
_ekbd_layout_build(Smart_Data *sd)
{
   Eina_List *l;
   Ekbd_Int_Key *ky;
   Evas_Object *o;

   evas_event_freeze(evas_object_evas_get(sd->layout_obj));
   EINA_LIST_FOREACH(sd->layout.keys, l, ky)
     {
        o = ekbd_layout_theme_obj_new(evas_object_evas_get(sd->layout_obj),
                                      NULL, "ekbd/key/default");
        evas_object_move(o, sd->x + ky->orig_x, sd->y + ky->orig_y);
        evas_object_resize(o, ky->orig_w, ky->orig_h);
        evas_object_clip_set(o, sd->layout_obj);
        evas_object_show(o);
        ky->obj = o;
     }

   o = evas_object_rectangle_add(evas_object_evas_get(sd->layout_obj));
   evas_object_color_set(o, 0, 0, 0, 0);
   sd->event_obj = o;
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                  _ekbd_layout_cb_mouse_down, sd);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE,
                                  _ekbd_layout_cb_mouse_move, sd);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP,
                                  _ekbd_layout_cb_mouse_up, sd);
   evas_object_show(o);

   ekbd_send_update(sd);
   evas_event_thaw(evas_object_evas_get(sd->layout_obj));
}

static void
_ekbd_layout_state_update(Smart_Data *sd)
{
   Ekbd_Int_Key *ky;
   Ekbd_Int_Key_State *st;
   const char *label, *icon;
   Eina_List *l;

   EINA_LIST_FOREACH(sd->layout.keys, l, ky)
     {
        Evas_Object *ic = NULL;
        int selected = 0;
        label = "";
        icon = NULL;
        st = _ekbd_layout_key_state_get(sd, ky);
        if (st)
          {
             label = st->label;
             icon = st->icon;
          }
        edje_object_part_text_set(ky->obj, "e.text.label", label);

        if (ky->icon_obj) evas_object_del(ky->icon_obj);
        if (icon)
          {
             char buf[PATH_MAX];
             char *p;

             snprintf(buf, sizeof(buf), "%s/%s", sd->layout.directory, icon);
             p = strrchr(icon, '.');
             if (!strcmp(p, ".edj"))
               {
                  ic = edje_object_add(evas_object_evas_get(sd->layout_obj));
                  edje_object_file_set(ic, buf, "icon");
               }
             else
               {
                  Evas_Coord ww, hh;
                  ic = evas_object_image_filled_add(evas_object_evas_get(sd->layout_obj));
                  //evas_object_image_smooth_scale_set(ic, EINA_FALSE);
                  evas_object_image_file_set(ic, buf, NULL);
                  evas_object_image_smooth_scale_set(ic, EINA_TRUE);
                  evas_object_image_size_get(ic, &ww, &hh);
                  evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_BOTH,
                                                   ww, hh);
                  evas_object_size_hint_max_set(ic, ww, hh);
               }
             evas_object_smart_member_add(sd->s_obj, ic);
             edje_object_part_swallow(ky->obj, "e.swallow.content", ic);
          }
        ky->icon_obj = ic;
         if ((sd->layout.state & SHIFT) && (ky->is_shift)) selected = 1;
         if ((sd->layout.state & CTRL) && (ky->is_ctrl)) selected = 1;
         if ((sd->layout.state & ALT) && (ky->is_alt)) selected = 1;
         if ((sd->layout.state & ALTGR) && (ky->is_altgr)) selected = 1;
         if ((sd->layout.state & CAPSLOCK) && (ky->is_capslock)) selected = 1;
         if ((sd->layout.state & (SHIFT | CAPSLOCK)) && (ky->is_multi_shift))
           selected = 1;
         if (selected)
           {
              if (!ky->selected)
                {
                   edje_object_signal_emit(ky->obj, "e,state,selected", "e");
                   ky->selected = EINA_TRUE;
                }
           }
         if (!selected)
           {
              if (ky->selected)
                {
                   edje_object_signal_emit(ky->obj, "e,state,unselected", "e");
                   ky->selected = EINA_FALSE;
                }
           }
     }
}


static void
_ekbd_layout_tie_build(Smart_Data *sd)
{
   Eina_List *l;
   Ekbd_Int_Tie *kt;
   Ekbd_Int_Key *ky;
   Evas_Object *o, *ic;
   double rw, rh;
   //Evas_Coord x, y, ex, ey, ew, eh;

   kt = sd->down.tie;
   rw = sd->w / (double)sd->layout.w;
   rh = sd->h / (double)sd->layout.h;


   evas_event_freeze(evas_object_evas_get(sd->layout_obj));
   o = ekbd_layout_theme_obj_new(evas_object_evas_get(sd->layout_obj),
                                 NULL, "ekbd/cover/default");
   evas_object_move(o, sd->x, sd->y);
   evas_object_resize(o, sd->w, sd->h);
   evas_object_show(o);
   sd->cover_obj = o;
   evas_object_raise(sd->layout.pressed->obj);

   o = ekbd_layout_theme_obj_new(evas_object_evas_get(sd->layout_obj),
                                 NULL, "ekbd/tie/default");
   evas_object_show(o);
   kt->base_obj = o;

   EINA_LIST_FOREACH(kt->keys, l, ky)
     {
        Ekbd_Int_Key_State *st;
        const char *label, *icon;

        o = ekbd_layout_theme_obj_new(
           evas_object_evas_get(sd->layout_obj), NULL, "ekbd/key/default");
        ky->obj = o;
        label = "";
        icon = NULL;
        st = _ekbd_layout_key_state_get(sd, ky);
        if (st)
          {
             label = st->label;
             icon = st->icon;
          }
        edje_object_part_text_set(o, "e.text.label", label);

        if (icon)
          {
             char buf[PATH_MAX];
             char *p;

             snprintf(buf, sizeof(buf), "%s/%s", sd->layout.directory, icon);
             p = strrchr(icon, '.');
             if (!strcmp(p, ".edj"))
               {
                  ic = edje_object_add(evas_object_evas_get(sd->layout_obj));
                  edje_object_file_set(ic, buf, "icon");
               }
             else
               {
                  ic = evas_object_image_filled_add(
                     evas_object_evas_get(sd->layout_obj));
                  evas_object_image_file_set(ic, buf, NULL);
               }
             edje_object_part_swallow(o, "e.swallow.content", ic);
          }
        edje_object_scale_set(o, rw);
        evas_object_clip_set(o, sd->layout_obj);
        evas_object_show(o);
     }
   evas_object_raise(sd->event_obj);
   _ekbd_layout_tie_calc(sd);

   evas_event_thaw(evas_object_evas_get(sd->layout_obj));
}

static void
_ekbd_layout_tie_del(Smart_Data *sd)
{
   Ekbd_Int_Tie *kt;
   Ekbd_Int_Key *ky;
   Eina_List *l;

   kt = sd->down.tie;
   sd->down.tie = NULL;
   evas_object_del(sd->cover_obj);
   evas_object_del(kt->base_obj);
   EINA_LIST_FOREACH(kt->keys, l, ky)
     {
        evas_object_del(ky->obj);
     }
}


static Ekbd_Int_Key_State *
_ekbd_layout_key_state_get(Smart_Data *sd, Ekbd_Int_Key *ky)
{
   Ekbd_Int_Key_State *found = NULL;
   Ekbd_Int_Key_State *st;
   Eina_List *l;

   EINA_LIST_FOREACH(ky->states, l, st)
     {
        if (st->state & sd->layout.state) return st;
        if (!found && st->state == NORMAL) found = st;
     }
   return found;
}


static void
_ekbd_layout_keydown_update(Smart_Data *sd)
{
   Ekbd_Int_Key *ky;

   if (sd->down.tie)
     ky = _ekbd_layout_at_coord_get(sd->down.tie->keys,
                                    sd->down.cx, sd->down.cy);
   else
     ky = _ekbd_layout_at_coord_get(sd->layout.keys,
                                    sd->down.cx, sd->down.cy);
   if (ky != sd->layout.pressed)
     {
        if (sd->down.hold_timeout)
          {
             ecore_timer_del(sd->down.hold_timeout);
             sd->down.hold_timeout = NULL;
          }
        if (sd->down.repeat)
          {
             ecore_timer_del(sd->down.repeat);
             sd->down.repeat = NULL;
          }
        if (sd->layout.pressed)
          {
             sd->layout.pressed->pressed = EINA_FALSE;
             edje_object_signal_emit(sd->layout.pressed->obj,
                                     "e,state,released", "e");
          }
        sd->layout.pressed = ky;
        if (ky)
          {
             sd->layout.pressed->pressed = EINA_TRUE;
             evas_object_raise(sd->layout.pressed->obj);
             evas_object_raise(sd->event_obj);
             edje_object_signal_emit(sd->layout.pressed->obj,
                                     "e,state,pressed", "e");
             if (!ky->is_shift && !ky->is_multi_shift && !ky->is_ctrl && !ky->is_alt
                 && !ky->is_altgr && !ky->is_capslock)
               sd->down.hold_timeout = ecore_timer_add(HOLD_TIMEOUT_DELAY,
                                                       _ekbd_layout_cb_hold_timeout, sd);
          }
     }
}

static Eina_Bool
_ekbd_layout_cb_repeat(void *data)
{
   Smart_Data *sd;
   double repeat;
   sd = data;
   if (!sd) return ECORE_CALLBACK_CANCEL;
   if (sd->layout.pressed)
     {
        _ekbd_layout_key_press_handle(sd, sd->layout.pressed);
     }
   if (sd->down.trepeat > MIN_REPEAT_DELAY)
     {
        repeat = sd->down.trepeat / 4.0;
        if (repeat > MIN_REPEAT_DELAY)
          sd->down.trepeat = repeat;
        else
          sd->down.trepeat = MIN_REPEAT_DELAY;
        sd->down.repeat = ecore_timer_add(sd->down.trepeat,
                                          _ekbd_layout_cb_repeat, sd);
        return ECORE_CALLBACK_CANCEL;
     }
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_ekbd_layout_cb_hold_timeout(void *data)
{
   Smart_Data *sd;
   Ekbd_Int_Key *ky;

   sd = data;
   sd->down.hold_timeout = NULL;
   if (sd->layout.pressed)
     {
        if (sd->down.tie)
          ky = _ekbd_layout_at_coord_get(sd->down.tie->keys,
                                         sd->down.cx, sd->down.cy);
        else
          ky = _ekbd_layout_at_coord_get(sd->layout.keys,
                                         sd->down.cx, sd->down.cy);
        if (ky == sd->layout.pressed)
          {
             _ekbd_layout_key_press_handle(sd, ky);
             sd->down.hold = EINA_TRUE;
             sd->down.trepeat = REPEAT_DELAY;
             sd->down.repeat = ecore_timer_add(REPEAT_DELAY,
                                               _ekbd_layout_cb_repeat, sd);
          }
     }
   return ECORE_CALLBACK_CANCEL;
}

static void
_ekbd_layout_cb_mouse_down(void *data, Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event)
{
   Evas_Event_Mouse_Down *ev;
   Smart_Data *sd;
   Ekbd_Int_Key *ky;
   Evas_Coord x, y, w, h;

   ev = event;
   if (ev->button != 1) return;
   sd = data;
   sd->down.x = ev->canvas.x;
   sd->down.y = ev->canvas.y;
   sd->down.down = EINA_TRUE;
   sd->down.stroke = EINA_FALSE;

   evas_object_geometry_get(sd->event_obj, &x, &y, &w, &h);
   x = sd->x + ev->canvas.x - x;
   y = sd->y + ev->canvas.y - y;
   sd->down.cx = x;
   sd->down.cy = y;

   if (sd->down.tie)
     ky = _ekbd_layout_at_coord_get(sd->down.tie->keys, x, y);
   else
     ky = _ekbd_layout_at_coord_get(sd->layout.keys, x, y);
   sd->layout.pressed = ky;
   if (sd->down.hold_timeout)
     {
        ecore_timer_del(sd->down.hold_timeout);
        sd->down.hold_timeout = NULL;
     }
   if (ky)
     {
        ky->pressed = EINA_TRUE;
        evas_object_raise(ky->obj);
        evas_object_raise(sd->event_obj);
        edje_object_signal_emit(ky->obj, "e,state,pressed", "e");
        if (!ky->is_shift && !ky->is_multi_shift && !ky->is_ctrl && !ky->is_alt
            && !ky->is_altgr && !ky->is_capslock)
          sd->down.hold_timeout = ecore_timer_add(HOLD_TIMEOUT_DELAY,
                                                  _ekbd_layout_cb_hold_timeout, sd);
     }
}

static void
_ekbd_layout_cb_mouse_move(void *data, Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event)
{
   Evas_Event_Mouse_Move *ev;
   Smart_Data *sd;
   Evas_Coord x, y, w, h;

   ev = event;
   sd = data;

   if (sd->down.down)
     {
        evas_object_geometry_get(sd->event_obj, &x, &y, &w, &h);
        x = sd->x + ev->cur.canvas.x - x;
        y = sd->y + ev->cur.canvas.y - y;
        sd->down.cx = x;
        sd->down.cy = y;
        _ekbd_layout_keydown_update(sd);
     }
   /*
      if (sd->down.stroke) return;
      dx = ev->cur.canvas.x - sd->down.x;
      dy = ev->cur.canvas.y - sd->down.y;
      */
}

static void
_ekbd_layout_cb_mouse_up(void *data, Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event)
{
   Evas_Event_Mouse_Up *ev;
   Smart_Data *sd;
   Ekbd_Int_Key *ky;

   ev = event;
   if (ev->button != 1) return;
   sd = data;

   if (!sd->down.down)
     {
       Evas_Coord x, y, w, h;
       sd->down.x = ev->canvas.x;
       sd->down.y = ev->canvas.y;
       sd->down.down = EINA_TRUE;
       sd->down.stroke = EINA_FALSE;

       evas_object_geometry_get(sd->event_obj, &x, &y, &w, &h);
       x = sd->x + ev->canvas.x - x;
       y = sd->y + ev->canvas.y - y;
       sd->down.cx = x;
       sd->down.cy = y;
       if (sd->down.tie)
         ky = _ekbd_layout_at_coord_get(sd->down.tie->keys, x, y);
       else
         ky = _ekbd_layout_at_coord_get(sd->layout.keys, x, y);
       if (ky)
         edje_object_signal_emit(ky->obj, "e,state,pressed", "e");
       sd->layout.pressed = ky;
     }

   if (sd->down.tie && !sd->down.hold)
     _ekbd_layout_tie_del(sd);
   ky = sd->layout.pressed;
   if (ky)
     {
        edje_object_signal_emit(ky->obj, "e,state,released", "e");
        ky->pressed = EINA_FALSE;
        if (!sd->down.hold) _ekbd_layout_key_press_handle(sd, ky);
        sd->layout.pressed = NULL;
     }
   if (sd->down.hold_timeout)
     {
        ecore_timer_del(sd->down.hold_timeout);
        sd->down.hold_timeout = NULL;
     }
   if (sd->down.repeat)
     {
        ecore_timer_del(sd->down.repeat);
        sd->down.repeat = NULL;
     }
   sd->down.down = EINA_FALSE;
   sd->down.hold = EINA_FALSE;
   sd->down.stroke = EINA_FALSE;
}

static void
_ekbd_layout_key_press_handle(Smart_Data *sd, Ekbd_Int_Key *ky)
{
   Ekbd_Int_Key_State *st;

   if (ky->is_shift)
     {
        if (sd->layout.state & SHIFT) sd->layout.state &= (~(SHIFT));
        else sd->layout.state |= SHIFT;
        _ekbd_layout_state_update(sd);
        return;
     }
   else if (ky->is_multi_shift)
     {
        if (sd->layout.state & SHIFT)
          {
             sd->layout.state &= (~(SHIFT));
             sd->layout.state |= CAPSLOCK;
          }
        else if (sd->layout.state & CAPSLOCK)
          sd->layout.state &= (~(CAPSLOCK));
        else
          sd->layout.state |= SHIFT;
        _ekbd_layout_state_update(sd);
        return;
     }
   else if (ky->is_ctrl)
     {
        if (sd->layout.state & CTRL) sd->layout.state &= (~(CTRL));
        else sd->layout.state |= CTRL;
        _ekbd_layout_state_update(sd);
        return;
     }
   else if (ky->is_alt)
     {
        if (sd->layout.state & ALT) sd->layout.state &= (~(ALT));
        else sd->layout.state |= ALT;
        _ekbd_layout_state_update(sd);
        return;
     }
   else if (ky->is_altgr)
     {
        if (sd->layout.state & ALTGR) sd->layout.state &= (~(ALTGR));
        else sd->layout.state |= ALTGR;
        _ekbd_layout_state_update(sd);
        return;
     }
   else if (ky->is_capslock)
     {
        if (sd->layout.state & CAPSLOCK) sd->layout.state &= (~(CAPSLOCK));
        else sd->layout.state |= CAPSLOCK;
        _ekbd_layout_state_update(sd);
        return;
     }

   st = _ekbd_layout_key_state_get(sd, ky);
   if (st->tie || st->lp_tie)
     {
        if (!sd->down.hold)
          {
             if (st->lp_tie)
               sd->down.tie = st->lp_tie;
             else
               sd->down.tie = st->tie;
             _ekbd_layout_tie_build(sd);
          }
     }
   else if (st->layout && !sd->down.hold)
     {
        ekbd_layout_select(sd, st->layout);
        ekbd_layout_keys_calc(sd);
     }
   else
     {
        const char *out;

        if(sd->down.hold && st->lp_out)
           out = st->lp_out;
        else
           out = st->out;

        if (out)
          {
             Ekbd_Mod mods = 0;
             if (sd->layout.state & CTRL) mods |= EKBD_MOD_CTRL;
             if (sd->layout.state & ALT) mods |= EKBD_MOD_ALT;

             if (out[0] == '"')
                ekbd_send_string_press(out, mods);
             else
                ekbd_send_keysym_press(out, mods);
          }

        if (sd->layout.state & (SHIFT | CTRL | ALT | ALTGR))
          {
             /* Clearing states */
             sd->layout.state &= (~(SHIFT | CTRL | ALT | ALTGR));
             _ekbd_layout_state_update(sd);
          }
     }
}

static Ekbd_Int_Key *
_ekbd_layout_at_coord_get(Eina_List *keys, Evas_Coord x, Evas_Coord y)
{
   Eina_List *l;
   Ekbd_Int_Key *ky, *closest_ky = NULL;

   EINA_LIST_FOREACH(keys, l, ky)
      if ((x >= ky->x) && (y >= ky->y) &&
        (x < (ky->x + ky->w)) && (y < (ky->y + ky->h)))
        return ky;
   return closest_ky;
}


static void
_ekbd_layout_tie_calc(Smart_Data *sd)
{
   double rw, rh, rr;
   Eina_List *l;
   Ekbd_Int_Key *ky;
   Evas_Coord x, y, ex, ey, ew, eh;
   Ekbd_Int_Tie *kt = sd->down.tie;

   rw = sd->w / (double)sd->layout.w;
   rh = sd->h / (double)sd->layout.h;
   if (rw < rh) rr = rw;
   else rr = rh;

   edje_object_size_min_calc(kt->base_obj, &ew, &eh);
   x = (kt->key->x + (kt->key->w / 2)) - ((rw * kt->w) / 2);
   y = kt->key->y - (rh * kt->h);
   if (y < sd->y)
     {
        y = kt->key->y + kt->key->h;
        edje_object_signal_emit(kt->base_obj, "ekbd.state.up", "");
     }
   else
     edje_object_signal_emit(kt->base_obj, "ekbd.state.down", "");
   if (x < sd->x)
     x = sd->x;
   else if ((x + (rw * kt->w) + ew) > (sd->x + sd->w))
     x = (sd->x + sd->w) - (rw * kt->w) - ew;

   evas_object_move(sd->cover_obj, sd->x, sd->y);
   evas_object_resize(sd->cover_obj, sd->w, sd->h);
   evas_object_move(kt->base_obj, x, y);
   evas_object_resize(kt->base_obj,  (rw * kt->w) + ew, (rh * kt->h) + eh);

   edje_object_part_drag_value_set(kt->base_obj, "cur.phylactere",
                                   (kt->key->x + ((kt->key->w ) / 2.0) - x)
                                   / (double)(kt->w * rw),
                                   0.0);
   edje_object_part_geometry_get(kt->base_obj, "e.swallow.content", &ex, &ey,
                                 NULL, NULL);
   x = x + ex;
   y = y + ey;

   EINA_LIST_FOREACH(sd->down.tie->keys, l, ky)
     {
        ky->x = x + (rw * ky->orig_x);
        ky->y = y + (rh * ky->orig_y);
        ky->w = ky->orig_w * rw;
        ky->h = ky->orig_h * rh;
        edje_object_scale_set(ky->obj, rr);
        evas_object_move(ky->obj, ky->x, ky->y);
        evas_object_resize(ky->obj, ky->w, ky->h);
     }
}



void
ekbd_layout_keys_calc(Smart_Data *sd)
{
   double rw, rh, rr;
   Eina_List *l;
   Ekbd_Int_Key *ky;

   rw = sd->w / (double)sd->layout.w;
   rh = sd->h / (double)sd->layout.h;
   if (rw < rh)
     {
        rr = rw;
        //evas_object_size_hint_min_set(sd->layout_obj, sd->w, sd->layout.h * rw);
     }
   else
     {
        rr = rh;
        //evas_object_size_hint_min_set(sd->layout_obj, sd->layout.w * rh, sd->h);
     }
   evas_object_move(sd->event_obj, sd->x, sd->y);
   evas_object_resize(sd->event_obj, sd->w, sd->h);

   EINA_LIST_FOREACH(sd->layout.keys, l, ky)
     {
        ky->x = sd->x + (rw * ky->orig_x);
        ky->y = sd->y + (rh * ky->orig_y);
        ky->w = ky->orig_w * rw;
        ky->h = ky->orig_h * rh;
        edje_object_scale_set(ky->obj, rr);
        evas_object_move(ky->obj, ky->x, ky->y);
        evas_object_resize(ky->obj, ky->w, ky->h);
     }
   if (sd->down.tie)
     _ekbd_layout_tie_calc(sd);
}

