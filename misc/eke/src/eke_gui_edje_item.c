/**
 * smart template.  i stole this from rephorm and made it check the
 * data_get calls for some validity.
 *
 * There's example structs in the #if0 block you can easily copy to a
 * header(mysmartobj.h)
 *
 */
#include <Evas.h>
#include <Edje.h>
#include <Etox.h>
#include <Ecore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "eke_gui_edje_item.h"

Evas_Object*
eke_gui_edje_item_new(Evas *e, const char *file, const char *group);
/* smart object handlers */
static Evas_Smart * _eke_gui_edje_item_object_smart_get();
static Evas_Object * eke_gui_edje_item_object_new(Evas *evas);
void _eke_gui_edje_item_object_add(Evas_Object *o);
void _eke_gui_edje_item_object_del(Evas_Object *o);
void _eke_gui_edje_item_object_layer_set(Evas_Object *o, int l);
void _eke_gui_edje_item_object_raise(Evas_Object *o);
void _eke_gui_edje_item_object_lower(Evas_Object *o);
void _eke_gui_edje_item_object_stack_above(Evas_Object *o, Evas_Object *above);
void _eke_gui_edje_item_object_stack_below(Evas_Object *o, Evas_Object *below);
void _eke_gui_edje_item_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
void _eke_gui_edje_item_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
void _eke_gui_edje_item_object_show(Evas_Object *o);
void _eke_gui_edje_item_object_hide(Evas_Object *o);
void _eke_gui_edje_item_object_color_set(Evas_Object *o, int r, int g, int b, int a);
void _eke_gui_edje_item_object_clip_set(Evas_Object *o, Evas_Object *clip);
void _eke_gui_edje_item_object_clip_unset(Evas_Object *o);

static void
_open_feed(void *data, Evas_Object *obj, const char *em, const char *src) 
{
    if(data) {
        char buf[PATH_MAX];
        snprintf(buf, PATH_MAX, "firefox -remote %s", (char*)data);
        printf("%s", buf);
        ecore_exe_run(buf, NULL);
    }

    return;
    obj = NULL;
    em = NULL;
    src = NULL;
}
void
eke_gui_edje_item_init(Evas_Object *o, const char *label, const char *link,
const char *body)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    if(edje_object_part_exists(data->obj, "label"))
    {
        if(label) {
            edje_object_part_text_set(data->obj, "label", label);
        } else {
            edje_object_part_text_set(data->obj, "label", "");
        }
    }
    if(edje_object_part_exists(data->obj, "link"))
    {
        if(link) {
            edje_object_part_text_set(data->obj, "link", link);
        } else {
            edje_object_part_text_set(data->obj, "link", "");
        }
    }
    if(edje_object_part_exists(data->obj, "body"))
    {
        if(body) {
            Evas_Object *desc;
            Etox_Context *ctx;

            desc = etox_new(evas_object_evas_get(o));
            ctx = etox_get_context(desc);
            etox_context_set_color(ctx, 0, 0, 0, 255);
            etox_context_set_font(ctx, "Vera", 10);
            etox_set_context(desc, ctx);

            etox_set_soft_wrap(desc, 1);
            etox_set_word_wrap(desc, 1);
            etox_set_text(desc, (char *)body);
            evas_object_show(desc);
            edje_object_part_swallow(data->obj, "body", desc);
        } else {
        }
    }
    edje_object_signal_callback_add(data->obj, "eke,link,open", "",
                                                            _open_feed,
                                                            (void*)link);
  }
}

Evas_Object*
eke_gui_edje_item_new(Evas *e, const char *file, const char *group)
{
  char buf[PATH_MAX], buf2[PATH_MAX];
  Evas_Object *result = NULL;
  Eke_Gui_Edje_Item *data = NULL;

  snprintf(buf, PATH_MAX, "%s", file);
  snprintf(buf2, PATH_MAX, "%s", group);
//    printf(stderr, "NEW:%s,%s\n", file, group);
  if((result = eke_gui_edje_item_object_new(e)))
  {
    if((data = evas_object_smart_data_get(result)))
    {
        data->obj = edje_object_add(e);
//  printf("%s\n", buf);
//  printf("%s\n", buf2);
        if(edje_object_file_set(data->obj, file, group) == 1)
        {
                evas_object_move(data->obj, -250, 80);
                evas_object_resize(data->obj, 350, 80);
        } else {
            fprintf(stderr, "edje_file_set_error %d\n", 
                    edje_object_load_error_get(data->obj));
                evas_object_del(data->obj);
                evas_object_del(result);
                result = NULL;
        }
    }
  }
  return(result);
}

/*==========================================================================
 * Smart Object Code, Go Away
 *========================================================================*/


/*** external API ***/

static Evas_Object *
eke_gui_edje_item_object_new(Evas *evas)
{
  Evas_Object *eke_gui_edje_item_object;

  eke_gui_edje_item_object = evas_object_smart_add(evas, _eke_gui_edje_item_object_smart_get());

  return eke_gui_edje_item_object;
}

/*** smart object handler functions ***/

static Evas_Smart *
_eke_gui_edje_item_object_smart_get()
{
  static Evas_Smart *smart = NULL;
  if(smart) return(smart);

  smart = evas_smart_new ("eke_gui_edje_item_object",
                          _eke_gui_edje_item_object_add,
                          _eke_gui_edje_item_object_del,
                          _eke_gui_edje_item_object_layer_set,
                          _eke_gui_edje_item_object_raise,
                          _eke_gui_edje_item_object_lower,
                          _eke_gui_edje_item_object_stack_above,
                          _eke_gui_edje_item_object_stack_below,
                          _eke_gui_edje_item_object_move,
                          _eke_gui_edje_item_object_resize,
                          _eke_gui_edje_item_object_show,
                          _eke_gui_edje_item_object_hide,
                          _eke_gui_edje_item_object_color_set,
                          _eke_gui_edje_item_object_clip_set,
                          _eke_gui_edje_item_object_clip_unset,
                          NULL
                          );

  return smart; 
}

void
_eke_gui_edje_item_object_add(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data = NULL;
    
  data = malloc(sizeof(Eke_Gui_Edje_Item));
  memset(data, 0, sizeof(Eke_Gui_Edje_Item));
  evas_object_smart_data_set(o, data);
}


void
_eke_gui_edje_item_object_del(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    free(data);
  }
}

void
_eke_gui_edje_item_object_layer_set(Evas_Object *o, int l)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_layer_set(data->obj, l);
  }
}

void
_eke_gui_edje_item_object_raise(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_raise(data->obj);
  }
}

void
_eke_gui_edje_item_object_lower(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_lower(data->obj);
  }
}

void
_eke_gui_edje_item_object_stack_above(Evas_Object *o, Evas_Object *above)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_stack_above(data->obj, above);
  }
}

void
_eke_gui_edje_item_object_stack_below(Evas_Object *o, Evas_Object *below)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_stack_below(data->obj, below);
  }
}

void
_eke_gui_edje_item_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_move(data->obj, x, y);
  }
}

void
_eke_gui_edje_item_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
  Eke_Gui_Edje_Item *data;
  Evas_Object *obj = NULL;

  if((data = evas_object_smart_data_get(o)))
  {
      printf("resizing to %d,%d\n", w, h);
    evas_object_resize(data->obj, w, h);
    if((obj = edje_object_part_swallow_get(data->obj, "body"))) {
      printf("resizing etox to %d,%d\n", w, h);
        evas_object_resize(obj, w, h);
    }
  }
}

void
_eke_gui_edje_item_object_show(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
      evas_object_show(data->obj);
  }
}

void
_eke_gui_edje_item_object_hide(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_hide(data->obj);
  }
}

void
_eke_gui_edje_item_object_color_set(Evas_Object *o, int r, int g, int b, int a)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_color_set(data->obj, r, g, b, a);
  }
}

void
_eke_gui_edje_item_object_clip_set(Evas_Object *o, Evas_Object *clip)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_clip_set(data->obj, clip);
  }
}

void
_eke_gui_edje_item_object_clip_unset(Evas_Object *o)
{
  Eke_Gui_Edje_Item *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    evas_object_clip_unset(data->obj);
  }
}
